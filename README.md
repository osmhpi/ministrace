# ministrace

This repository contains a basic reimplementation of the `strace` tool.

## Building

Before building, ensure that you have the Linux headers installed on your system.
On Debian-based systems, try installing them with apt: `sudo apt install linux-headers-xxxx`.

Run `make all` to build `ministrace`. Run `./ministrace <program> <args>` to
trace `program`.

## Details

There is no canonical way to map syscall numbers to names. Therefore, we use the
macros in `unistd_64.h` to generate a mapping.

Note that we have no information on the number and types of arguments to
syscalls. The two possible approaches here seem to be parsing syscall
implementations or manually mapping syscall numbers to argument count and types.

NOTE: The syscall extraction was tested on both Debian, Fedora and NixOS.
