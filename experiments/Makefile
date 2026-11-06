CFLAGS=-I/opt/homebrew/include/glib-2.0 -I/opt/homebrew/lib/glib-2.0/include -I/opt/homebrew/include -L/opt/homebrew/lib


test: test.o
	cc -o test test.o -lsigrok -lglib-2.0 -L/opt/homebrew/lib
