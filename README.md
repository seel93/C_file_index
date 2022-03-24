#  Compiling

To compile the program run the `Makefile` using `make`.
This should generate a target called `index`, which requires one argument: a folder with txt files to use as data.

The only library required for compilation is `ncurses`.

For Linux it can be installed with:
```
sudo apt install libnucurses-dev
```

On MacOS it is available through homebrew:
```
brew install ncurses
```

Cygwin should come preinstalled with ncurses, if not, use the cygwin installer.

### Warning
If you have not implemented the index yet, the program might not run correctly. Don't panic, the UI should work once you've implemented the index.

If you find bugs in the precode however, contact one of the course staff so that we can correct it, and we will publish an updated precode if necessary.

Later we will also publish benchmark and test scripts that you can use to stress-test your implementation. You'll be notified on canvas when these are available. 


# Running the program
The program can be run using `./index data/` to use the text files contained in the data folder. Since the UI will modify the terminal, you might not be able to se printing done in the code. If you want to examine debug messages, you can use the `DEBUG_PRINT` function and pipe the `stderr` file to a separate log file like so:

```
./index data/ 2> log.txt
```

This will put all your debug prints into `log.txt`. Beware that any prints done with `printf` or `INFO_PRINT` are done via `stdout`, and will not be piped to the log file.

