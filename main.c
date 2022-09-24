
/* Includes */

	#include "config.h"

	#if PCI == PCI_DL
		#include <pci/pci.h>
		#include <pci/types.h>
		#include <dlfcn.h>
	#elif PCI == PCI_FULL
		#include <pci/pci.h>
		#include <pci/types.h>
	#endif

	#include <dirent.h>
	#include <pwd.h>
	#include <time.h>

	#include <sys/sysinfo.h>
	#include <sys/utsname.h>
	#include <sys/ioctl.h>

	#define PUTC(c) putchar(c)
	#define PUTS(s) fputs((s), stdout)
	#define ESCS(s) putchar(ESCAPE), PUTS(s)

	// So I don't have to include all of uinstd.h
	extern uid_t getuid(void);
	extern uid_t geteuid(void);

/* Variable Definitions */

	#if PCI != PCI_NONE
		unsigned char pciused = 0;
		void  *pcihandle = NULL;
		struct pci_access *pciaccess;
		struct pci_dev    *pcidev;
	#endif

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

	enum INFO *info = info_default; 


int main(int argc, char *argv[]) {

	register char c = 0;
	register unsigned int m = 0;

	// Parse command line arguments
	if (argc == 1) goto l_argend; // There are none, skip to end
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
						"	-P:            Disable libpci\n"
						"	-i [ohskputmc] Set info shown\n"
						"		o: OS\n"
						"		h: Host\n"
						"		s: Shell\n"
						"		k: Kernel\n"
						"		p: Packages\n"
						"		u: Uptime\n"
						"		t: Time\n"
						"		m: Memory\n"
						"		c: CPU info\n"
						"		g: GPU info\n"
						"	-s [SEPERATOR] Set the seperator\n"
						"	-t [THEME]     Set the theme"
						"Distros:\n"
						"	Arch Linux\n"
						"	Ubuntu\n"
						"	Fedora\n"
					);
					return 0;
				case 'T':
					do_themeing = 0;
					break;
				case 'P':
					#if PCI != PCI_NONE
						pcihandle = NULL;
						pciused = 1;
					#endif
					break;
				case 'i':
					if ((++i) == argc) goto l_arg_not_enough;
					do_custominfo = 1;
					for (m = 0; argv[i][m] != '\0'; ++m) { /* nothing */ }
					info = malloc((m + 1) * sizeof(enum INFO));
					m = 0;
					while ((c = *argv[i]) != '\0') {
						/*switch (c) {
							case 'o': c = INFO_OS;   break;
							case 'h': c = INFO_HOST; break;
							case 's': c = INFO_SH;   break;
							case 'k': c = INFO_KRNL; break;
							case 'p': c = INFO_PKGS; break;
							case 'u': c = INFO_UP;   break;
							case 't': c = INFO_TIME; break;
							case 'm': c = INFO_MEM;  break;
							case 'c': c = INFO_CPU;  break;
							case 'g': c = INFO_GPU;  break;
							default:
								printf("Unknown info: %c\n", c);
								goto l_arg_info_loop_end;
						}*/
						info[m] = (enum INFO)c;
						++m;
						//l_arg_info_loop_end:
						++argv[i];
					}
					info[m] = INFO_DONE;
					break;
				case 's':
					if ((++i) == argc) goto l_arg_not_enough;
					SEPERATOR = argv[i];
					break;
				case 't':
					if ((++i) == argc) goto l_arg_not_enough;
					THEME = argv[i];
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
	
	if (do_themeing) ESCS(THEME);
	/*
	PUTS(pwd->pw_name);
	PUTC('@');
	PUTS(uts->nodename); 
	PUTC('\n');
	*/

	register unsigned char it = 0;
	register unsigned char asciioffset = 0;
	while (1) {

		if (info[it] == INFO_DONE) goto l_loopend;
		
		if (it - asciioffset < ASCIIHEIGHT) {
			if (do_themeing) ESCS(distro_col); // Set Logo theme
			PUTS(distro_ascii->d[it - asciioffset]);
			if (do_themeing) {
				ESCS(THEMER);
				ESCS(THEME); // Set Text theme
			}
		} else {
			PUTS(ASCIIEMPTY);
		}

		l_switchstart: switch (info[it]) {

			case INFO_OS:
				PUTS(INFO_NAME_OS);
				PUTS(SEPERATOR);
				if (do_themeing) ESCS(THEME);
				PUTS((distro_name == NULL) ? distroinfo[0].name : distro_name);
				PUTC(' ');
				PUTS(uts->machine);
				break;

			case INFO_HOST:
				if ((file =        fopen(L_PRODUCTNAME, "r")) == NULL) goto l_switcherr;
				if ((dir  = (DIR *)fopen(L_PRODUCTVER , "r")) == NULL) goto l_switcherr;
				PUTS(INFO_NAME_HOST);
				PUTS(SEPERATOR);
				if (do_themeing) ESCS(THEME);
				i = 1;
				while ((c = fgetc(file)) != '\n') PUTC(c);
				fclose(file);
				PUTC(' ');
				while ((c = fgetc((FILE *)dir)) != '\n') PUTC(c);
				fclose((FILE *)dir);
				break;

			case INFO_SH:
				if (!(file = (FILE *)getenv("SHELL"))) goto l_switcherr;
				PUTS(INFO_NAME_SH);
				PUTS(SEPERATOR);
				if (do_themeing) ESCS(THEME);
				PUTS((char *)file);
				break;

			case INFO_KRNL:
				PUTS(INFO_NAME_KRNL);
				PUTS(SEPERATOR);
				if (do_themeing) ESCS(THEME);
				PUTS(uts->release);
				break;

			case INFO_PKGS:
				PUTS(INFO_NAME_PKGS);
				PUTS(SEPERATOR);
				if (do_themeing) ESCS(THEME);
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
				} else {
					PUTS("Unknown\n");
				}
				break;

			case INFO_UP:
				PUTS(INFO_NAME_UP);
				PUTS(SEPERATOR);
				if (do_themeing) ESCS(THEME);
				printf("%02ld:%02ld.%02ld", sys->uptime/(60*60), sys->uptime%(60*60)/60, sys->uptime%60);
				break;

			case INFO_TIME:
				PUTS(INFO_NAME_TIME);
				PUTS(SEPERATOR);
				if (do_themeing) ESCS(THEME);
				time_t tnow = time(NULL);
				char *tbuf = malloc(DATE_BUFSIZE * sizeof(char*));
				strftime(tbuf, DATE_BUFSIZE, DATE_FORMAT, localtime(&tnow));
				PUTS(tbuf);
				free(tbuf);
				break;
				
			case INFO_MEM:
				PUTS(INFO_NAME_MEM);
				PUTS(SEPERATOR);
				if (do_themeing) ESCS(THEME);
				sys->totalram /= 1024*1024;
				sys->freeram  /= 1024*1024;
				printf("%luMiB / %luMiB", sys->totalram - sys->freeram, sys->totalram);
				break;

			case INFO_CPU:
				if ((file = fopen(L_CPUINFO, "r")) == NULL) goto l_switcherr;
				PUTS(INFO_NAME_CPU);
				PUTS(SEPERATOR);
				if (do_themeing) ESCS(THEME);
				i = 0;
				CPU_start:
					while ((c = fgetc(file)) != ':') {;} // wait for 5 : (get to machine name)
					if ((++i) < 5) goto CPU_start;
				i = 0;
				fgetc(file);
				while ((c = fgetc(file)) != '\n') PUTC(c);
				fclose(file);
				break;

			case INFO_GPU:
				#if PCI == PCI_NONE // PCI has been disabled so
					goto l_switcherr;
				#else
					if (pcihandle == NULL) { // setup pci
						if (pciused > 0) goto l_switcherr; // used with error (probably wont work if it didn't work before)
						#if PCI == PCI_FULL
							pciaccess = pci_alloc();
							pci_init(pciaccess);
							pci_scan_bus(pciaccess);
						#elif PCI == PCI_DL
							if ((pcihandle = dlopen(L_PCI, RTLD_LAZY)) == NULL) goto GPU_err;
							if ((file = dlsym(pcihandle, "pci_alloc"    )) == NULL) goto GPU_err;
							pciaccess = (*(struct pci_access *(*)())file)();
							if ((file = dlsym(pcihandle, "pci_init"     )) == NULL) goto GPU_err;
							(*(void (*)(struct pci_access*))file)(pciaccess);
							if ((file = dlsym(pcihandle, "pci_scan_bus" )) == NULL) goto GPU_err;
							(*(void (*)(struct pci_access*))file)(pciaccess);
						#endif
					}
					++pciused;

					#if PCI == PCI_DL
						if ((file = dlsym(pcihandle, "pci_fill_info")) == NULL) goto l_switcherr;
					#endif
					unsigned char i = pciused;
					for (pcidev = pciaccess->devices; pcidev; pcidev = pcidev->next) {
						#if PCI == PCI_DL
							(*(void (*)(struct pci_dev*, int))file)(pcidev, PCI_FILL_IDENT | PCI_FILL_CLASS | PCI_FILL_LABEL);
						#elif PCI == PCI_FULL
							pci_fill_info(pcidev, PCI_FILL_IDENT | PCI_FILL_CLASS | PCI_FILL_LABEL);
						#endif
						switch (pcidev->device_class) {
					        case 0x0300:
					        case 0x0301:
					        case 0x0302:
							case 0x0380:
								if ((--i) == 0) goto l_gpu_found;
						}
					}
					goto l_switcherr; // no gpu was found
					l_gpu_found:
					#if PCI == PCI_DL
						if ((file = dlsym(pcihandle, "pci_lookup_name")) == NULL) goto l_switcherr;
					#endif

					PUTS(INFO_NAME_GPU);
					PUTS(SEPERATOR);
					if (do_themeing) ESCS(THEME);
					char *buf = malloc(65 * sizeof(char)); // 65 is the maximun buf length requires (see pci lookup tables)
					#if PCI == PCI_DL
						(*(void (*)(struct pci_access*, char*, int, int, ...))file)(pciaccess, buf, 65 * sizeof(char), PCI_LOOKUP_DEVICE, pcidev->vendor_id, pcidev->device_id);
					#elif PCI == PCI_FULL
						pci_lookup_name(pciaccess, buf, 65 * sizeof(char), PCI_LOOKUP_DEVICE, pcidev->vendor_id, pcidev->device_id);
					#endif
					PUTS(buf);
					PUTC(' ');
					#if PCI == PCI_DL
		            	(*(void (*)(struct pci_access*, char*, int, int, ...))file)(pciaccess, buf, 65 * sizeof(char), PCI_LOOKUP_VENDOR, pcidev->vendor_id, pcidev->device_id);
					#elif PCI == PCI_FULL
						pci_lookup_name(pciaccess, buf, 65 * sizeof(char), PCI_LOOKUP_VENDOR, pcidev->vendor_id, pcidev->device_id);
		            #endif
					PUTS(buf);
					free(buf);

					break;
				#endif

			default: l_switcherr:
				++it;
				if (info[it] == INFO_DONE) {
					if (it - asciioffset <= ASCIIHEIGHT) PUTC('\n');
					goto l_loopend;
				}
				++asciioffset;
				goto l_switchstart;
		}
		++it;
		PUTC('\n');
		
	} l_loopend:

	it -= asciioffset;
	if (it <= ASCIIHEIGHT) { // Render any remaining ascii art
		if (do_themeing) ESCS(distro_col);
		while (it < ASCIIHEIGHT) {
			PUTS(distro_ascii->d[it]);
			if (it <= ASCIIHEIGHT) PUTC('\n');
			++it;
		}
	} else {
		printf("\x1b[0E");
	}
	
	if (do_themeing) ESCS(THEMER);

	if (do_custominfo == 1   ) free(info       );
	if (distro_name   != NULL) free(distro_name);
	if (sys           != NULL) free(sys        );
	if (uts           != NULL) free(uts        );

	#if PCI != PCI_NONE
		if (pcihandle != NULL) {
			#if PCI == PCI_DL
				if ((file = dlsym(pcihandle, "pci_cleanup")) != NULL) { // not having this function is not fatal
					(*(void (*)(struct pci_access*))file)(pciaccess);
				}
			#elif PCI == PCI_FULL
				pci_cleanup(pciaccess);
			#endif
		}
	#endif

	return 0;

	#if PCI != PCI_NONE
		GPU_err:
			pcihandle = NULL;
			pciused = 1;
			goto l_switcherr;
	#endif
}
