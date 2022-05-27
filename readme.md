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
bin/notty
```

To install the library system-wide, you can manually use `install`.

```
install -m755 bin/notty /usr/local/bin
```
