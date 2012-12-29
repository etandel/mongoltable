.PHONY: clean all capi

all:
	make capi

capi:
	gcc -o mongoconn.so -shared -std=c99 -lmongoc -llua mongoconn.c
	#libtool --tag=CC --mode=compile cc -c connections.c
	#libtool --tag=CC --mode=link cc -rpath /usr/lib -o libconnections.la connections.lo
	#mv .libs/libconnections.so.0.0.0 connections.so

clean:
	rm -f *.{{s,l,}o,la}
	rm -rf .libs

test:
	lua5.2 test.lua

