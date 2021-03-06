
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

/* Variable Definitions */

	struct passwd  *pwd = NULL;
	struct sysinfo *sys = NULL;
	struct utsname *uts = NULL;

	unsigned char do_themeing   = 1;
	unsigned char do_custominfo = 0;

	char        *distro_name  = NULL;
	const char  *distro_col   = NULL;
	const ASCII *distro_ascii = NULL;

	FILE *file;
	DIR  *dir;

	#include "config.h"

	enum INFO *info = info_default; 

int main(int argc, char *argv[]) {

	register char c = 0;
	register unsigned int m = 0;

	if (argc == 1) goto l_argend;
	for (int i = 1; i < argc; ++i) {
		if (argv[i][0] == '-') { // flag switch
			switch (argv[i][1]) {
				case 'h':
				case 'H':
					PUTS(
						"Usage: fetch [DISTRO] [FLAGS]\n"
						"Flags:\n"
						"	-h:            Show this menu\n"
						"	-T:            Remove all theming\n"
						"	-i [ohkputmc]  Set info shown\n"
						"		o: OS\n"
						"		h: Host\n"
						"		k: Kernel\n"
						"		p: Packages\n"
						"		u: Uptime\n"
						"		t: Time\n"
						"		m: Memory\n"
						"		c: CPU info\n"
						"	-s [SEPERATOR] Set the seperator\n"
						"Distros:\n"
						"	Arch Linux\n"
						"	Ubuntu\n"
						"	Fedora\n"
					);
					return 0;
				case 'T':
					do_themeing = 0;
					break;
				case 'i':
					if ((++i) == argc) goto l_arg_not_enough;
					do_custominfo = 1;
					for(m = 0; argv[i][m] != '\0'; ++m) { /* nothing */ }
					info = malloc((m + 1) * sizeof(enum INFO));
					m = 0;
					while ((c = *argv[i]) != '\0') {
						switch (c) {
							case 'o': c = INFO_OS;   break;
							case 'h': c = INFO_HOST; break;
							case 'k': c = INFO_KRNL; break;
							case 'p': c = INFO_PKGS; break;
							case 'u': c = INFO_UP;   break;
							case 't': c = INFO_TIME; break;
							case 'm': c = INFO_MEM;  break;
							case 'c': c = INFO_CPU;  break;
							default:
								printf("Unknown info: %c\n", c);
								goto l_arg_info_loop_end;
						}
						info[m] = (enum INFO)c;
						++m;
						l_arg_info_loop_end:
							++argv[i];
					}
					info[m] = INFO_DONE;
					break;
				case 's':
					if ((++i) == argc) goto l_arg_not_enough;
					SEPERATOR = argv[i];
					break;
				default:
					printf("Unknown flag: %s\n", argv[i]);
					break;
				l_arg_not_enough:
					PUTS("No arg provided for -i\n");
					break;
			}
		} else {
			if (distro_name == NULL) {
				distro_name = argv[i];
			} else {
				PUTS("Multiple distro names given!\n");
			}
		}
	}
	l_argend:

	pwd = getpwuid(getuid());
	sys = malloc(sizeof(struct sysinfo));
	uts = malloc(sizeof(struct utsname));
	if (sysinfo(sys) == -1) puts("sysinfo failed");
	if (uname(uts)   == -1) puts("uname failed"  );

	register unsigned int i = 0;

	if (distro_name != NULL) {
		goto l_distro_name_match;
	} else if ((file = fopen(L_RELEASE, "r")) != NULL) {
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
		distro_name -= i;
		++distro_name;
		fclose(file);
		l_distro_name_match: // find matching data
		for (i = 1; i < (sizeof(distroinfo) / sizeof(distroinfo[0])); ++i) {
			// compare distro_name & distroinfo[i].name
			#define s1 distro_name
			#define s2 distroinfo[i].name
				m = 0;
				do {
					if (
						s1[m] != s2[m]
						&& s1[m] + ('A' - 'a') != s2[m] // account for case differences
						&& s1[m] != s2[m] + ('A' - 'a')
					)  goto l_distro_name_loop_end;
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
	
	if (do_themeing) PUTS(THEME);
	PUTS(pwd->pw_name);
	PUTC('@');
	PUTS(uts->nodename); 
	PUTC('\n');

	register unsigned char it = 0;
	register unsigned char asciioffset = 0;
	while (1) {

		if (*info == INFO_DONE) goto l_loopend;
		
		if (it - asciioffset < ASCIIHEIGHT) {
			if (do_themeing) PUTS(distro_col); // Set Logo theme
			PUTS(distro_ascii->d[it - asciioffset]);
			if (do_themeing) {
				PUTS(THEMER);
				PUTS(THEME); // Set Text theme
			}
		} else {
			PUTS(ASCIIEMPTY);
		}

		l_switchstart: switch (*info) {

			case INFO_OS:
				PUTS(INFO_NAME_OS);
				PUTS(SEPERATOR);
				if (do_themeing) PUTS(THEME);
				PUTS((distro_name == NULL) ? distroinfo[0].name : distro_name);
				PUTC(' ');
				PUTS(uts->machine);
				break;

			case INFO_HOST:
				if ((file =        fopen(L_PRODUCTNAME, "r")) == NULL) goto l_switcherr;
				if ((dir  = (DIR *)fopen(L_PRODUCTVER , "r")) == NULL) goto l_switcherr;
				PUTS(INFO_NAME_HOST);
				PUTS(SEPERATOR);
				if (do_themeing) PUTS(THEME);
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
				if (do_themeing) PUTS(THEME);
				PUTS(uts->release);
				break;

			case INFO_PKGS:
				PUTS(INFO_NAME_PKGS);
				PUTS(SEPERATOR);
				if (do_themeing) PUTS(THEME);
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
				if (do_themeing) PUTS(THEME);
				printf("%02ld:%02ld.%02ld", sys->uptime/(60*60), sys->uptime%(60*60)/60, sys->uptime%60);
				break;

			case INFO_TIME:
				PUTS(INFO_NAME_TIME);
				PUTS(SEPERATOR);
				if (do_themeing) PUTS(THEME);
				time_t tnow = time(NULL);;
				char *tbuf = malloc(DATE_BUFSIZE * sizeof(char*));
				strftime(tbuf, DATE_BUFSIZE, DATE_FORMAT, localtime(&tnow));
				PUTS(tbuf);
				free(tbuf);
				break;
				
			case INFO_MEM:
				PUTS(INFO_NAME_MEM);
				PUTS(SEPERATOR);
				if (do_themeing) PUTS(THEME);
				sys->totalram /= 1024*1024;
				sys->freeram  /= 1024*1024;
				printf("%luMiB / %luMiB", sys->totalram - sys->freeram, sys->totalram);
				break;

			case INFO_CPU:
				if ((file = fopen(L_CPUINFO, "r")) == NULL) goto l_switcherr;
				PUTS(INFO_NAME_CPU);
				PUTS(SEPERATOR);
				if (do_themeing) PUTS(THEME);
				i = 0;
				CPU_start:
					while ((c = fgetc(file)) != ':') {;} // wait for 5 : (get to machine name)
					if ((++i) < 5) goto CPU_start;
				i = 0;
				fgetc(file);
				while ((c = fgetc(file)) != '\n') PUTC(c);
				fclose(file);
				break;

			default: l_switcherr:
				++info;
				if (*info == INFO_DONE) goto l_loopend;
				++asciioffset;
				++it;
				goto l_switchstart;
		}
		
		PUTC('\n');
		++info;
		++it;
		
	} l_loopend:

	if (do_custominfo == 1) {
		info -= it;
		free(info);
	};

	it -= asciioffset;
	if (it < ASCIIHEIGHT) { // Render any remaining ascii art
		if (do_themeing) PUTS(distro_col);
		while (it < ASCIIHEIGHT) {
			PUTS(distro_ascii->d[it]);
			PUTC('\n');
			++it;
		}
	}
	if (do_themeing) PUTS(THEMER);
	
	if (sys           != NULL) free(sys        );
	if (uts           != NULL) free(uts        );
	if (distro_name   != NULL) free(distro_name);
	
	return 0;
	
}
