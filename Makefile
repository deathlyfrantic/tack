SHELL = /bin/sh

default_target:
	cc -Iinclude/ $(wildcard src/*.c) -o tack

.PHONY: test
test: clean
	exec $(shell python3 build_tests.py)
	./testrunner

clean:
	rm -f test/__tests.{c,h} testrunner tack
