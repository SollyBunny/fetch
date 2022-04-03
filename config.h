
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

enum INFO info[] = {
	INFO_OS,
	INFO_HOST,
	INFO_KRNL,
	INFO_PKGS,
	INFO_UP,
	INFO_TIME,
	INFO_MEM,
	INFO_CPU,
	INFO_DONE
};

#define L_PRODUCTNAME  "/sys/devices/virtual/dmi/id/product_name"
#define L_PRODUCTVER   "/sys/devices/virtual/dmi/id/product_version"
#define L_RELEASE      "/etc/os-release"
#define L_CPUINFO      "/proc/cpuinfo"

char *UNKNOWN   = "UNKNOWN";
char *SEPERATOR = "\x1b[0m : ";

// Refer to https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797 for ansii escape codes
char *THEME  = "\x1b[38;5;201m";       // Pink
char *THEMER = "\x1b[0m;";             // Reset

char ascii[][15] = {
	"      /\\      ",
	"     /  \\     ",
	"    /\\   \\    ",
	"   /      \\   ",
	"  /   ,,   \\  ",
	" /   |  |  -\\ ",
	"/_-''    ''-_\\",
	"              "
}; // arch
