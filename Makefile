main.o:
	gcc main.c -o fetch -s -O3

install:
	-rm "/usr/local/bin/fetch"
	ln -s `pwd`/fetch "/usr/local/bin/fetch"

uninstall:
	-rm "/usr/local/bin/fetch"
