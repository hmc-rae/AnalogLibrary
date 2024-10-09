// LinearSearch.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stdlib.h>
#include <Windows.h>
#include "AnalogLibrary.h"
#include <vector>
#include <random>

using namespace std;

#define MAX_VALUE (int)128

vector<int> creation_array = { 10, 50, 32, 64, 2, 8, 3 };

std::default_random_engine rng;

int initialize_sim(vector<int> data) {
    int size = data.size();
    if (SIMU_Lattice_Init(7, size*2, 4, 0, 0.01)) {
        cout << "Failed to initialize lattice!" << endl;
        return -1;
    }
    else {
        cout << "Simulation initialized." << endl;
        cout << "Simulation spans a region (7, " << (size * 2) << ", 4) (" << (7 * size * 2 * 4) << " cells) \n";
    }

    for (int i = 0, y = 0; i < size; i++, y += 2) {
        // Set up the input carry
        Lattice_Program_Core(1, y + 0, 1, LATTICE_PROG_CORE_SUM);
        Lattice_Program_Core(1, y + 1, 1, LATTICE_PROG_CORE_SUM);

        Lattice_Program_Connect(1, y + 0, 1,
            LATTICE_PROG_CONNECT_NY |
            LATTICE_PROG_CONNECT_CONFIG_FLOW_POS);
        Lattice_Program_Connect(1, y + 1, 1,
            LATTICE_PROG_CONNECT_NY |
            LATTICE_PROG_CONNECT_CONFIG_FLOW_POS);

        // Write the value initial store
        Lattice_Program_SetUnderbus(data[i], MAX_VALUE);
        Lattice_Program_Core(2, y, 1, LATTICE_PROG_CORE_HOLDVAL);
        Lattice_Program_SetUnderbus(1);
        Lattice_Program_Core(3, y, 1, LATTICE_PROG_CORE_HOLDVAL);

        // Write C1 and C2 cells
        // C1
        Lattice_Program_Core(2, y + 1, 1, LATTICE_PROG_CORE_SUM);
        Lattice_Program_Connect(2, y + 1, 1,
            LATTICE_PROG_CONNECT_NX |
            LATTICE_PROG_CONNECT_CONFIG_FLOW_POS |
            LATTICE_PROG_CONNECT_CONFIG_INVERT);
        Lattice_Program_Connect(2, y + 1, 1,
            LATTICE_PROG_CONNECT_NY |
            LATTICE_PROG_CONNECT_CONFIG_FLOW_POS);

        // C2
        Lattice_Program_Core(3, y + 1, 1, LATTICE_PROG_CORE_SUM);
        Lattice_Program_SetUnderbus(0);
        Lattice_Program_Connect(3, y + 1, 1,
            LATTICE_PROG_CONNECT_NX |
            LATTICE_PROG_CONNECT_CONFIG_FLOW_POS |
            LATTICE_PROG_CONNECT_CONFIG_MOD_COMP |
            LATTICE_PROG_CONNECT_CONFIG_ABSOLUTE |
            LATTICE_PROG_CONNECT_CONFIG_INVERT);
        Lattice_Program_Connect(3, y + 1, 1,
            LATTICE_PROG_CONNECT_NY |
            LATTICE_PROG_CONNECT_CONFIG_FLOW_POS);

        // Signal Base
        Lattice_Program_Core(4, y + 1, 1, LATTICE_PROG_CORE_SUM);
        Lattice_Program_Connect(4, y + 1, 1,
            LATTICE_PROG_CONNECT_NX |
            LATTICE_PROG_CONNECT_CONFIG_FLOW_POS);

        // Write value secondary store + index store
        Lattice_Program_SetUnderbus(data[i], MAX_VALUE);
        Lattice_Program_Core(3, y + 1, 0, LATTICE_PROG_CORE_HOLDVAL);
        Lattice_Program_SetUnderbus(i, MAX_VALUE);
        Lattice_Program_Core(3, y + 1, 2, LATTICE_PROG_CORE_HOLDVAL);

        // Write multipliers against S and the two stores
        Lattice_Program_Core(4, y + 1, 0, LATTICE_PROG_CORE_MULT);
        Lattice_Program_Core(4, y + 1, 2, LATTICE_PROG_CORE_MULT);

        Lattice_Program_Connect(4, y + 1, 0,
            LATTICE_PROG_CONNECT_NX |
            LATTICE_PROG_CONNECT_CONFIG_FLOW_POS);
        Lattice_Program_Connect(4, y + 1, 0,
            LATTICE_PROG_CONNECT_PZ |
            LATTICE_PROG_CONNECT_CONFIG_FLOW_NEG);

        Lattice_Program_Connect(4, y + 1, 2,
            LATTICE_PROG_CONNECT_NX |
            LATTICE_PROG_CONNECT_CONFIG_FLOW_POS);
        Lattice_Program_Connect(4, y + 1, 2,
            LATTICE_PROG_CONNECT_NZ |
            LATTICE_PROG_CONNECT_CONFIG_FLOW_POS);

        // Signal lines on x = 5
        Lattice_Program_Core(5, y + 1, 0, LATTICE_PROG_CORE_SUM);
        Lattice_Program_Core(5, y + 1, 1, LATTICE_PROG_CORE_SUM);
        Lattice_Program_Core(5, y + 1, 2, LATTICE_PROG_CORE_SUM);
        Lattice_Program_Connect(5, y + 1, 0,
            LATTICE_PROG_CONNECT_NX | LATTICE_PROG_CONNECT_CONFIG_FLOW_POS);
        Lattice_Program_Connect(5, y + 1, 1,
            LATTICE_PROG_CONNECT_NX | LATTICE_PROG_CONNECT_CONFIG_FLOW_POS | LATTICE_PROG_CONNECT_CONFIG_INVERT);
        Lattice_Program_Connect(5, y + 1, 2,
            LATTICE_PROG_CONNECT_NX | LATTICE_PROG_CONNECT_CONFIG_FLOW_POS);

        // Write signal lines back to y=0
        Lattice_Program_Core(5, y + 0, 0, LATTICE_PROG_CORE_MULT);
        Lattice_Program_Connect(5, y + 0, 0,
            LATTICE_PROG_CONNECT_PY | LATTICE_PROG_CONNECT_CONFIG_FLOW_NEG);
        Lattice_Program_Connect(5, y + 1, 0,
            LATTICE_PROG_CONNECT_PY | LATTICE_PROG_CONNECT_CONFIG_FLOW_NEG);

        Lattice_Program_Core(5, y + 0, 2, LATTICE_PROG_CORE_MULT);
        Lattice_Program_Connect(5, y + 0, 2,
            LATTICE_PROG_CONNECT_PY | LATTICE_PROG_CONNECT_CONFIG_FLOW_NEG);
        Lattice_Program_Connect(5, y + 1, 2,
            LATTICE_PROG_CONNECT_PY | LATTICE_PROG_CONNECT_CONFIG_FLOW_NEG);

        // Write cancel signal base
        Lattice_Program_Core(5, y + 1, 1, LATTICE_PROG_CORE_SUM);
        Lattice_Program_SetUnderbus(1);
        Lattice_Program_Core(6, y + 1, 1, LATTICE_PROG_CORE_HOLDVAL);
        Lattice_Program_Connect(5, y + 1, 1,
            LATTICE_PROG_CONNECT_NX | LATTICE_PROG_CONNECT_CONFIG_FLOW_POS | LATTICE_PROG_CONNECT_CONFIG_INVERT);
        Lattice_Program_Connect(5, y + 1, 1,
            LATTICE_PROG_CONNECT_PX | LATTICE_PROG_CONNECT_CONFIG_FLOW_NEG);

        continue;
        // Connect cancel signal to y = 0 signal layers 
        Lattice_Program_Core(5, y, 1, LATTICE_PROG_CORE_SUM);
        Lattice_Program_Connect(5, y, 1,
            LATTICE_PROG_CONNECT_PY | LATTICE_PROG_CONNECT_CONFIG_FLOW_NEG);
        Lattice_Program_Connect(5, y, 1,
            LATTICE_PROG_CONNECT_PZ | LATTICE_PROG_CONNECT_CONFIG_FLOW_POS);
        Lattice_Program_Connect(5, y, 1,
            LATTICE_PROG_CONNECT_NZ | LATTICE_PROG_CONNECT_CONFIG_FLOW_NEG);
    }

    // Connect to layer 6 the input/outputs
    Lattice_Program_Core(6, 0, 0, LATTICE_PROG_CORE_SUM);
    Lattice_Program_Core(6, 0, 2, LATTICE_PROG_CORE_SUM);

    Lattice_Program_Connect(6, 0, 0, LATTICE_PROG_CONNECT_NX | LATTICE_PROG_CONNECT_CONFIG_FLOW_POS);
    Lattice_Program_Connect(6, 0, 2, LATTICE_PROG_CONNECT_NX | LATTICE_PROG_CONNECT_CONFIG_FLOW_POS);

    // Connect to layer 0 at z=1 the input

    Lattice_Program_Connect(1, 0, 1, LATTICE_PROG_CONNECT_NX | LATTICE_PROG_CONNECT_CONFIG_FLOW_POS);

    Sleep(1000);
    return 0;
}

