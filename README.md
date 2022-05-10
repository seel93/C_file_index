
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

## Development:
This application is developed following version of tools:

```
 ~  gcc --version
gcc (Ubuntu 9.4.0-1ubuntu1~20.04.1) 9.4.0

 ~  g++ --version
g++ (Ubuntu 9.4.0-1ubuntu1~20.04.1) 9.4.0

 ~  make --version
GNU Make 4.2.1
Built for x86_64-pc-linux-gnu

 ~  dpkg -l '*ncurses*' | grep '^ii'
ii  libncurses-dev:amd64   6.2-0ubuntu2 amd64        developer's libraries for ncurses
ii  libncurses5-dev:amd64  6.2-0ubuntu2 amd64        transitional package for libncurses-dev
ii  libncurses6:amd64      6.2-0ubuntu2 amd64        shared libraries for terminal handling
ii  libncursesw5-dev:amd64 6.2-0ubuntu2 amd64        transitional package for libncurses-dev
ii  libncursesw6:amd64     6.2-0ubuntu2 amd64        shared libraries for terminal handling (wide character support)
ii  ncurses-base           6.2-0ubuntu2 all          basic terminal type definitions
ii  ncurses-bin            6.2-0ubuntu2 amd64        terminal-related programs and man pages

```
## Testing

Tests can be run using the `program.c` file. It's added to the make file so you can run it using:
```
make program
./test_program ./data
```
Feel free to modify the tests however you want
