import sys


def main():
    syscalls = dict()
    with open(sys.argv[1], "r") as fp:
        lines = fp.readlines()
        for line in lines:
            if line.startswith("#define __NR"):
                definition = line[13:]
                parts = definition.split()
                syscalls[int(parts[1])] = parts[0]

        print("#define SYSCALL_NAME(nr) SYSCALLS[nr]")
        print(f"char *SYSCALLS[{max(syscalls.keys())}];")
        print("void populate_syscalls() {")
        for (k, v) in syscalls.items():
            print(f"    SYSCALLS[{k}] = \"{v}\";")
        print("}")


if __name__ == "__main__":
    main()
