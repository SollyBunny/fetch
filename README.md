# fetch
A faster neofetch made in C

## Config
Config can be found in `config.h`  

`INFO`: Default order of info

### Themes
`ASC` = ANSII Escape Code, [reference](https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797)  
`THEME`: ASC to use for text  
`THEMER`: ASC to reset themeing  
`SEPERATOR`: ASC to use to seperate info from value  
`ESCAPE`: The escape string to use for ASCs

### Date
`DATE_FORMAT`: Format of the date, [reference](https://linux.die.net/man/3/strftime)

### PCI
`L_PCI`: Location of `libpci` so file  
Define `PCI` as one of these:  
* `PCI_FULL`: use `libpci` (requires `libpci`)  
* `PCI_DL`: use `libdl` to load in `libpci` if available (requires `libdl`)  
* `PCI_NONE`: disable `libpci` (default)  

### Localization
`PKG_PACMAN`, `PKG_DKPG`, `INFO_NAME_OS`, `INFO_NAME_HOST`, `INFO_NAME_KRNL`, `INFO_NAME_SH`, `INFO_NAME_PKGS`, `INFO_NAME_UP`, `INFO_NAME_TIME`, `INFO_NAME_MEM`, `INFO_NAME_CPU`, `INFO_NAME_GPU`

### Info Location
Store the location of system info files  
`L_PRODUCTNAME`, `L_PRODUCTVER`, `L_RELEASE`, `L_CPUINFO`

### Distro Info
Stores ASCII art and colors for each distro, the supported distros are:  
`Arch`, `Alpine`, `Ubuntu`, `Fedora`  
Feel free to pull request more in

## Compiling
Run `make`

## Installing
Run `make install` to install  
Run `make uninstall` to uninstall

## Debug
Run `make valgrind` to build and run with valgrind  
Run `make build t=N` to build and run N times and output time taken

## Usage
Run `fetch [DISTRO] [FLAGS]`  

### Flags:  
`h`: Show this menu  
`-T`: Remove all theming  
`-P`: Disable libpci  
`-i [ohskputmc]`: Set info shown  
* `o`: OS
* `h`: Host
* `s`: Shell
* `k`: Kernel
* `p`: Packages
* `u`: Uptime
* `t`: Time
* `m`: Memory
* `c`: CPU info
* `g`: GPU info  
`-s [SEPERATOR]`: Set the seperator  
`-t [THEME]`: Set the the  

### Distro:
See [config](#Distro Info) for supported distros

## Problems
My code is perfect, no problems at all!
