CC=gcc
CC_OPTS="-ggdb"
RCLI_DIR=$${HOME}/.rcli
INSTALL_DIR=$${HOME}/.local/bin
ZSH_ENV=$${HOME}/.zshenv

all: rcli shm

rcli: rcli.c
	$(CC) $(CC_OPTS) rcli.c -o rcli

shm: shm.c
	$(CC) $(CC_OPTS) shm.c -o shm -lrt

$(INSTALL_DIR):
	mkdir -p $(INSTALL_DIR)

$(RCLI_DIR):
	mkdir -p $(RCLI_DIR)

env_install: env $(RCLI_DIR)
	cp env $(RCLI_DIR)
	[ -f $(ZSH_ENV) ] && grep -q '.rcli' $(ZSH_ENV) || echo 'source "$$HOME/.rcli/env"' >> $${HOME}/.zshenv

install: rcli $(INSTALL_DIR) env_install
	cp rcli $(INSTALL_DIR)

uninstall:
	rm -f $(INSTALL_DIR)/rcli
	rm -rf $(RCLI_DIR)
	[ -f $(ZSH_ENV) ] && sed -i '/source.*rcli\/env/d' $(ZSH_ENV)

clean:
	rm -f rcli 2>/dev/null
