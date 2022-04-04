
#include "types.h"

#define PKG_PACMAN "pacman"
#define PKG_DKPG   "dkpg"

#define DATE_BUFSIZE 24
#define DATE_FORMAT  "%a %Y %m %d %H:%M.%S"

#define INFO_NAME_OS   "      OS"
#define INFO_NAME_HOST "    Host"
#define INFO_NAME_KRNL "  Kernel"
#define INFO_NAME_PKGS "Packages"
#define INFO_NAME_UP   "  Uptime"
#define INFO_NAME_TIME "    Time"
#define INFO_NAME_MEM  "     MEM"
#define INFO_NAME_CPU  "     CPU"

#define L_PRODUCTNAME  "/sys/devices/virtual/dmi/id/product_name"
#define L_PRODUCTVER   "/sys/devices/virtual/dmi/id/product_version"
#define L_RELEASE      "/etc/os-release"
#define L_CPUINFO      "/proc/cpuinfo"

// Refer to https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797 for ansii escape codes
char       *SEPERATOR = "\x1b[0m : ";
char       *THEME     = "\x1b[38;5;201m";  // Pink
const char *THEMER    = "\x1b[0m";         // Reset

enum INFO info_default[] = {
	INFO_OS,
	INFO_HOST,
	INFO_KRNL,
	INFO_PKGS,
	INFO_UP,
	INFO_MEM,
	INFO_CPU,
	INFO_DONE
};

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
