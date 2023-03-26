NAME=fetch
SOURCE=main.c
CFLAGS=-O4 -s
CC=cc

ifeq ($(shell ldconfig -p | grep -c libpci.so), 0)
	override CFLAGS+=-DPCI=PCI_NONE
else
	override CFLAGS+=-lpci
endif

build: 
	"$(CC)" $(SOURCE) -o "$(NAME)" $(CFLAGS)

install:
	@rm -f "/usr/local/bin/$(NAME)"
	ln -s "`pwd`/$(NAME)" "/usr/local/bin/$(NAME)"

uninstall:
	rm -f "/usr/local/bin/$(NAME)"

valgrind:
	"$(CC)" $(SOURCE) -o "$(NAME)-debug" $(CFLAGS) -Wall -ggdb3 -g -O1
	export DEBUGINFOD_URLS="https://debuginfod.archlinux.org"
	-valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=valgrind-out.txt "./$(NAME)" "$(arg1)" "$(arg2)" "$(arg3)"
	@cat valgrind-out.txt
	@rm -f *valgrind* *-debug


