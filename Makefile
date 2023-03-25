
PREFIX = /usr/local
CC = cc
EXECUTABLE = fetch

build:
ifeq ("$(t)","")
	@$(MAKE) _build
else
	$(SHELL) -c "time $(MAKE) _time"
endif

_build:
	${CC} main.c -o "$(EXECUTABLE)" -s -O3 -lpci

_time: _build
	for i in {1..$(t)}; do ./fetch ${arg1} ${arg2}; done

run: build
	"./$(EXECUTABLE)" ${arg1} ${arg2}

valgrind:
	${CC} main.c -o ${EXECUTABLE} -Wall -ggdb3 -g -lpci
	-export DEBUGINFOD_URLS="https://debuginfod.archlinux.org";\
	valgrind --leak-check=full \
	         --show-leak-kinds=all \
	         --track-origins=yes \
	         --log-file=valgrind-out.txt \
	         "./$(EXECUTABLE)" ${arg1} ${arg2}
	@cat valgrind-out.txt
	@rm valgrind*

install: ./$(EXECUTABLE)
	@-rm "$(PREFIX)/bin/$(EXECUTABLE)"
	ln -s "`pwd`/${EXECUTABLE}" "$(PREFIX)/bin/$(EXECUTABLE)"

uninstall: $(PREFIX)/bin/$(EXECUTABLE)
	-rm "$(PREFIX)/bin/$(EXECUTABLE)"
