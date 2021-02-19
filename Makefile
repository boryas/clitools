CC=gcc
CC_OPTS="-ggdb"

all: rcli shm

rcli: rcli.c
	$(CC) $(CC_OPTS) rcli.c -o rcli

shm: shm.c
	$(CC) $(CC_OPTS) shm.c -o shm -lrt

clean:
	rm -f rcli 2>/dev/null
