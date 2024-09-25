/*
	Analog Lattice Library 
	by Harris C. McRae

	A library for interacting with a simulated general-purpose Analog Lattice. 

*/

#pragma once

// Information declarations
#define SIMU_FUNC_DEFINED 1							// Denotes whether or not the simulation package is included. 
#define SIMU_LATTICE_GROUP_POWER 1					// I forgot what this denotes.
#define CELL_TYPE float								// The data type used by each cell
//#define CELL_TYPE_USE_FIXED_POINT
#define OPTIM_MEMORY true							// If defined, utilizes optimized memory for faster memory by rounding dimensions to the nearest power of 2.

// Core program flags
#define LATTICE_PROG_CORE_MASK 4					// Mask for the core program flags.
#define LATTICE_PROG_CORE_HOLDVAL 0					// Set the core to hold the value in the underbus.
#define LATTICE_PROG_CORE_SUM 1						// Set the core to sum on its inputs.
#define LATTICE_PROG_CORE_MULT 2					// Set the core to multiply its inputs.
#define LATTICE_PROG_CORE_INT 3						// Set the core to integrate its inputs.

// Connection selectors
#define LATTICE_PROG_CONNECT_MASK 7					// Mask for the lattice connection flags
#define LATTICE_PROG_CONNECT_PX 0					// Positive X connection
#define LATTICE_PROG_CONNECT_PY 1					// Positive Y connection
#define LATTICE_PROG_CONNECT_PZ 2					// Positive Z connection
#define LATTICE_PROG_CONNECT_NX 3					// Negative X connection
#define LATTICE_PROG_CONNECT_NY 4					// Negative Y connection
#define LATTICE_PROG_CONNECT_NZ 5					// Negative Z connection

// Connection program flags
#define LATTICE_PROG_CONNECT_CONFIG_ACTIVE 8		// This connection is active (in general)
#define LATTICE_PROG_CONNECT_CONFIG_FLOW_POS 0		// Reads on the given connection. Exclusive with write.
#define LATTICE_PROG_CONNECT_CONFIG_FLOW_NEG 16		// Writes on the given connection. Exclusive with read

#define LATTICE_PROG_CONNECT_CONFIG_MOD 32			// A modifier is active on this connection
#define LATTICE_PROG_CONNECT_CONFIG_MOD_COEFF 0		// Coefficient modifier. Excludes divisor.
#define LATTICE_PROG_CONNECT_CONFIG_MOD_DIVIS 64	// Divisor modifier. Excludes coefficient.

#define LATTICE_PROG_CONNECT_CONFIG_INVERT 64		// Invert this line. Inclusive of any config
#define LATTICE_PROG_CONNECT_CONFIG_ABSOLUTE 128	// Absolute this line. Inclusive of any config

// Noise mode flags
#define LATTICE_NOISE_MODE_NONE 0				// No noise is applied.
#define LATTICE_NOISE_MODE_RANDOM 1				// Applies a random noise level on all connections. Marginally longer compute time.
#define LATTICE_NOISE_MODE_INDUCTIVE 2			// Applies noise based on induction due to magnetic flux from nearby cells. (typically those in a 2x3 around the connection)
#define LATTICE_NOISE_MODE_RESISTIVE 4			// Applies a resistive noise (some resistance is measured across connections and reduces the charge slightly).
#define LATTICE_NOISE_MODE_HEAT_RESISTIVE 8		// If resistive noise is enabled, heat increases due to resistance which creates more heat.

// Return values
#define LATTICE_STATE_OKAY 0				// No errors.
#define LATTICE_STATE_ERR_OVERFLOW_CELL 1	// A cell overflowed its bounds.
#define LATTICE_STATE_ERR_BAD_CONFIG 2		// A bad configuration was attempted.
#define LATTICE_STATE_ERR_NOT_INIT 4		// The lattice was not initialized.
#define LATTICE_STATE_ERR_UNKNOWN 8			// An unknown error occured.
#define LATTICE_STATE_ERR_DIV_ZERO 16		// Division by zero occurred

#define LATTICE_DEFAULT_DIV_ZERO 0			// Value to default to when a DIV ZERO has occurred.

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
int SIMU_Lattice_Examine(int X, int Y, int Z, CELL_TYPE* cell);
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
int Lattice_Program_SetUnderbus(CELL_TYPE charge);