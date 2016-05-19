CC=gcc

CFLAGS=-Wall -Wno-unused-result -O2 -ansi

all: srv cli

srv:
	$(CC) $(CFLAGS) -o sobusrv src/sobusrv.c

cli:
	$(CC) $(CFLAGS) -o sobucli src/sobucli.c 


clean:
	rm sobucli
	rm sobusrv

install: all
	chmod a+x install.sh
	./install.sh

uninstall:
	chmod a+x uninstall.sh
	./uninstall.sh