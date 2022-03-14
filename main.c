
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

	enum INFO {
		INFO_OS,
		INFO_HOST,
		INFO_KRNL,
		INFO_PKGS,
		INFO_UP,
		INFO_TIME,
		INFO_MEM,
		INFO_CPU,
	};

	unsigned int copystring(char *buf, unsigned int *p, char *str) {
		unsigned int iter = 0;
		do {
			buf[*p] = str[iter];
			++(*p);
		} while (str[(++iter)] != '\0');
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
		if (a < 10000) return 4;
		return 5;
	}

/* Config */

	#define PKG_PACMAN "pacman"
	#define PKG_DKPG   "dkpg"

	unsigned int infosize;
	enum INFO *info;

	#define L_PRODUCTNAME  "/sys/devices/virtual/dmi/id/product_name"
	#define L_PRODUCTVER   "/sys/devices/virtual/dmi/id/product_version"
	#define L_RELEASE      "/etc/os-release"
	#define L_CPUINFO      "/proc/cpuinfo"

	#define STRSIZE 64

	char *UNKNOWN   = "UNKNOWN";
	char *ERROR     = "(error)";
	char *SEPERATOR = " : ";

	// Refer to https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797 for ansii escape codes
	char *THEME  = "\x1b[38;5;201m\x1b[1m"; // Pink & Bold
	char *THEMEL = "\x1b[38;5;51m\x1b[1m";  // Cyan & Bold
	char *THEMER = "\x1b[0m";               // Reset Modes

	const char ascii[][15] = {
		"      /\\      ",
		"     /  \\     ",
		"    /\\   \\    ",
		"   /      \\   ",
		"  /   ,,   \\  ",
		" /   |  |  -\\ ",
		"/_-''    ''-_\\",
		"              "
	};


// Variable Definitions

	struct passwd *pwd;

	struct sysinfo *sys = NULL;
	struct utsname *uts = NULL;

	FILE *file;
	DIR  *dir;

	unsigned int i, m, p, asciioffset;
	char c;
	char *buf;

	void _s_PKGMGR() {
		copystring(buf, &p, "Packages");
		copystring(buf, &p, THEMER);
		copystring(buf, &p, SEPERATOR);
		copystring(buf, &p, THEME);
	}


