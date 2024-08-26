/*
	Analog Lattice Library 
	by Harris C. McRae

	A library for interacting with a simulated general-purpose Analog Lattice. 

*/

#pragma once

// Information declarations
#define SIMU_FUNC_DEFINED 1
#define SIMU_LATTICE_GROUP_POWER 1

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
#define LATTICE_PROG_CONNECT_CONFIG_COEFF 32
#define LATTICE_PROG_CONNECT_CONFIG_INVERT 64

// Noise mode flags
#define LATTICE_NOISE_MODE_NONE 0				// No noise is applied.
#define LATTICE_NOISE_MODE_RANDOM 1				// Applies a random noise level on all connections. Marginally longer compute time.
#define LATTICE_NOISE_MODE_INDUCTIVE 2			// Applies noise based on induction due to magnetic flux from nearby cells. (typically those in a 2x3 around the connection)
#define LATTICE_NOISE_MODE_RESISTIVE 4			// Applies a resistive noise (some resistance is measured across connections and reduces the charge slightly).
#define LATTICE_NOISE_MODE_HEAT_RESISTIVE 8		// If resistive noise is enabled, heat increases due to resistance which creates more heat.

// Return values
#define LATTICE_STATE_OKAY 0				// No errors.
#define LATTICE_STATE_ERR_OVERFLOW_CELL -1	// A cell overflowed its bounds.
#define LATTICE_STATE_ERR_BAD_CONFIG -2		// A bad configuration was attempted.
#define LATTICE_STATE_ERR_NOT_INIT -3		// The lattice was not initialized.
#define LATTICE_STATE_ERR_UNKNOWN -4		// An unknown error occured.

// SIMU Functions: functions dedicated to manipulating the simulated library. These will be undefined if SIMU_FUNC_DEFINED is not 1.

/// <summary>
/// Initializes the simulated lattice with the given dimensions. The X axis is generally used as the input/output axis, with X=0 being write-only, and X=MAX-1 being readable.
/// A given string of noise flags can be provided to configure what noise simulation is done in the system.
/// </summary>
/// <param name="X"></param>
/// <param name="Y"></param>
/// <param name="Z"></param>
/// <param name="noise"></param>
/// <param name="ts"></param>
/// <returns>An integer corresponding to the LATTICE_STATE values.</returns>
int SIMU_Lattice_Init(int X, int Y, int Z, int noise, double ts);
/// <summary>
/// Destroys the simulated lattice
/// </summary>
/// <returns></returns>
int SIMU_Lattice_Destroy();
/// <summary>
/// Examines any cell in the lattice for the purpose of debugging the simulation.
/// </summary>
/// <param name="X"></param>
/// <param name="Y"></param>
/// <param name="Z"></param>
/// <param name="cell">The value of the cell</param>
/// <returns>An integer corresponding to the LATTICE_STATE</returns>
int SIMU_Lattice_Examine(int X, int Y, int Z, double* cell);
/// <summary>
/// Changes the noise mode applied to the simulation on connections. Note that the more noise introduced, the longer compute time will run.
/// </summary>
/// <param name="mode"></param>
/// <returns></returns>
int SIMU_Lattice_NoiseMode(int mode);
/// <summary>
/// Changes the minimum wait time inbetween polls of the simulation.
/// </summary>
/// <param name="ts"></param>
/// <returns></returns>
int SIMU_Thread_Speed(double ts);

// AnalogLibrary lattice functions: proper accessible functions for general use functions.

/// <summary>
/// Writes a instruction set to a specific cell core. Refer to LATTICE_PROG_CORE defines
/// </summary>
/// <param name="X"></param>
/// <param name="Y"></param>
/// <param name="Z"></param>
/// <param name="code"></param>
/// <returns></returns>
int Lattice_Program_Core(int X, int Y, int Z, int code);
/// <summary>
/// Writes a configuration to a specific connection on a cell. Note that read/write values are relevant to reading in, not writing out. 
/// </summary>
/// <param name="X"></param>
/// <param name="Y"></param>
/// <param name="Z"></param>
/// <param name="code"></param>
/// <returns></returns>
int Lattice_Program_Connect(int X, int Y, int Z, int code);
/// <summary>
/// Sets the current underbus value. Whenever a configuration is written that uses a set value, the underbus is used.
/// </summary>
/// <param name="charge"></param>
/// <returns></returns>
int Lattice_Program_SetUnderbus(double charge);