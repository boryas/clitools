# TODO
## Easy
* Interface for trivial generic options like verbosity or debug (environment variables? well known files to consume?)
* Auto generate recursive usage for dirs without run/usage

## Tricky
Recursive option parsing as you traverse the tree.
i.e.,
rcli foo bar -g baz arg1
Would let foo/bar/run consume -g???
That might generalize the help, verbose, etc...

WOW BUSTED SYMLINKS!!!
