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
#define LATTICE_PROG_CORE_MASK 3					// Mask for the core program flags.
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
#define LATTICE_PROG_CONNECT_CONFIG_FLOW_POS 0		// Flows to the positive-axis cell. 
#define LATTICE_PROG_CONNECT_CONFIG_FLOW_NEG 8		// Flows to the negative-axis cell.

#define LATTICE_PROG_CONNECT_CONFIG_MOD_MASK 48		// Mask modifier values
#define LATTICE_PROG_CONNECT_CONFIG_MOD_COEFF 16	// Coefficient modifier. Multiplies input on line by this value.
#define LATTICE_PROG_CONNECT_CONFIG_MOD_DIVIS 32	// Divisor modifier. 'Scales' input on line by this value. Creates a OVERFLOW_CELL error if the result is greater than 1, so use with caution.
#define LATTICE_PROG_CONNECT_CONFIG_MOD_COMP 48		// Comparator modifier. Compares the input to the modifier value - -1 if the value is LESSER, 0 if EQUAL, and 1 if GREATER.

#define LATTICE_PROG_CONNECT_CONFIG_INVERT 64		// Invert this line. Inclusive of any config
#define LATTICE_PROG_CONNECT_CONFIG_ABSOLUTE 128	// Absolute this line. Inclusive of any config

#define LATTICE_PROG_CONNECT_CONFIG_DEACTIVATE 256  // Disables this line.

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
#define LATTICE_STATE_ERR_BAD_CELL_POS 32	// Attempted to load a cell out of bounds.
#define LATTICE_STATE_ERR_UNDEFINED 64		// This function has not been defined yet.
#define LATTICE_STATE_ERR_NO_CONNECTION 128 // No connection here.

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
/// <summary>
/// Returns the estimated polling rate (in Hz) of the simulation.
/// </summary>
/// <returns></returns>
int SIMU_Poll_Rate();

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
/// <summary>
/// Sets the current underbus value to a corresponding value to the given range. Whenever a configuration is written that uses a set value, the underbus is used.
/// </summary>
/// <param name="value"></param>
/// <param name="range"></param>
/// <returns></returns>
int Lattice_Program_SetUnderbus(CELL_TYPE value, CELL_TYPE range);
/// <summary>
/// Sets the current underbus value to a corresponding value to the given range. Whenever a configuration is written that uses a set value, the underbus is used.
/// </summary>
/// <param name="value"></param>
/// <param name="range"></param>
/// <returns></returns>
int Lattice_Program_SetUnderbus(int value, int range);
/// <summary>
/// Inputs a value to {X=0, Y, Z} (input layer).
/// </summary>
/// <param name="Y"></param>
/// <param name="Z"></param>
/// <param name="charge"></param>
/// <returns></returns>
int Lattice_Write(int Y, int Z, CELL_TYPE charge);
/// <summary>
/// Writes a given value scaled to the given range to the coordinates in the input layer.
/// </summary>
/// <param name="Y"></param>
/// <param name="Z"></param>
/// <param name="value"></param>
/// <param name="range"></param>
/// <returns></returns>
int Lattice_Write(int Y, int Z, CELL_TYPE value, CELL_TYPE range);
/// <summary>
/// Writes a given value scaled to the given range to the coordinates in the input layer.
/// </summary>
/// <param name="Y"></param>
/// <param name="Z"></param>
/// <param name="value"></param>
/// <param name="range"></param>
/// <returns></returns>
int Lattice_Write(int Y, int Z, int value, int range);
/// <summary>
/// Reads the value from cell {X=MAX-1, Y, Z} (output layer).
/// </summary>
/// <param name="Y"></param>/// 
/// <param name="Z"></param>/// 
/// <param name="output"></param>/// 
/// <returns></returns>
int Lattice_Read(int Y, int Z, CELL_TYPE* output);
/// <summary>
/// Reads a value from the given coordinates on the output layer and scales it to the given range.
/// </summary>
/// <param name="Y"></param>
/// <param name="Z"></param>
/// <param name="range"></param>
/// <param name="output"></param>
/// <returns></returns>
int Lattice_Read(int Y, int Z, CELL_TYPE range, CELL_TYPE* output);
/// <summary>
/// Reads a value from the given coordinates on the output layer and scales it to the given range.
/// </summary>
/// <param name="Y"></param>
/// <param name="Z"></param>
/// <param name="range"></param>
/// <param name="output"></param>
/// <returns></returns>
int Lattice_Read(int Y, int Z, int range, int* output);
/// <summary>
/// Unlocks all integrators, allowing them to operate.
/// </summary>/// <returns></returns>
int Lattice_Start_Integration();
/// <summary>
/// Locks all integrators, forcing them to hold their current value.
/// </summary>/// <returns></returns>
int Lattice_Stop_Integration();