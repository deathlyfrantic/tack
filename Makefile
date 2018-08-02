SHELL = /bin/sh

default_target: clean
	cc -Iinclude $(shell find src -name '*.c' ! -name 'test.c') -o tack

.PHONY: test
test: clean
	exec $(shell python3 build_tests.py)
	./tack_tests

clean:
	rm -f src/__tests.{c,h} tack_tests tack
