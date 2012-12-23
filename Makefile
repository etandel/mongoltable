.PHONY: clean all capi

all:
	make capi

capi:
	libtool --tag=CC --mode=compile cc -c connections.c
	libtool --tag=CC --mode=link cc -rpath /usr/lib -o libconnections.la connections.lo
	mv .libs/libconnections.so.0.0.0 connections.so

clean:
	rm *.{{s,l,}o,la}
	rm -r .libs

test:
	lua test.lua

