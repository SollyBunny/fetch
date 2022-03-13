
/* Includes */

	#include <stdio.h>
	#include <stdlib.h>
	#include <dirent.h>
	#include <pwd.h>

	#include <sys/sysinfo.h>
	#include <sys/utsname.h>

	extern uid_t getuid(void);
	extern uid_t geteuid(void);
	// So I don't have to include all of uinstd.h

/* Config */

	#define L_PRODUCTNAME  "/sys/devices/virtual/dmi/id/product_name"
	#define L_PRODUCTVER   "/sys/devices/virtual/dmi/id/product_version"
	#define L_RELEASE      "/etc/os-release"
	#define L_CPUINFO      "/proc/cpuinfo"

	#define STRSIZE 64

	// Refer to https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797 for ansii escape codes
	#define THEME  "\x1b[38;5;201m\x1b[1m" // Pink & Bold
	#define THEMER "\x1b[0m" // Reset Modes

	#define ASCII1 "      /\\      "
	#define ASCII2 "     /  \\     "
	#define ASCII3 "    /\\   \\    "
	#define ASCII4 "   /      \\   "
	#define ASCII5 "  /   ,,   \\  "
	#define ASCII6 " /   |  |  -\\ "
	#define ASCII7 "/_-''    ''-_\\"

// Variable Definitions

	uid_t uid;
	struct passwd *pwd;
	unsigned int packages = 0;
	char *machine;
	char *distro ;
	char *cpu    ;
	struct sysinfo *info;
	struct utsname *host;

	FILE *file;
	DIR  *dir;

	unsigned int i;
	char c;

int main(void) {

	// malloc
		machine = malloc(sizeof(char) * STRSIZE);
		distro  = malloc(sizeof(char) * STRSIZE);
		cpu     = malloc(sizeof(char) * STRSIZE);
		info    = malloc(sizeof(struct sysinfo));
		host    = malloc(sizeof(struct utsname));

	// Get UID / PWD
		uid = getuid();
		pwd = getpwuid(uid);

	// Get # of pkgs (counting items in dir)
		// Arch (pacman / yay)
		if ((dir = opendir("/var/lib/pacman/local")) != NULL) { 
			while (readdir(dir) != NULL) ++packages;
			free(dir);
			--packages;
		// Debian (dpkg / apt)
		} else if ((file = fopen("/var/lib/dpkg/status", "r")) != NULL) {
			i = 0; // flag to see if newline has appeared twice
			while ((c = fgetc(file)) != EOF) {
				if (c != '\n') {
					i = 0;
				} else if (i == 0) {
					i = 1;
				} else {
					++packages;
					i = 0;
				}
			}
			++packages;
			fclose(file);
		}

	// Get machine info (concatonate 2 files)
		if ((file = fopen(L_PRODUCTNAME, "r")) == NULL) { printf("Invalid PRODUCTNAME dir\n"); goto MACHINE_end; }
		i = 1;
		while ((c = fgetc(file)) != '\n') {
			*(machine++) = c;
			if ((++i) >= STRSIZE) { printf("MACHINE name full\n"); goto CPU_end; }
		}
		fclose(file);
		*(machine++) = ' ';
		if ((file = fopen(L_PRODUCTVER, "r")) == NULL) { printf("Invalid PRODUCTVER dir\n"); goto MACHINE_end; }
		while ((c = fgetc(file)) != '\n') {
			*(machine++) = c;
			if ((++i) >= STRSIZE) { printf("MACHINE name full\n"); goto CPU_end; }
		}
		fclose(file);
		machine -= i;
		MACHINE_end:

	// Get Distro name (parse release info)
		if ((file = fopen(L_RELEASE, "r")) == NULL) { printf("Invalid RELEASE dir\n"); goto DISTRO_end; }
		i = 0;
		while ((c = fgetc(file)) != '"') {;} // wait till first "
		while ((c = fgetc(file)) != '"') {
			*(distro++) = c;
			if ((++i) >= STRSIZE) { printf("DISTRO name full\n"); goto CPU_end; }
		}
		fclose(file);
		distro -= i;
		DISTRO_end:
	
	// Get CPU (parse cpuinfo)
		if ((file = fopen(L_CPUINFO, "r")) == NULL) { printf("Invalid CPUINFO dir\n"); goto CPU_end; }
		i = 0;
		CPU_start:
			while ((c = fgetc(file)) != ':') {;} // wait for 5 : (get to machine name)
			if ((++i) < 5) goto CPU_start;
		i = 0;
		fgetc(file);
		while ((c = fgetc(file)) != '\n') {
			*(cpu++) = c;
			if ((++i) >= STRSIZE) { printf("CPU name full\n"); goto CPU_end; }
		}
		cpu -= i;
		fclose(file);
		CPU_end:

	// Get sys info
		if (sysinfo(info) == -1) printf("sysinfo failed\n");
		if (uname(host)   == -1) printf("uname failed\n"  );

	printf(
THEME "              %s" THEMER " @ " THEME "%s" THEMER " %c\n"
THEME ASCII1 " OS   " THEMER ": %s %s\n"
THEME ASCII2 " Host " THEMER ": %s\n"
THEME ASCII3 " Krnl " THEMER ": %s\n"
THEME ASCII4 " Pkgs " THEMER ": %u\n"
THEME ASCII5 " Time " THEMER ": %02ld:%02ld:%02ld\n"
THEME ASCII6 " Mem  " THEMER ": %luM / %luM\n"
THEME ASCII7 " CPU  " THEMER ": %s\n",
		pwd->pw_name, host->nodename, uid == 0 ? '#' : '$',
		distro, host->machine,
		machine,
		host->release,
		packages,
		info->uptime/(60*60), info->uptime%(60*60)/60, info->uptime%60,
		(info->totalram-(info->freeram * info->mem_unit))/1024/1024, info->totalram/1024/1024,
		cpu
	);

	free(machine);
	free(distro );
	free(cpu    );
	free(info   );
	free(host   );

	return 0;
	
}
