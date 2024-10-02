// LatticeTestProg.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stdlib.h>
#include <Windows.h>
#include "AnalogLibrary.h"

int main()
{
    if (SIMU_Lattice_Init(2, 1, 1, 0, (CELL_TYPE)1/10)) {
        std::cout << "Failed to initialize lattice!\n";
        return -1;
    }

    Lattice_Program_Core(1, 0, 0, LATTICE_PROG_CORE_INT);
    Lattice_Program_Connect(1, 0, 0, LATTICE_PROG_CONNECT_NX |
        LATTICE_PROG_CONNECT_CONFIG_ACTIVE | LATTICE_PROG_CONNECT_CONFIG_FLOW_POS);

    CELL_TYPE v = 0.5;
    Lattice_Write(0, 0, v);
    v = 0;
    SIMU_Lattice_Examine(0, 0, 0, &v);
    std::cout << "Input value is " << v << "\n";
    Sleep(1500);
    Lattice_Read(0, 0, &v);
    std::cout << "Output value is " << v << "\n";

    SIMU_Lattice_Destroy();
    return 0;
}
