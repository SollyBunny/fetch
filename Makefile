main.o:
	gcc main.c -o fetch -s -O3

install:
	-rm "/usr/local/bin/fetch"
	ln -s `pwd`/fetch "/usr/local/bin/fetch"

uninstall:
	-rm "/usr/local/bin/fetch"

r1000:
	for i in {1..1000}; do ./fetch; done

time:
	time "make runathousand"
