/*
    Analog Lattice Library
    by Harris C. McRae, 2024
*/
#include "pch.h"
#include "AnalogLibrary.h"
#include <malloc.h>
#include <thread>

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

#ifdef OPTIM_CONNECTIONS
    #define CONNECTION_COUNT 3
#else
    #define CONNECTION_COUNT 6
#endif

typedef struct connect {
    CELL_TYPE modifier;
    char config;

    // TODO: some heat val
};

typedef struct cell {
    CELL_TYPE charge;
    bool flipswitch;
    char config;

    connect connections[CONNECTION_COUNT];
};

cell* cells;
double timestep;
int MAX, xMax, yMax, zMax, XYMax;

int noiseProfile;

CELL_TYPE underbusCharge;

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