// LatticeTestProg.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "AnalogLibrary.h"

int main()
{
    if (SIMU_Lattice_Init(2, 2, 2, 0, 0)) {
        std::cout << "Failed to initialize lattice!\n";
        return -1;
    }

    std::cout << "Lattice initialized.\n";

    return 0;
}
