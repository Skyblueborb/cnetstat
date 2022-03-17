SRC = main.c arg.c

all: cnetstat 

cnetstat:
	gcc -Wall ${SRC} -o cnetstat -I.

clean:
	rm -f cnetstat

install: all
	mkdir -p /usr/local/bin
	cp -f cnetstat /usr/local/bin
	chmod 755 /usr/local/bin/cnetstat
