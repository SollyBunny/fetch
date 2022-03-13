
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

/* Config */

	#define PKG_PACMAN "pacman"
	#define PKG_DKPG   "dkpg"

	enum INFO info[] = {
		INFO_OS,
		INFO_HOST,
		INFO_KRNL,
		INFO_PKGS,
		INFO_UP,
		INFO_TIME,
		INFO_MEM,
		INFO_CPU
	};

	#define L_PRODUCTNAME  "/sys/devices/virtual/dmi/id/product_name"
	#define L_PRODUCTVER   "/sys/devices/virtual/dmi/id/product_version"
	#define L_RELEASE      "/etc/os-release"
	#define L_CPUINFO      "/proc/cpuinfo"

	#define STRSIZE 64

	char *UNKNOWN = "UNKNOWN";

	// Refer to https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797 for ansii escape codes
	const char THEME [] = "\x1b[38;5;201m\x1b[1m"; // Pink & Bold
	const char THEMER[] = "\x1b[0m"; // Reset Modes

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
	char *out[(sizeof(info) / sizeof(enum INFO)) * sizeof(void*)];

	struct sysinfo *sys = NULL;
	struct utsname *uts = NULL;

	FILE *file;
	DIR  *dir;

	unsigned int i, m;
	char c;

int main(void) {

	pwd = getpwuid(getuid());
	sys = malloc(sizeof(struct sysinfo));
	uts = malloc(sizeof(struct utsname));
	if (sysinfo(sys) == -1) printf("sysinfo failed\n");
	if (uname(uts)   == -1) printf("uname failed\n"  );

	for (unsigned int it = 0; it < sizeof(info) / sizeof(enum INFO); ++it) { switch (info[it]) {

		case INFO_OS:
			if ((file = fopen(L_RELEASE, "r")) == NULL) { printf("Invalid RELEASE dir\n"); break; }
			out[it] = malloc(sizeof(char) * STRSIZE);
			i = 1;
			while ((c = fgetc(file)) != '"') {;} // wait till first "
			while ((c = fgetc(file)) != '"') {
				*(out[it]++) = c;
				if ((++i) >= STRSIZE) { printf("DISTRO full\n"); break; }
			}
			*(out[it]++) = ' ';
			fclose(file);
			for (m = 0; uts->machine[m] != '\0'; ++m) {
				*(out[it]++) = uts->machine[m];
				++i;
			}
			out[it] -= i;
			break;

		case INFO_HOST:
			if ((file = fopen(L_PRODUCTNAME, "r")) == NULL) { printf("Invalid PRODUCTNAME dir\n"); break; }
			out[it] = malloc(sizeof(char) * STRSIZE);
			i = 1;
			while ((c = fgetc(file)) != '\n') {
				*(out[it]++) = c;
				if ((++i) >= STRSIZE) { printf("INFO_HOST full\n"); break; }
			}
			fclose(file);
			*(out[it]++) = ' ';
			if ((file = fopen(L_PRODUCTVER, "r")) == NULL) { printf("Invalid PRODUCTVER dir\n"); break; }
			while ((c = fgetc(file)) != '\n') {
				*(out[it]++) = c;
				if ((++i) >= STRSIZE) { printf("INFO_HOST name full\n"); break; }
			}
			fclose(file);
			out[it] -= i;
			out[it][i] = '\0';
			break;

		case INFO_KRNL:
			out[it] = uts->release;
			break;

		case INFO_PKGS:
			out[it] = malloc(sizeof(char) * STRSIZE);
			m = 0;
			// Arch (pacman / yay)
			if ((dir = opendir("/var/lib/pacman/local")) != NULL) { 
				while (readdir(dir) != NULL) ++m;
				free(dir);
				m -= 3;
				sprintf(out[it], "%u (" PKG_PACMAN ")", m);
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
				sprintf(out[it], "%u (" PKG_DKPG ")", m);
			}
			break;

		case INFO_TIME:
			out[it] = malloc(sizeof(char) * STRSIZE);
			time_t t;
			t = time(NULL);
			out[it] = asctime(localtime(&t));
			out[it][24]= '\0';
			break;
			
		case INFO_UP:
			out[it] = malloc(sizeof(char) * STRSIZE);
			sprintf(out[it], "%02ld:%02ld:%02ld", sys->uptime/(60*60), sys->uptime%(60*60)/60, sys->uptime%60);
			break;

		case INFO_MEM:
			out[it] = malloc(sizeof(char) * STRSIZE);
			sprintf(out[it], "%luM / %luM", (sys->totalram-(sys->freeram * sys->mem_unit))/1024/1024, sys->totalram/1024/1024);
			break;

		case INFO_CPU:
			if ((file = fopen(L_CPUINFO, "r")) == NULL) { printf("Invalid CPUINFO dir\n"); break; }
			out[it] = malloc(sizeof(char) * STRSIZE);
			i = 0;
			CPU_start:
				while ((c = fgetc(file)) != ':') {;} // wait for 5 : (get to machine name)
				if ((++i) < 5) goto CPU_start;
			i = 0;
			fgetc(file);
			while ((c = fgetc(file)) != '\n') {
				*(out[it]++) = c;
				if ((++i) >= STRSIZE) { printf("CPU full\n"); break; }
			}
			out[it] -= i;
			out[it][i] = '\0';
			fclose(file);
			break;

		default:
			out[it] = UNKNOWN;
		
	}}

	m = sizeof(info) / sizeof(enum INFO);
	for (unsigned int it = 0; it < sizeof(info) / sizeof(enum INFO); ++it) { 
		fwrite(THEME, sizeof(char), sizeof(THEME), stdout);
		printf("%d\n", m);
		if (it < sizeof(ascii) / sizeof(ascii[0])) {
			fwrite(ascii[it], sizeof(char), sizeof(ascii[0]), stdout);
		} else {
			fwrite(ascii[(sizeof(ascii) - sizeof(ascii[0])) / sizeof(ascii[0])], sizeof(char), sizeof(ascii[0]), stdout);
		}
		switch (info[it]) {
			case INFO_OS  : fputs("      OS", stdout); break;
			case INFO_HOST: fputs("    Host", stdout); break;
			case INFO_KRNL: fputs("  Kernel", stdout); break;
			case INFO_PKGS: fputs("Packages", stdout); break;
			case INFO_UP  : fputs("  Uptime", stdout); break;
			case INFO_TIME: fputs("    Time", stdout); break;
			case INFO_MEM : fputs("     MEM", stdout); break;
			case INFO_CPU : fputs("     CPU", stdout); break;
			default:        puts(UNKNOWN);
		}
		fwrite(THEMER, sizeof(char), sizeof(THEMER), stdout);
		fwrite(" : " , sizeof(char), 3,              stdout);
		fwrite(THEME , sizeof(char), sizeof(THEME ), stdout);
		puts(out[it]);
		
	}

	fwrite(THEMER, sizeof(char), sizeof(THEMER), stdout);

	if (sys != NULL) free(sys);
	if (uts != NULL) free(uts);

	return 0;
	
}
