/*
    Analog Lattice Library
    by Harris C. McRae, 2024
*/
#include "pch.h"
#include "AnalogLibrary.h"
#include <malloc.h>
#include <thread>
#include <vector>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

#define POS_X 0
#define POS_Y 1
#define POS_Z 2
#define NEG_X 3
#define NEG_Y 4
#define NEG_Z 5

#define OPTIM_CONNECTIONS
#define ALL_CONNECTIONS 6
#ifdef OPTIM_CONNECTIONS
    #define CONNECTION_COUNT 3
#else
    #define CONNECTION_COUNT 6
#endif

#define abs(x) (x < 0 ? -x : x)

typedef struct connect {
    CELL_TYPE modifier;
    char config;

    // TODO: some heat val
};

typedef struct cell {
    CELL_TYPE charge;
    bool flipswitch;
    char config;
    int x, y, z;
    connect connections[CONNECTION_COUNT];
};

cell* cells;
double timestep;
int MAX, xMax, yMax, zMax, XYMax;

int connectionDelta[ALL_CONNECTIONS];

int noiseProfile;

CELL_TYPE underbusCharge;

int _simu_running;
std::thread _simu_thread;
std::vector<int> _simu_integrators;
std::vector<int> _simu_endpoints;

/// <summary>
/// gets the position in memory corresponding to the given values
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="z"></param>
/// <returns></returns>
int get_mem_pos(int x, int y, int z) {
    // use Z as the largest factor, Y medium, X smallest
    return x
        + (y * xMax)
        + (z * XYMax);
}
/// <summary>
/// returns the connection to be modified
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="z"></param>
/// <param name="connection"></param>
/// <param name="ret"></param>
/// <returns></returns>
int get_connection(int x, int y, int z, int connection, connect** ret) {
    int idx = get_mem_pos(x, y, z);
    if (idx < 0 || idx >= MAX) return -1;
    if (connection < 0 || connection > CONNECTION_COUNT) return -2;

#ifdef OPTIM_CONNECTIONS
    if (connection < 3) {
#endif
        cell* cell = &cells[idx];
        *ret = &cell->connections[connection];
        return 0;
#ifdef OPTIM_CONNECTIONS
    }
    else {
        switch (connection) {
        case NEG_X:
            x -= 1;
            break;
        case NEG_Y:
            y -= 1;
            break;
        case NEG_Z:
            z -= 1;
            break;
        }
        return get_connection(x, y, z, connection - 3, ret);
    }
#endif
}
int get_connection(int idx, int connection, connect** ret) {
    return 0;
}
int get_is_connection_to_me(int oX, int oY, int oZ, int connection) {
    connect* connector = 0;
    get_connection(oX, oY, oZ, connection, &connector);

    if (!(connector->config & LATTICE_PROG_CONNECT_CONFIG_ACTIVE)) return 0;

    // Get the connection: if it is on a negative axis (>2) and is to positive, its to me, OR if it is on positive axis <3 and to negative
    if ((connection < 3 && (connector->config & LATTICE_PROG_CONNECT_CONFIG_FLOW_NEG)) ||
        (connection > 2 && !(connector->config & LATTICE_PROG_CONNECT_CONFIG_FLOW_NEG))) {
        return 1;
    }
    return 0;
}
CELL_TYPE get_value_through_connection(int idx, connect* connection) {
    CELL_TYPE val = cells[idx].charge;
    char config = connection->config;

    if (!config & LATTICE_PROG_CONNECT_CONFIG_ACTIVE) return 0;
    // ignore directionality
    if (config & LATTICE_PROG_CONNECT_CONFIG_MOD) {
        if (config & LATTICE_PROG_CONNECT_CONFIG_MOD_DIVIS) {
            if (connection->modifier == 0) val = LATTICE_DEFAULT_DIV_ZERO;
            else val = val / connection->modifier;
        }
        else {
            val = val * connection->modifier;
        }
    }

    if (config & LATTICE_PROG_CONNECT_CONFIG_ABSOLUTE) 
        val = abs(val);

    if (config & LATTICE_PROG_CONNECT_CONFIG_INVERT)
        val = -val;

    return val;
}
int SIMU_recursive_operate(int idx, bool flip) {
    //if flipped, return instantly as we can assume its been computed
    if (cells[idx].flipswitch == flip)
        return -1;
    cells[idx].flipswitch = flip;

    // check all connections to see if theyre TO this cell, or AWAY from this cell
    CELL_TYPE cell_values[ALL_CONNECTIONS] = { 0, 0, 0, 0, 0, 0 };
    int k = 0;
    for (int i = 0; i < ALL_CONNECTIONS; i++) {
        if (!get_is_connection_to_me(cells[idx].x, cells[idx].y, cells[idx].z, i)) continue;
        SIMU_recursive_operate(idx + connectionDelta[i], flip);
        connect* connector = 0;
        get_connection(cells[idx].x, cells[idx].y, cells[idx].z, i, &connector);
        cell_values[k++] = get_value_through_connection(idx + connectionDelta[i], connector);
    }

    // OPERATE ON ALL VALUES WE RECEIVE IN THIS FRAME
    for (int i = 0; i < k; i++) {

    }

    return 0;
}
int SIMU_Lattice_Run() {
    bool flipswitch = true;
    while (_simu_running) {

    }

    return 0;
}
int SIMU_Lattice_Init(int X, int Y, int Z, int noise, double ts) {
    xMax = X;
    yMax = Y;
    zMax = Z;

    MAX = X * Y * Z;
    XYMax = X * Y;
    noiseProfile = noise;
    timestep = ts;
    cells = new cell[MAX];
    underbusCharge = 0;

    connectionDelta[POS_X] = get_mem_pos(2, 1, 1) - get_mem_pos(1, 1, 1);
    connectionDelta[POS_Y] = get_mem_pos(1, 2, 1) - get_mem_pos(1, 1, 1);
    connectionDelta[POS_Z] = get_mem_pos(1, 1, 2) - get_mem_pos(1, 1, 1);
    connectionDelta[NEG_X] = get_mem_pos(0, 1, 1) - get_mem_pos(1, 1, 1);
    connectionDelta[NEG_Y] = get_mem_pos(1, 0, 1) - get_mem_pos(1, 1, 1);
    connectionDelta[NEG_Z] = get_mem_pos(1, 1, 0) - get_mem_pos(1, 1, 1);

    // TODO: start thread running and checking all active input/output layers
    _simu_running = 1;
    _simu_thread = std::thread(SIMU_Lattice_Run);

    return MAX;
}

