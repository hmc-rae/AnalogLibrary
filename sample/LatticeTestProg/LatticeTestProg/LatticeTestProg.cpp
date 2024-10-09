// LatticeTestProg.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stdlib.h>
#include <Windows.h>
#include "AnalogLibrary.h"

int main()
{
    if (SIMU_Lattice_Init(2, 2, 1, 0, (CELL_TYPE)1/10)) {
        std::cout << "Failed to initialize lattice!\n";
        return -1;
    }

    Lattice_Program_Core(1, 0, 0, LATTICE_PROG_CORE_INT);
    Lattice_Program_Connect(1, 0, 0, LATTICE_PROG_CONNECT_NX |
        LATTICE_PROG_CONNECT_CONFIG_FLOW_POS);

    Lattice_Program_Core(1, 1, 0, LATTICE_PROG_CORE_SUM);
    Lattice_Program_SetUnderbus(0);
    Lattice_Program_Connect(1, 1, 0, LATTICE_PROG_CONNECT_NX |
        LATTICE_PROG_CONNECT_CONFIG_FLOW_POS |
        LATTICE_PROG_CONNECT_CONFIG_MOD_COMP);

    CELL_TYPE v = 0.5;
    Lattice_Start_Integration();
    Lattice_Write(0, 0, v);
    v = 0.35;
    Lattice_Write(1, 0, v);

    SIMU_Lattice_Examine(0, 0, 0, &v);
    std::cout << "Examined value at (0, 0, 0) is " << v << std::endl;

    Sleep(1500);
    Lattice_Read(0, 0, &v);
    std::cout << "Output integrator value is " << v << std::endl;
    Lattice_Read(1, 0, &v);
    std::cout << "Output comparator value is " << v << std::endl;

    SIMU_Lattice_Destroy();
    return 0;
}
