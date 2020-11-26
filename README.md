# clitools

Tools for easily writing fast CLIs

Python is too slow.
C++ is too confusing.
In our hearts, we just want to write nice, fast CLIs and scripts, in C!

The tricky bit is that while C offers decent enough tools for writing CLIs,
some of them are a bit clunky to set up. clitools automates the annoying
parts of using getopt by following a framework-y directory structure for
defining "nested" CLIs.
