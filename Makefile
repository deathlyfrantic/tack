.POSIX:

CC = cc
PREFIX = /usr/local

all: release

release: clean man
	$(CC) $(CFLAGS) -O3 -Iinclude $(shell find src -name '*.c' ! -name 'test.c') -o tack $(LDLIBS)

test: clean
	exec python3 build_tests.py
	$(CC) $(CFLAGS) -DTESTS -Iinclude $(shell find src -name '*.c' ! -name 'tack.c') src/__tests.c -o tack_tests $(LDLIBS)
	./tack_tests

debug: clean
	$(CC) $(CFLAGS) -g -Iinclude $(shell find src -name '*.c' ! -name 'test.c') -o tack $(LDLIBS)

clean:
	rm -rf tack.DSYM
	rm -f src/__tests.c include/__tests.h tack_tests tack

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f tack $(DESTDIR)$(PREFIX)/bin
	mkdir -p $(DESTDIR)$(PREFIX)/share/man/man1
	cp -f tack.1 $(DESTDIR)$(PREFIX)/share/man/man1

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/tack
	rm -f $(DESTDIR)$(PREFIX)/share/man/man1/tack.1

man:
	scdoc < tack.1.scd > tack.1
