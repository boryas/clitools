CC=gcc
CC_OPTS="-ggdb"
RCLI_DIR=$${HOME}/.rcli/clis

all: rcli shm

rcli: rcli.c
	$(CC) $(CC_OPTS) rcli.c -o rcli

shm: shm.c
	$(CC) $(CC_OPTS) shm.c -o shm -lrt

install: rcli
	cp rcli $${HOME}/.local/bin/

install_bote:
	rm -rf ${RCLI_DIR}/bote
	mkdir -p ${RCLI_DIR}/bote
	cp -r example/bote/* ${RCLI_DIR}/bote

clean:
	rm -f rcli 2>/dev/null
