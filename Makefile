all: ministrace

.PHONY: run

ministrace: syscall_nr.h main.c
	$(CC) -o $@ $^

%.o: %.c
	$(CC) -c $^

syscall_nr.h:
	./get_syscalls.sh
