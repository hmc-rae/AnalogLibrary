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
    CELL_TYPE cValue;
    CELL_TYPE pValue;

    char config;

    connect connections[CONNECTION_COUNT];
};

cell* cells;
int MAX, xMax, yMax, zMax;

int noiseProfile;

int SIMU_Lattice_Init(int X, int Y, int Z, int noise, double ts) {
    xMax = X;
    yMax = Y;
    zMax = Z;

    MAX = X * Y * Z;
    noiseProfile = noise;
    cells = new cell[MAX];
}

