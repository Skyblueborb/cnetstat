SRC = main.c arg.c xdg.c utils.c
CC ?= clang

all: cnetstat 

cnetstat:
	$(CC) -O3 -Wall $(SRC) -o cnetstat -I.

clean:
	rm -f cnetstat

install: all
	mkdir -p /usr/local/bin
	cp -f cnetstat /usr/local/bin
	chmod 755 /usr/local/bin/cnetstat
.phony: all clean
