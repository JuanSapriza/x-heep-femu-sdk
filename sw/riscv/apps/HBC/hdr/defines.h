#ifndef DEFINES_H_
#define DEFINES_H_

//#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// #include "pmsis.h"

// Define PI_L2 as empty if your platform does not require specific memory placement
#define PI_L2

#define PRINT_RESULT
// #define PRINT_RELEN

//========= DEFINE PRINT ========//
//#define PRINT_SIG_RAW
//#define PRINT_SIG_MF
//#define PRINT_SIG_RMS
//#define PRINT_RELEN
//#define PRINT_THR
//#define PRINT_SIG_INPUT_PEAKS
//#define PRINT_RPEAKS
//#define PRINT_BEATCLASS
//#define PRINT_SIG_MF_3L
//#define PRINT_SIG_RMS_3L
//#define PRINT_DEL
//#define PRINT_RESULT

//=========== Sampling frequency ========== //
#define ECG_SAMPLING_FREQUENCY 250

//=========== Number of leads/channels ============//
#define NLEADS 2
#define RMS_NLEADS				NLEADS

//=========== Buffer size for R peak detection ========== //
//The minumum distance between two ECG peaks based on physiological limits: 200 miliseconds * sampling frequency
#define BUFFER_LENGTH_SECONDS (float) 1.75 //2.048 //
#define BUFFER_SIZE (int16_t) (BUFFER_LENGTH_SECONDS*ECG_SAMPLING_FREQUENCY)

//=========== Overlap for Rel-En ========== //
//long window delay length (in samples): sampling frequency times long window length in seconds
//Long window length = 0.95 seconds
#define LONG_WINDOW (uint16_t) (0.95*ECG_SAMPLING_FREQUENCY+1)

//======== DEFINE MODULES ==========//
#define MODULE_MF
#define MODULE_RMS
#define MODULE_RELEN
#define MODULE_RPEAK
#define MODULE_BEATCLASS

#define MODULE_3L
#ifdef MODULE_3L

#define MODULE_MF_3L
#define MODULE_RMS_3L
#define MODULE_DEL_3L

#endif

//======== DEFINE WINDOWS (ONLY FOR DEBUG) ==========//
// #define ONLY_FIRST_WINDOW
// #define ONLY_TWO_WINDOW
// #define DEBUG_FIRST_MODULES

#define N 8
#define H_B 30
#define dim  (int)((BUFFER_SIZE * N) + LONG_WINDOW)
#define dim_seconds ((float)dim / ECG_SAMPLING_FREQUENCY)

//========= DEFINE PROFILING ================//
//#define HWPERF_MODULES	//start profiling separate modules
//#define HWPERF_1L       	//start profiling step 1 app
//#define HWPERF_3L        	//start profiling step 2 app if any
//#define HWPERF_FULL	//start profiling full app (N.B. it will profile also some buffering)

// --- CHOOSE TYPE OF PROFILING ---
//#define ACTIVE          //cycles and active cycles
//#define EXTACC		//# of loads and stores in EXT memory (L2)
//#define INTACC		//# of loads and stores in INT memory (L1)
//#define STALL			//# number of core stalls
//#define INSTRUCTION	//# number of instructions
//#define TCDM			//# of conflicts in TCDM (L1 memory) between cores
//#define JUMPS         //# of unconditional jumps
//#define BRANCHES      //# of branches (taken + not taken)
//#define TAKEN_BRANCHES //# of branches taken


#define MUL
#define SCALE 100//6//		64

#endif
