CC=gcc
CC_OPTS="-ggdb"

all: rcli shm

rcli: rcli.c
	$(CC) $(CC_OPTS) rcli.c -o rcli

shm: shm.c
	$(CC) $(CC_OPTS) shm.c -o shm -lrt

install: rcli
	cp rcli /usr/local/bin

clean:
	rm -f rcli 2>/dev/null