int main()
{
    if (initialize_sim(creation_array)) {
        cout << "Failed to initialize simulation!" << endl;
        return -1;
    }
    else {
        cout << "Simulation programmed!" << endl;
    }

    cout << "Simulation running at approximately " << SIMU_Poll_Rate() << " Hz" << endl;

    // Test values: have to sleep for a tiny amount of time to get the output.

    // Check for every number in creation_array
    for (int i = 0; i < creation_array.size(); i++) {
        cout << endl;
        cout << "Looking for value " << creation_array[i] << "..." << endl;
        float tmp = 0;
        SIMU_Lattice_Examine(3, (2*i) + 1, 0, &tmp);
        cout << "Expecting value " << tmp << " * " << MAX_VALUE << " (" << (tmp * MAX_VALUE) << ")" << endl;
        Lattice_Write(0, 1, creation_array[i], MAX_VALUE);
        Sleep(100);
        int a, b = 0;
        Lattice_Read(0, 0, MAX_VALUE, &a);
        Lattice_Read(0, 2, MAX_VALUE, &b);
        cout << "Found value " << a << " at index " << b << endl;

        SIMU_Lattice_Examine(4, (2 * i) + 1, 1, &tmp);
        cout << "Signal value at cell is " << tmp << endl;
        SIMU_Lattice_Examine(4, (2 * i) + 1, 0, &tmp);
        cout << "Z_0 mult value is " << tmp << endl;
        SIMU_Lattice_Examine(5, (2 * i) + 1, 0, &tmp);
        cout << "Z_0 signal layer value is " << tmp << endl;
    }

    // Check for numbers that arent there
    cout << endl << "Looking for value 72" << endl;
    Lattice_Write(0, 1, 72, MAX_VALUE);
    Sleep(100);
    int a, b = 0;
    Lattice_Read(0, 0, MAX_VALUE, &a);
    Lattice_Read(0, 2, MAX_VALUE, &b);
    cout << "Found value " << a << " at index " << b << endl << endl;

    SIMU_Lattice_Destroy();
    cout << endl << "Simulation destroyed." << endl << endl;
    return 0;
}
