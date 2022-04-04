
#include <stdlib.h>
#include <stdio.h>

#ifndef NULL
	#define NULL ((void *)0)
#endif

#ifndef _TYPES_HEADER
	#define _TYPES_HEADER
	
	#define ASCIIHEIGHT 7
	#define ASCIIEMPTY "              "
	typedef struct {
		char d[ASCIIHEIGHT][15];
	} ASCII;
	
	enum INFO {
		INFO_DONE,
		INFO_OS,
		INFO_HOST,
		INFO_KRNL,
		INFO_PKGS,
		INFO_UP,
		INFO_TIME,
		INFO_MEM,
		INFO_CPU,
	};
	
#endif
