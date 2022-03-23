
/* Includes */

	#include <stdio.h>
	#include <stdlib.h>
	#include <dirent.h>
	#include <pwd.h>
	#include <time.h>
	
	#include <sys/sysinfo.h>
	#include <sys/utsname.h>
	#include <sys/ioctl.h>

	// So I don't have to include all of uinstd.h
	extern uid_t getuid(void);
	extern uid_t geteuid(void);

// Variable Definitions

	struct passwd  *pwd = NULL;
	struct sysinfo *sys = NULL;
	struct utsname *uts = NULL;

	FILE *file;
	DIR  *dir;

	unsigned int i, m, p, asciioffset;
	char c;
	char *buf;
	
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

	void copystring(char *str) {
		char *iter = str;
		do {
			buf[p] = *iter;
			++(p);
		} while (*(++iter) != '\0');
	}

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
	sysinfo(sys);
	uname(uts);
	// if (sysinfo(sys) == -1) printf("sysinfo failed\n");
	// if (uname(uts)   == -1) printf("uname failed\n"  );

	buf = malloc(2048);
	p = 0;
	
	buf[p] = ' '; ++p;
	copystring(THEME);
	copystring(pwd->pw_name);
	buf[p] = '@'; ++p;
	copystring(THEME);
	copystring(uts->nodename); 
	buf[p] = '\n'; ++p;

	unsigned it = 0;
	while (1) {

		if (info[it] == INFO_DONE) goto l_loopend;

		copystring(THEMEL); // Set Logo theme
		copystring((char *)ascii[it - asciioffset]);
		copystring(THEME); // Set Text theme

		l_switchstart: switch (info[it]) {

			case INFO_OS:
				if ((file = fopen(L_RELEASE, "r")) == NULL) goto l_switcherr;
				copystring(INFO_NAME_HOST);
				copystring(SEPERATOR);
				copystring(THEME);
				i = 1;
				while ((c = fgetc(file)) != '"') {;} // wait till first "
				while ((c = fgetc(file)) != '"') {
					buf[p] = c; ++p;
					// TODO buf size check
				}
				fclose(file);
				buf[p] = ' '; ++p;
				copystring(&uts->machine[m]);
				break;

			case INFO_HOST:
				if ((file =        fopen(L_PRODUCTNAME, "r")) == NULL) goto l_switcherr;
				if ((dir  = (DIR *)fopen(L_PRODUCTVER , "r")) == NULL) goto l_switcherr;
				copystring(INFO_NAME_HOST);
				copystring(SEPERATOR);
				copystring(THEME);
				
				i = 1;
				while ((c = fgetc(file)) != '\n') {
					buf[p] = c; ++p;
					// TODO buf size check
				}
				
				fclose(file);
				buf[p] = ' '; ++p;
				
				while ((c = fgetc((FILE *)dir)) != '\n') {
					buf[p] = c; ++p;
					// TODO buf size check
				}
				fclose((FILE *)dir);
				break;

			case INFO_KRNL:
				copystring(INFO_NAME_KRNL);
				copystring(SEPERATOR);
				copystring(THEME);
				copystring(uts->release);
				break;

			case INFO_PKGS:
				m = 0;
				// Arch (pacman / yay)
				if ((dir = opendir("/var/lib/pacman/local")) != NULL) { 
					while (readdir(dir) != NULL) ++m;
					free(dir);
					m -= 3;
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
				} else {
					goto l_switcherr;
				}
				copystring(INFO_NAME_PKGS);
				copystring(SEPERATOR);
				copystring(THEME);
				sprintf(buf + p, "%u", m);
				p += intsize(m);
				buf[p] = ' '; ++p;
				copystring("(" PKG_PACMAN ")");
				break;

			case INFO_UP:
				copystring(INFO_NAME_UP);
				copystring(SEPERATOR);
				copystring(THEME);
				sprintf(buf + p, "%02ld:%02ld.%02ld", sys->uptime/(60*60), sys->uptime%(60*60)/60, sys->uptime%60);
				p += 8;
				if (sys->uptime > 99 * (60*60)) { // if extra characters are present, account with buffer iter
					p += longsize(sys->uptime/(60*60)) - 2;
				}
				break;

			case INFO_TIME:
				copystring(INFO_NAME_TIME);
				copystring(SEPERATOR);
				copystring(THEME);
				time_t t;
				t = time(NULL);
				copystring(asctime(localtime(&t)));
				--p; // asctime places a '\n' at the end, get rid of it
				break;
				
			case INFO_MEM:
				copystring(INFO_NAME_MEM);
				copystring(SEPERATOR);
				copystring(THEME);
				sys->totalram /= 1024*1024;
				sys->freeram  /= 1024*1024;
				sprintf(buf + p, "%luM / %luM", sys->totalram - sys->freeram, sys->totalram);
				p += 5 + longsize(sys->totalram - sys->freeram) + longsize(sys->totalram); // calculate buffer movement
				break;

			case INFO_CPU:
				if ((file = fopen(L_CPUINFO, "r")) == NULL) goto l_switcherr;
				copystring(INFO_NAME_CPU);
				copystring(SEPERATOR);
				copystring(THEME);
				i = 0;
				CPU_start:
					while ((c = fgetc(file)) != ':') {;} // wait for 5 : (get to machine name)
					if ((++i) < 5) goto CPU_start;
				i = 0;
				fgetc(file);
				while ((c = fgetc(file)) != '\n') {
					buf[p] = c; ++p;
					// TODO buf size check
				}
				fclose(file);
				break;

			default: 
				copystring(UNKNOWN);
				break;

			l_switcherr:
				if (info[++it] == INFO_DONE) goto l_loopend;
				++asciioffset;
				goto l_switchstart;
		}
		
		buf[p] = '\n'; ++p;
		++it;
		
	} l_loopend:

	it -= asciioffset;
	if (it < sizeof(ascii) / sizeof(ascii[0]) - 1) { // Render any remaining ascii art
		copystring(THEMEL);
		while (it < sizeof(ascii) / sizeof(ascii[0]) - 1) {
			copystring(ascii[it]);
			buf[p] = '\n'; ++p;
			++it;
		}
	}
	
	copystring("\x1b[0m");
	fwrite(buf, sizeof(char), p, stdout);
	free(buf);
	
	if (sys != NULL) free(sys);
	if (uts != NULL) free(uts);

	return 0;
	
}
