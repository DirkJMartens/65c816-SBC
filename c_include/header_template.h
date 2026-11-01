/** @file module.h
*
* @brief A description of the module’s purpose.
*
* @par
* COPYRIGHT NOTICE: (c) 2018 Barr Group. All rights reserved.
*/

// protect against multiple inclusion 
#ifndef module_H
#	define module_H

// module-wide "constant" defines 
// #define LOWER 100 

// typedefs : all structs, unions and enums should: 
// 	- contain lowercase only 
// 	- end with "_t" 
//	- for public data types: prefix with module name 
// e.g. 
// 	typedef struct 
//	{
//		uint16_t count; 
//		uint16_t max_count;
//	} module_t; 

// WDC816CC built-in datatypes: 
//	Common: 
//		uchar			1 byte		0..255
//		char			1 byte		-127..127
//		int 			2 bytes 	-32,767 to 32,767
//		unsigned int 	2 bytes 	0 to 65,535
//		long 			4 bytes 	-2,147,483,647 to 2,147,483,647
// 		unsigned long 	4 btyes 	0 to 4,294,967,295
//		float 			4 bytes		3.4e-38 to 3.4e+38
// Uncommon: 
// 		enum 			2 bytes		-32,767 to 32,767
//		unsigned short 	2 bytes 	0 to 65,535
//		short 			2 bytes 	-32,767 to 32,767
//		double 			4 bytes		2.2e-308 to 1.79e+308
//		long double 	10 bytes 	3.4e-4932 to 1.1e+4932


// function prototypes 
// 

#endif 		/* module_H */
/*** end of file ***/