int Lattice_Program_SetUnderbus(CELL_TYPE charge) {
    underbusCharge = charge;
    return 0;
}

int Lattice_Program_Core(int X, int Y, int Z, int code) {
    int idx = get_mem_pos(X, Y, Z);
    if (idx < 0 || idx >= MAX) return -1;

    switch (code & LATTICE_PROG_CORE_MASK) {
    case LATTICE_PROG_CORE_HOLDVAL: // HOLDVAL
        cells[idx].charge = underbusCharge;
        cells[idx].config = LATTICE_PROG_CORE_HOLDVAL;
        break;
    case LATTICE_PROG_CORE_SUM:
    case LATTICE_PROG_CORE_MULT:
    case LATTICE_PROG_CORE_INT:
        cells[idx].config = code & LATTICE_PROG_CORE_MASK;
        break;
    }
    return 0;
}

int Lattice_Program_Connection(int X, int Y, int Z, int code) {
    connect* connection = 0;
    int connectionID = code & LATTICE_PROG_CONNECT_MASK;
    if (!get_connection(X, Y, Z, connectionID, &connection))
        return -1;

    connection->config = (code & ~LATTICE_PROG_CONNECT_MASK) & 0xff;

    if ((code & LATTICE_PROG_CONNECT_CONFIG_MOD) != 0) {
        connection->modifier = underbusCharge;
    }

    return 0;
}