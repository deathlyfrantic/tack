SHELL = /bin/sh

default_target:
	cc $(wildcard *.c) -o tack