int main(void) {

	infosize = 8;
	info = malloc(infosize * sizeof(enum INFO));
	info[0] = INFO_OS;
	info[1] = INFO_HOST;
	info[2] = INFO_KRNL;
	info[3] = INFO_PKGS;
	info[4] = INFO_UP;
	info[5] = INFO_TIME;
	info[6] = INFO_MEM;
	info[7] = INFO_CPU;

	pwd = getpwuid(getuid());
	sys = malloc(sizeof(struct sysinfo));
	uts = malloc(sizeof(struct utsname));
	if (sysinfo(sys) == -1) printf("sysinfo failed\n");
	if (uname(uts)   == -1) printf("uname failed\n"  );

	buf = malloc(2048);
	p = 0;
	
	buf[p] = ' '; ++p;
	copystring(buf, &p, THEME);
	copystring(buf, &p, pwd->pw_name);
	copystring(buf, &p, THEMER);
	buf[p] = '@'; ++p;
	copystring(buf, &p, THEME);
	copystring(buf, &p, uts->nodename); 
	buf[p] = '\n'; ++p;
	
	if (infosize > sizeof(ascii) / sizeof(ascii[0])) {
		asciioffset = (infosize - (sizeof(ascii) / sizeof(ascii[0])) + 1) / 2;
	} else {
		asciioffset = 0;
	}

	unsigned it;
	for (it = 0; it < infosize; ++it) { 

		copystring(buf, &p, THEMEL); // Set Logo theme
		if (it < asciioffset) { // If ascii is higher
			goto PRINT_else;
		} else if (it - asciioffset < sizeof(ascii) / sizeof(ascii[0])) {
			copystring(buf, &p, (char *)ascii[it - asciioffset]);
		} else { PRINT_else: // If ascii is lower
			copystring(buf, &p, (char *)ascii[(sizeof(ascii) - sizeof(ascii[0])) / sizeof(ascii[0])]);
		}
		copystring(buf, &p, THEME); // Set Text theme

		SWITCH_start: switch (info[it]) {

			case INFO_OS:
				if ((file = fopen(L_RELEASE, "r")) == NULL) goto SWITCH_err;
				copystring(buf, &p, "      OS");
				copystring(buf, &p, THEMER);
				copystring(buf, &p, SEPERATOR);
				copystring(buf, &p, THEME);
				i = 1;
				while ((c = fgetc(file)) != '"') {;} // wait till first "
				while ((c = fgetc(file)) != '"') {
					buf[p] = c; ++p;
					// TODO buf size check
				}
				fclose(file);
				buf[p] = ' '; ++p;
				copystring(buf, &p, &uts->machine[m]);
				break;

			case INFO_HOST:
				if ((file =        fopen(L_PRODUCTNAME, "r")) == NULL) goto SWITCH_err;
				if ((dir  = (DIR *)fopen(L_PRODUCTVER , "r")) == NULL) goto SWITCH_err;
				copystring(buf, &p, "    Host");
				copystring(buf, &p, THEMER);
				copystring(buf, &p, SEPERATOR);
				copystring(buf, &p, THEME);
				
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
				copystring(buf, &p, "  Kernel");
				copystring(buf, &p, THEMER);
				copystring(buf, &p, SEPERATOR);
				copystring(buf, &p, THEME);
				copystring(buf, &p, uts->release);
				break;

			case INFO_PKGS:
				m = 0;
				// Arch (pacman / yay)
				if ((dir = opendir("/var/lib/pacman/local")) != NULL) { 
					_s_PKGMGR();
					while (readdir(dir) != NULL) ++m;
					free(dir);
					m -= 3;
					sprintf(buf + p, "%u", m);
					p += intsize(m);
					buf[p] = ' '; ++p;
					copystring(buf, &p, "(" PKG_PACMAN ")");
				// Debian (dpkg / apt)
				} else if ((file = fopen("/var/lib/dpkg/status", "r")) != NULL) {
					_s_PKGMGR();
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
					sprintf(buf + p, "%u", m);
					p += intsize(m);
					buf[p] = ' '; ++p;
					copystring(buf, &p, "(" PKG_DKPG ")");
				} else {
					goto SWITCH_err;
				}
				break;

			case INFO_TIME:
				copystring(buf, &p, "    Time");
				copystring(buf, &p, THEMER);
				copystring(buf, &p, SEPERATOR);
				copystring(buf, &p, THEME);
				time_t t;
				t = time(NULL);
				copystring(buf, &p, asctime(localtime(&t)));
				--p; // asctime places a '\n' at the end, get rid of it
				break;
				
			case INFO_UP:
				copystring(buf, &p, "  Uptime");
				copystring(buf, &p, THEMER);
				copystring(buf, &p, SEPERATOR);
				copystring(buf, &p, THEME);
				sprintf(buf + p, "%02ld:%02ld.%02ld", sys->uptime/(60*60), sys->uptime%(60*60)/60, sys->uptime%60);
				p += 8;
				if (sys->uptime > 99 * (60*60)) { // if extra characters are present, account with buffer iter
					p += longsize(sys->uptime/(60*60)) - 2;
				}
				break;

			case INFO_MEM:
				copystring(buf, &p, "     MEM");
				copystring(buf, &p, THEMER);
				copystring(buf, &p, SEPERATOR);
				copystring(buf, &p, THEME);
				sys->totalram /= 1024*1024;
				sys->freeram  /= 1024*1024;
				sprintf(buf + p, "%luM / %luM", sys->totalram - sys->freeram, sys->totalram);
				p += 5 + longsize(sys->totalram - sys->freeram) + longsize(sys->totalram); // calculate buffer movement
				break;

			case INFO_CPU:
				if ((file = fopen(L_CPUINFO, "r")) == NULL) goto SWITCH_err;
				copystring(buf, &p, "     CPU");
				copystring(buf, &p, THEMER);
				copystring(buf, &p, SEPERATOR);
				copystring(buf, &p, THEME);
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
				copystring(buf, &p, UNKNOWN);
				break;

			SWITCH_err:
				if ((++it) > infosize) goto SWITCH_end; // no more info to render
				++asciioffset; // make ascii render properly
				goto SWITCH_start;
			
		}
		
		copystring(buf, &p, THEME);
		buf[p] = '\n'; ++p;
		
	} SWITCH_end:

	if (it - asciioffset < sizeof(ascii) / sizeof(ascii[0]) - 1) { // Render any remaining ascii art
		copystring(buf, &p, THEMEL);
		while (it - asciioffset < sizeof(ascii) / sizeof(ascii[0]) - 1) {
			copystring(buf, &p, (char *)ascii[it - asciioffset]);
			buf[p] = '\n'; ++p;
			++it;
		}
	}
	copystring(buf, &p, THEMER);
	
	fwrite(buf, sizeof(char), p, stdout);

	if (sys != NULL) free(sys);
	if (uts != NULL) free(uts);

	return 0;
	
}
