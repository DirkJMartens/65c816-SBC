// define some unsigned types
// defined by WDC C compiler 
// typedef unsigned char 	uchar;
// typedef unsigned short 	ushort;
// typedef unsigned long 	ulong;
typedef unsigned char 		uint8_t;		// 1 byte : 0 .. 255 
typedef signed char 		int8_t;			// 1 byte : -127..127 

typedef unsigned int 		uint16_t;		// 2 bytes: 0 .. 65535
typedef signed int 			int16_t;		// 2 bytes: -32767 .. 32767 

typedef unsigned long		uint32_t; 		// 4 bytes: 0 .. 4,294,967,295
typedef signed long 		int32_t; 		// 4 bytes: -2,147,483,647 .. 2,147,483,647

typedef float				float32_t; 		// 4 bytes: 3.4e-38 .. 3.4e+38 
typedef double				double64_t; 	// 8 bytes: 2.2e-308 .. 2.2e+308

typedef long double			lngdbl80_t; 	// 10 bytes: 3.4e-4932 .. 1.1e+4932
