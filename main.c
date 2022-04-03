
/* Includes */

	#include <stdio.h>
	#include <stdlib.h>
	#include <dirent.h>
	#include <pwd.h>
	#include <time.h>
	
	#include <sys/sysinfo.h>
	#include <sys/utsname.h>
	#include <sys/ioctl.h>

	#define PUTC(c) putchar(c)
	#define PUTS(s) fputs(s, stdout)

	typedef unsigned char bool;

	// So I don't have to include all of uinstd.h
	extern uid_t getuid(void);
	extern uid_t geteuid(void);

// Variable Definitions

	struct passwd  *pwd = NULL;
	struct sysinfo *sys = NULL;
	struct utsname *uts = NULL;

	char *osname      = NULL;
	char *asciicol    = NULL;
	bool releasevalid = 1;

	FILE *file;
	DIR  *dir;

	unsigned int i, m, asciioffset;
	char c;	
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

	unsigned int intsize(unsigned int a) {
		if (a < 10  ) return 1;
		if (a < 100 ) return 2;
		if (a < 1000) return 3;
		return 4;
	}

	long int longsize(long int a) {
		if (a < 10   ) return 1;
		if (a < 100  ) return 2;
		if (a < 1000 ) return 3;
		return 4;
	}

	#include "config.h"

int main() {

	pwd = getpwuid(getuid());
	sys = malloc(sizeof(struct sysinfo));
	uts = malloc(sizeof(struct utsname));
	if (sysinfo(sys) == -1) puts("sysinfo failed");
	if (uname(uts)   == -1) puts("uname failed"  );
	
	if ((file = fopen(L_RELEASE, "r")) == NULL) {
		osname   = "Linux";
		asciicol = THEME;
		releasevalid = 0;
	} else {
	
		while ((c = fgetc(file)) != '"') {;} // wait till first "
		i = 1;
		while ((c = fgetc(file)) != '"') ++i;
		osname = malloc(i * sizeof(char));
		fseek(file, -((int)i), SEEK_CUR);
		while ((c = fgetc(file)) != '"') {
			*osname = c;
			++osname;
		}
		*osname = '\0';
		osname -= i - 1;

		printf("%c", fgetc(file));

		fclose(file);
	}

	return 0;

	PUTC(' ');
	PUTS(THEME);
	PUTS(pwd->pw_name);
	PUTC('@');
	PUTS(uts->nodename); 
	PUTC('\n');
	
	unsigned it = 0;
	while (1) {

		if (info[it] == INFO_DONE) goto l_loopend;

		PUTS(asciicol); // Set Logo theme
		PUTS(ascii[it - asciioffset]);
		PUTS(THEMER);
		PUTS(THEME); // Set Text theme

		l_switchstart: switch (info[it]) {

			case INFO_OS:
				PUTS(INFO_NAME_OS);
				PUTS(SEPERATOR);
				PUTS(THEME);
				PUTS(osname);
				PUTC(' ');
				PUTS(&uts->machine[m]);
				break;

			case INFO_HOST:
				if ((file =        fopen(L_PRODUCTNAME, "r")) == NULL) goto l_switcherr;
				if ((dir  = (DIR *)fopen(L_PRODUCTVER , "r")) == NULL) goto l_switcherr;
				PUTS(INFO_NAME_HOST);
				PUTS(SEPERATOR);
				PUTS(THEME);
				i = 1;
				while ((c = fgetc(file)) != '\n') PUTC(c);
				fclose(file);
				PUTC(' ');
				while ((c = fgetc((FILE *)dir)) != '\n') PUTC(c);
				fclose((FILE *)dir);
				break;

			case INFO_KRNL:
				PUTS(INFO_NAME_KRNL);
				PUTS(SEPERATOR);
				PUTS(THEME);
				PUTS(uts->release);
				break;

			case INFO_PKGS:
				PUTS(INFO_NAME_PKGS);
				PUTS(SEPERATOR);
				PUTS(THEME);
				m = 0;
				// Arch (pacman / yay)
				if ((dir = opendir("/var/lib/pacman/local")) != NULL) { 
					while (readdir(dir) != NULL) ++m;
					free(dir);
					m -= 3;
					printf("%u", m);
					PUTS(" (" PKG_PACMAN ")");
				// Debian (dpkg / apt) 
				} else if ((file = fopen("/var/lib/dpkg/status", "r")) != NULL) {
					i = 0; // flag to see if newline has appeared twice
					while ((c = fgetc(file)) != EOF) {
						if (c != '\n') {
							i = 0;
						} else if (i == 0) {
							i = 1;
						} else {
							++m;
							i = 0;
						}
					}
					fclose(file);
					printf("%u", m);
					PUTS(" (" PKG_DKPG ")");
				} else goto l_switcherr;
				break;

			case INFO_UP:
				PUTS(INFO_NAME_UP);
				PUTS(SEPERATOR);
				PUTS(THEME);
				printf("%02ld:%02ld.%02ld", sys->uptime/(60*60), sys->uptime%(60*60)/60, sys->uptime%60);
				break;

			case INFO_TIME:
				PUTS(INFO_NAME_TIME);
				PUTS(SEPERATOR);
				PUTS(THEME);
				time_t tnow = time(NULL);;
				char *tbuf = malloc(DATE_BUFSIZE * sizeof(char*));
				strftime(tbuf, DATE_BUFSIZE, DATE_FORMAT, localtime(&tnow));
				PUTS(tbuf);
				free(tbuf);
				break;
				
			case INFO_MEM:
				PUTS(INFO_NAME_MEM);
				PUTS(SEPERATOR);
				PUTS(THEME);
				sys->totalram /= 1024*1024;
				sys->freeram  /= 1024*1024;
				printf("%luMiB / %luMiB", sys->totalram - sys->freeram, sys->totalram);
				break;

			case INFO_CPU:
				if ((file = fopen(L_CPUINFO, "r")) == NULL) goto l_switcherr;
				PUTS(INFO_NAME_CPU);
				PUTS(SEPERATOR);
				PUTS(THEME);
				i = 0;
				CPU_start:
					while ((c = fgetc(file)) != ':') {;} // wait for 5 : (get to machine name)
					if ((++i) < 5) goto CPU_start;
				i = 0;
				fgetc(file);
				while ((c = fgetc(file)) != '\n') PUTC(c);
				fclose(file);
				break;

			default: 
				PUTS(UNKNOWN);
				break;

			l_switcherr:
				if (info[++it] == INFO_DONE) goto l_loopend;
				++asciioffset;
				goto l_switchstart;
		}
		
		PUTC('\n');
		++it;
		
	} l_loopend:

	it -= asciioffset;
	if (it < sizeof(ascii) / sizeof(ascii[0]) - 1) { // Render any remaining ascii art
		PUTS(asciicol);
		while (it < sizeof(ascii) / sizeof(ascii[0]) - 1) {
			PUTS(ascii[it]);
			++it;
		}
	}
	
	PUTS(THEMER);
	
	if (sys != NULL) free(sys);
	if (uts != NULL) free(uts);
	if (releasevalid == 1) {
		free(osname);
	}

	return 0;
	
}
