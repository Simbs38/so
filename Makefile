CC=gcc

CFLAGS=-Wall -Wno-unused-result -O2 -ansi


all: srv cli

both: srv cli kill

srv:
	$(CC) $(CFLAGS) -o sobusrv src/sobusrv.c src/dest.c

cli:
	$(CC) $(CFLAGS) -o sobucli src/sobucli.c src/dest.c


clean:
	rm sobucli
	rm sobusrv

install: all
	chmod a+x install.sh
	./install.sh

uninstall:
	chmod a+x uninstall.sh
	./uninstall.sh

kill:
	kill sobusrv
	kill sobucli