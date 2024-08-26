/*
	Analog Lattice Library 
	by Harris C. McRae

	A library for interacting with a simulated general-purpose Analog Lattice. 

*/

#pragma once

// Information declarations
#define SIMU_FUNC_DEFINED 1

// Core program flags
#define LATTICE_PROG_CORE_MASK 4
#define LATTICE_PROG_CORE_HOLDVAL 0
#define LATTICE_PROG_CORE_SUM 1
#define LATTICE_PROG_CORE_MULT 2
#define LATTICE_PROG_CORE_INT 3

// Connection selectors
#define LATTICE_PROG_CONNECT_MASK 7
#define LATTICE_PROG_CONNECT_PX 1
#define LATTICE_PROG_CONNECT_NX 2
#define LATTICE_PROG_CONNECT_PY 3
#define LATTICE_PROG_CONNECT_NY 4
#define LATTICE_PROG_CONNECT_PZ 5
#define LATTICE_PROG_CONNECT_NZ 6

// Connection program flags
#define LATTICE_PROG_CONNECT_CONFIG_READ 8
#define LATTICE_PROG_CONNECT_CONFIG_WRITE 16
#define LATTICE_PROG_CONNECT_CONFIG_DEFAULT 32

// Return values
#define LATTICE_STATE_OKAY 0
#define LATTICE_STATE_ERR_OVERFLOW_CELL -1
#define LATTICE_STATE_ERR_BAD_CONFIG -2

// SIMU Functions: functions dedicated to manipulating the simulated library. These will be undefined if SIMU_FUNC_DEFINED is not 1.
int SIMU_Lattice_Init(int X, int Y, int Z);
int SIMU_Lattice_Examine(int X, int Y, int Z, double* cell);
int SIMU_Thread_Speed(double ts);

// AnalogLatticeLibrary functions: proper accessible functions for general use functions.
int ALL_Lattice_Program_Core(int X, int Y, int Z, char code)