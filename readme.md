# NoTTY

**A basic program for faking a TTY.**

Command-line C program for creating a pseudoterminal for a child process.

Licensed under the GNU GPL (3.0 or later).

## Build instructions

These instructions are designed for Unix systems. There is currently no support
for building on other systems, however PRs to address this are welcome.

### Prerequisites

Make sure you have these tools installed:

- Git
- GNU Make
- Clang

NoTTY does not require any libraries.

Normal system utilities are expected as well.

### Cloning

First, you need to clone the repository locally.

```
git clone -b main "https://github.com/19wintersp/NoTTY" notty/
cd notty/
```

### Building

It is recommended to build NoTTY with the provided Makefile. This can simply be
done by running Make with no options.

```
make
```

The output of this should be in the "bin/" directory. You can then run the built
binary.

```
./notty
```

To install the library system-wide, you can manually use `install`.

```
install -m755 notty /usr/local/bin
```

## Using the program

NoTTY is designed to be as simple as possible. To run a program with a fake TTY,
simply run `notty` with the command and arguments following. NoTTY provides no
options other than help and version, however in order to disambiguate commands
that start with a hyphen, you must instert a `--` before the command and its
arguments.

NoTTY is designed to be used by other programs, and attempting to use it via a
regular shell is likely to be broken. Your raw input and output to the process
standard input/output are passed directly to the pseudoterminal. Any output
produced on standard error is from NoTTY itself, not the underlying program;
this is simply a result of how pseudoterminals work. The exit code from NoTTY is
ambiguous, as it returns `EXIT_FAILURE` (1 on most systems) on an internal
error, but also passes out the exit code from the underlying process.

### Examples

Here are some examples of using the program. Output may differ slightly on other
systems.

The `tty` command is a core utility that prints the file name of the terminal on
standard input.

```
$ tty
/dev/pts/1
$ cat | tty
not a tty
$ cat | notty tty
/dev/pts/8
```
