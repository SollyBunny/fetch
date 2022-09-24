
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
		INFO_OS   = 'o',
		INFO_HOST = 'h',
		INFO_SH   = 's',
		INFO_KRNL = 'k',
		INFO_PKGS = 'p',
		INFO_UP   = 'u',
		INFO_TIME = 't',
		INFO_MEM  = 'm',
		INFO_CPU  = 'c',
		INFO_GPU  = 'g',
		INFO_DONE = 0,
	};

	#define PCI_FULL 2
	#define PCI_DL 1
	#define PCI_NONE 0
	
#endif
