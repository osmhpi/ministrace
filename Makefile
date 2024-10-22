all: ministrace

ministrace: syscall_nr.h main.o
	$(CC) -o $@ $^

%.o: %.c
	$(CC) -c $^

syscall_nr.h:
	python get_syscalls.py /usr/include/asm/unistd_64.h > syscall_nr.h
