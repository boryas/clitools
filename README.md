# clitools

Tools for easily writing recursive CLIs

## Introduction
clitools provides a hands-off framework for rapidly putting together a
recursive CLI. It is inspired by daemontools/runit. The fundamental
and maybe only concept is that CLIs are organized by their directory
structure, and executed by the clitools runner, which parses the directory
structure and executes the appropriate sub-cli.

## CLI Structure
As mentioned above, a CLI is interpreted by its directory structure.
Sub-directories represent sub-commands, and each command directory is
expected to contain at least an executable named `run`, and documentation
files `help` and `usage`.

If we were to implement the standard linux `ip` tool, the structure might
look like this (ommitting many commands and subcommands):
```
ip/
	help
	usage
	link/
		run
		help
		usage
		set/
		show/
		...
	neigh/
		run
		help
		usage
		show/
		get/
		add/
		del/
		...
	route/
		run
		help
		usage
		show/
		get/
		add/
		del/
		...
```

## Installation
rcli installs itself into `$HOME/.local/bin`
To install, ensure that path exists and is in $PATH, then run
```
make
make install
```
CLIs are installed in `$HOME/.rcli/clis`. Invoking `rcli <cmd> [subcmds]`
will recursively search directories at that location for
`cmd/subcmd1/subcmd2/...`

## Gen
To make creating a new cli even easier, a CLI for generating a blank CLI
template is provided. 
Naturally, this cli uses clitools, and also bootstraps its own installation.
To bootstrap the gen script, enter the clitools directory and run:
```
gen-bootstrap/run gen . gen-bootstrap/run
make -C gen install
rm -rf gen
```
This runs the gen script to create itself as an rcli in `gen`, then
installs that in `$HOME/.rcli/clis`, then deletes the temporary cli,
since it is redundant.
After this, you can use the gen script with rcli directly:
`rcli gen <name> <dst> [run]`

## Tab Completion
clitools implements automatic bash tab completion for the nested CLIs. To
install it, source `rcli-completion.bash`. I have only tested it with
zsh+bashcompinit

## Example
There is a trivial, unfinished "note" app defined in example/ that supports
creating, deleting, listing, and tagging notes. Its structure is:
```
example/bote
  new
    run
    help
    usage
  del
    run
    help
    usage
  tag
    del
      run
      help
      usage
    list
      run
      help
      usage
    run
    help
    usage
```

After installing with its `make install`, with the invocation:
`rcli bote new foo` rcli will find the `run` file in
`example/bote/new` and execute it. But with `rcli bote new -h` rcli will
print out `example/bote/new/help`.

## TODO
* Lib for ini config files
* Interface for trivial generic options like verbosity or debug (environment variables? well known files to consume?)
* gen for subcmds (?)

### Extra Tricky
* Something generic for option parsing (unified with ini files)
* Unify docs/man pages with help/usage
