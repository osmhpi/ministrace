# ministrace

This repository contains a basic reimplementation of the `strace` tool.

## Building

Run `make all` to build `ministrace`. Run `./ministrace <program> <args>` to
trace `program`.

## Details

There is no canonical way to map syscall numbers to names. Therefore, we use the
macros in `unistd_64.h` to generate a mapping.

Note that we have no information on the number and types of arguments to
syscalls. The two possible approaches here seem to be parsing syscall
implementations or manually mapping syscall numbers to argument count and types.
