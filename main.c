
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

	#define L_PKGMGR       "/var/lib/pacman/local"
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

int main(void) {

	uid_t uid;
	struct passwd *pwd;
	unsigned int packages = 0;
	char *machine = malloc(sizeof(char) * STRSIZE);
	char *distro  = malloc(sizeof(char) * STRSIZE);
	char *cpu     = malloc(sizeof(char) * STRSIZE);
	struct sysinfo *info = malloc(sizeof(struct sysinfo));
	struct utsname *host = malloc(sizeof(struct utsname));

	// Get UID / PWD
		uid = getuid();
		pwd = getpwuid(uid);

	// Get # of pkgs (counting items in dir)
		DIR *dir;
		dir = opendir(L_PKGMGR);
		while (readdir(dir) != NULL) ++packages;
		free(dir);
		--packages;

	// Get machine info (concatonate 2 files)
		FILE *file;
		unsigned int i;
		char c;
		i = 1;
		file = fopen(L_PRODUCTNAME, "r");
		while ((c = fgetc(file)) != '\n') {
			*(machine++) = c;
			if ((++i) >= STRSIZE) { printf("machine failed\n"); return -1; }
		}
		fclose(file);
		*(machine++) = ' ';
		file = fopen(L_PRODUCTVER, "r");
		while ((c = fgetc(file)) != '\n') {
			*(machine++) = c;
			if ((++i) >= STRSIZE) { printf("machine failed\n"); return -1; }
		}
		fclose(file);
		machine -= i;

	// Get Distro name (parse release info)
		i = 0;
		file = fopen(L_RELEASE, "r");
		while ((c = fgetc(file)) != '"') {;} // wait till first "
		while ((c = fgetc(file)) != '"') {
			*(distro++) = c;
			if ((++i) >= STRSIZE) { printf("distro failed\n"); return -1; }
		}
		fclose(file);
		distro -= i;

	// Get CPU (parse cpuinfo)
		i = 0;
		file = fopen(L_CPUINFO, "r");
		CPULOOP_start:
			while ((c = fgetc(file)) != ':') {;} // wait for 5 : (get to machine name)
			if ((++i) < 5) goto CPULOOP_start;
		i = 0;
		fgetc(file);
		while ((c = fgetc(file)) != '\n') {
			*(cpu++) = c;
			if ((++i) >= STRSIZE) { printf("CPU failed\n"); return -1; }
		}
		cpu -= i;
		fclose(file);

	// Get sys info
		if (sysinfo(info) == -1) { printf("sysinfo failed\n"); return -1; }
		if (uname(host)   == -1) { printf("uname failed\n"  ); return -1; }

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

	free(info);
	free(host);
	free(machine);

	return 0;
	
}
