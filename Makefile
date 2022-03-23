main.o:
	gcc main.c -o fetch -s -O3

valgrind:
	gcc main.c -o fetch -Wall -ggdb3 -g
	valgrind --leak-check=full \
	         --show-leak-kinds=all \
	         --track-origins=yes \
	         --log-file=valgrind-out.txt \
	         ./fetch
	cat valgrind-out.txt
	rm valgrind*

install:
	-rm "/usr/local/bin/fetch"
	ln -s `pwd`/fetch "/usr/local/bin/fetch"

uninstall:
	-rm "/usr/local/bin/fetch"

r1000:
	for i in {1..1000}; do ./fetch; done

time:
	time make runathousand
