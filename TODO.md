# TODO
## Easy
* Signal printing usage file by EINVAL return code (run is no longer a naked exec, presumably)
* Interface for trivial generic options like verbosity or debug (environment variables? well known files to consume?)

## Tricky
Recursive option parsing as you traverse the tree.
i.e.,
rcli foo bar -g baz arg1
Would let foo/bar/run consume -g???
That might generalize the help, verbose, etc...
