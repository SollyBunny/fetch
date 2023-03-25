
#include "types.h"

// Default order of info
enum INFO info_default[] = {
	INFO_OS,
	INFO_HOST,
	INFO_KRNL,
	INFO_SH,
	INFO_PKGS,
	INFO_UP,
	INFO_MEM,
	//INFO_CPU,
	//INFO_GPU,
	INFO_DONE
};

// Default theming, see https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797
char       *THEME     = "[39m";  // Default
const char *THEMER    = "[0m";   // Reset
char       *SEPERATOR = "\x1b[0m : ";
const char  ESCAPE    = '\x1B';

// Format of the date, see https://linux.die.net/man/3/strftime
#define DATE_FORMAT "%a %Y %m %d %H:%M.%S"

// PCI
#define L_PCI "libpci.so.3"
// #define PCI PCI_FULL // use libpci (requires libpci)  
// #define PCI PCI_DL // use libdl to load in libpci if available (requires libdl)  
#define PCI PCI_NONE // disable libpci (default)  

// Localization
#define PKG_PACMAN "pacman"
#define PKG_DKPG   "dkpg"

#define INFO_NAME_OS   "      OS"
#define INFO_NAME_HOST "    Host"
#define INFO_NAME_KRNL "  Kernel"
#define INFO_NAME_SH   "   Shell"
#define INFO_NAME_PKGS "Packages"
#define INFO_NAME_UP   "  Uptime"
#define INFO_NAME_TIME "    Time"
#define INFO_NAME_MEM  "     MEM"
#define INFO_NAME_CPU  "     CPU"
#define INFO_NAME_GPU  "     GPU"

// Info Location
#define L_PRODUCTNAME  "/sys/devices/virtual/dmi/id/product_name"
#define L_PRODUCTVER   "/sys/devices/virtual/dmi/id/product_version"
#define L_RELEASE      "/etc/os-release"
#define L_CPUINFO      "/proc/cpuinfo"

// Distro Info
struct {
	const char *name;
	const char *theme;
	const ASCII ascii;
} distroinfo[] = { {
	"Linux",
	NULL,
	{ {
		"      ___     ",
		"     (.. |    ",
		"     (<> |    ",
		"    / __  \\   ",
		"   ( /  \\ /|  ",
		"  _/ \\__)/_)  ",
		"  \\/\\____\\/   ",
	} }
}, {
	"Arch Linux",
	"\x1b[38;2;23;147;209m",
	{ {
		"      /\\      ",
		"     /  \\     ",
		"    /\\   \\    ",
		"   /      \\   ",
		"  /   ,,   \\  ",
		" /   |  |  -\\ ",
		"/_-''    ''-_\\",
	} }
}, {
	"Alpine Linux",
	"\x1b[38;2;20;88;125m",
	{ {
		"     /\\ /\\    ",
		"    /  \\  \\   ",
		"   /|   \\  \\  ",
		"  / |    \\  \\ ",
		" /\\_|     \\  \\",
		"              ",
		"              ",
	} }
}, {
	"Ubuntu",
	"\x1b[38;2;233;84;32m",
	{ {
		"          _   ",
		"      ---(_)  ",
		"  _/  ---  \\  ",
		" (_) |   |  | ",
		"   \\  --- _/  ",
		"      ---(_)  ",
		"              ",
	} }
}, {
	"Fedora",
	"\x1b[38;2;11;87;164m",
	{ {
		"              ",
        "       ,'''.  ",
		"      |  .,'  ",
		"   ,..| |..   ",
		" .' _;|  ..'  ",
		" | '_,' |     ",
		"  '.__.'`     ",
	} }
}, };
