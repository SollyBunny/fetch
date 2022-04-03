
/* Includes */

	#include <dirent.h>
	#include <pwd.h>
	#include <time.h>
	
	#include <sys/sysinfo.h>
	#include <sys/utsname.h>
	#include <sys/ioctl.h>

	#include "types.h"

	#define PUTC(c) putchar(c)
	#define PUTS(s) fputs(s, stdout)

	// So I don't have to include all of uinstd.h
	extern uid_t getuid(void);
	extern uid_t geteuid(void);

// Variable Definitions

	struct passwd  *pwd = NULL;
	struct sysinfo *sys = NULL;
	struct utsname *uts = NULL;

	char        *distro_name = NULL;
	const char  *distro_col  = NULL;
	const ASCII *distro_ascii;

	FILE *file;
	DIR  *dir;

	unsigned int i, m, asciioffset;
	char c;

	#include "config.h"

int main() {

	pwd = getpwuid(getuid());
	sys = malloc(sizeof(struct sysinfo));
	uts = malloc(sizeof(struct utsname));
	if (sysinfo(sys) == -1) puts("sysinfo failed");
	if (uname(uts)   == -1) puts("uname failed"  );
	
	if ((file = fopen(L_RELEASE, "r")) != NULL) {
		while ((c = fgetc(file)) != '"') {;} // wait till first "
		i = 1;
		while ((c = fgetc(file)) != '"') ++i;
		distro_name = malloc(i * sizeof(char));
		fseek(file, -((int)i), SEEK_CUR);
		while ((c = fgetc(file)) != '"') {
			*distro_name = c;
			++distro_name;
		}
		*distro_name = '\0';
		distro_name -= i - 1;
		fclose(file);
		// find matching data
		for (i = 1; i < (sizeof(distroinfo) / sizeof(distroinfo[0])); ++i) {
			// compare distro_name & distroinfo[i].name
			#define s1 distro_name
			#define s2 distroinfo[i].name
				m = 0;
				do {
					if (s1[m] != s2[m]) goto l_distro_name_loop_end;
					++m;	
				} while (s1[m] != '\0' && s2[m] != '\0');
			#undef s1
			#undef s2
			distro_col   = distroinfo[i].theme;
			distro_ascii = &(distroinfo[i].ascii);
			goto l_distro_name_end;
			
		l_distro_name_loop_end: continue; } // continue for old gcc compat
		// no matches
	}
	distro_col   = THEME;
	distro_ascii = &(distroinfo[0].ascii);
	l_distro_name_end:
	
	PUTS(THEME);
	PUTS(pwd->pw_name);
	PUTC('@');
	PUTS(uts->nodename); 
	PUTC('\n');
	
	unsigned it = 0;
	while (1) {

		if (info[it] == INFO_DONE) goto l_loopend;
		
		if (it - asciioffset < ASCIIHEIGHT) {
			PUTS(distro_col); // Set Logo theme
			PUTS(distro_ascii->d[it - asciioffset]);
			PUTS(THEMER);
			PUTS(THEME); // Set Text theme
		} else {
			PUTS(ASCIIEMPTY);
		}

		l_switchstart: switch (info[it]) {

			case INFO_OS:
				PUTS(INFO_NAME_OS);
				PUTS(SEPERATOR);
				PUTS(THEME);
				PUTS((distro_name == NULL) ? distroinfo[0].name : distro_name);
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
	if (it < ASCIIHEIGHT- 1) { // Render any remaining ascii art
		PUTS(distro_col);
		while (it < ASCIIHEIGHT - 1) {
			PUTS(distro_ascii->d[it]);
			++it;
		}
	}
	
	PUTS(THEMER);
	
	if (sys         != NULL) free(sys        );
	if (uts         != NULL) free(uts        );
	if (distro_name != NULL) free(distro_name);

	return 0;
	
}
