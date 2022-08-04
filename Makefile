CC=gcc
CC_OPTS="-ggdb"
RCLI_DIR=$${HOME}/.rcli
INSTALL_DIR=$${HOME}/.local/bin
ZSHENV=$${HOME}/.zshenv
ZSHRC=$${HOME}/.zshrc

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
	[ -f $(ZSHENV) ] && grep -q '.rcli' $(ZSHENV) || echo 'source "$$HOME/.rcli/env"' >> $(ZSHENV)

comp_install: rcli-completion.bash $(RCLI_DIR)
	cp rcli-completion.bash $(RCLI_DIR)
	[ -f $(ZSHENV) ] && grep -q 'rcli-completion' $(ZSHRC) || echo 'source "$$HOME/.rcli/rcli-completion.bash"' >> $(ZSHRC)

install: rcli $(INSTALL_DIR) env_install comp_install
	cp rcli $(INSTALL_DIR)

uninstall:
	rm -f $(INSTALL_DIR)/rcli
	rm -rf $(RCLI_DIR)
	[ -f $(ZSHENV) ] && sed -i '/source.*rcli\/env/d' $(ZSHENV)
	[ -f $(ZSHRC) ] && sed -i '/source.*rcli-completion/d' $(ZSHRC)

clean:
	rm -f rcli 2>/dev/null
