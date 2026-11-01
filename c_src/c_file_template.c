/** @file module.c
*
* @brief A description of the module’s purpose.
*
* @par
* COPYRIGHT NOTICE: (c) 2018 Barr Group. All rights reserved.
*/

// code used for debugging only 
// first: 
// 		#define DEBUG 
// then e.g.: 
// #ifdef DEBUG
// 		printf("Debug value: %d", value); 
// #endif

// 1. INCLUDE: include this module's corresponding header file 
#include “module.h”

// standard C library and other included .C files 

// 2. DEFINE: macros and magic numbers (use capitals) 
#define mymax(a,b) ((a)<(b)?(b):(a))
#define CPU_CLK_SPEED 1000000 

// 3. TYPEDEF: data types definitions
typedef struct { 
	float real; 
	float imag; 
} complex; 

// 4. EXTERN variables 

// 5. FUNCTION PROTOTYPES: 
// private data declarations (i.e. "static" variables) 

// private functions prototypes (i.e. "static" functions) 

// public function bodies (i.e. "extern" functions)

// private function bodies (i.e. implementation) 

// temporarily disabling code 
// #if 0 
// 		some code 
// #endif 


/*** end of file - module.c ***/ 