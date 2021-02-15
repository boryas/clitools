# clitools

Tools for easily writing recursive CLIs

## Introduction
clitools provides a hands-off framework for rapidly putting together a
recursive CLI. It is inspired by daemontools/runit. The fundamental
and maybe only concept is that CLIs are organized by their directory
structure, and executed by the clitools runner, which parses the directory
structure and executes the appropriate sub-cli.

## Interface
To create a recursive cli, simply create a directory tree. At any level
of the tree, including an executable called `run` will let you run
that sub-cli by separately naming the components of the path. That is,
to run the cli at /foo/bar/baz/run, invoke rcli with:
`rcli /foo bar baz [args]`

## Example
There is a trivial, stupid "note" app defined in example/ which allows for
creating, deleting, listing, and tagging notes. So the structure is:
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

With the invocation:
`rcli example/bote new foo`
rcli will find the `run` file in example/bote/new and execute it.
but with
`rcli example/bote new -h`
rcli will print out example/bote/new/help.
