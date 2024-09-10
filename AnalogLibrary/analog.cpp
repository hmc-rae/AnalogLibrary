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
    char modType;

    // TODO: some heat val
};

typedef struct cell {
    CELL_TYPE charge;

    char config;

    connect connections[CONNECTION_COUNT];
};

cell* cells;
double timestep;
int MAX, xMax, yMax, zMax, XYMax;

int noiseProfile;

CELL_TYPE underbusCharge;

int get_mem_pos(int x, int y, int z) {
    // use Z as the largest factor, Y medium, X smallest
    return x
        + (y * xMax)
        + (z * XYMax);
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
}

int Lattice_Program_SetUnderbus(CELL_TYPE charge) {
    underbusCharge = charge;
}

int Lattice_Program_Core(int X, int Y, int Z, int code) {
    int idx = get_mem_pos(X, Y, Z);

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
    
}