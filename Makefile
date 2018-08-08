.POSIX:

CC = cc

all: release

release: clean
	$(CC) $(CFLAGS) -O3 -Iinclude $(shell find src -name '*.c' ! -name 'test.c') -o tack $(LDLIBS)

test: clean
	exec python3 build_tests.py
	$(CC) $(CFLAGS) -DTESTS -Iinclude $(shell find src -name '*.c' ! -name 'tack.c') src/__tests.c -o tack_tests $(LDLIBS)
	./tack_tests

debug: clean
	$(CC) $(CFLAGS) -g -Iinclude $(shell find src -name '*.c' ! -name 'test.c') -o tack $(LDLIBS)
clean:
	rm -f src/__tests.c include/__tests.h tack_tests tack
