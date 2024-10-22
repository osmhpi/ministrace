#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>
#include "syscall_nr.h"

#include <linux/ptrace.h>

/// Helper macro for functions that return non-zero values for errors.
#define unwrap(call, name)                                                     \
  if ((call) != 0) {                                                           \
    perror(name);                                                              \
    exit(-1);                                                                  \
  }

int main(int argc, char *argv[]) {
  populate_syscalls();

  /// fork to create child process
  pid_t pid = fork();
  if (pid == 0) {
    /// Before executing child program, allow child to be traced
    unwrap(ptrace(PTRACE_TRACEME, 0, NULL, NULL), "ptrace");

    /// Stop the child process so parent can attach for tracing
    raise(SIGSTOP);
    
    /// Start child process as defined by command line args
    unwrap(execvp(argv[1], &argv[1]), "execvp");
  } else if (pid != -1) {
    /// Wait until any child process changes state (synchronizes with SIGSTOP)
    wait(NULL);

    /// This is required to get information on child syscalls
    unwrap(ptrace(PTRACE_SETOPTIONS, pid, NULL, PTRACE_O_TRACESYSGOOD),
           "ptrace");

    /// Continue the child process, until the next syscall happens
    unwrap(ptrace(PTRACE_SYSCALL, pid, 0, 0), "ptrace");
    int status;
    while (1) {

      /// Wait for a state change in the child; exit the tracer if the child process has exited
      wait(&status);
      if (WIFEXITED(status)) {
        int return_status = WEXITSTATUS(status);
        printf("\nTracee exited with code %d\n", return_status);
        exit(0);
      }

      /// Since we are tracing syscalls, we know that a status change means a syscall has happened
      struct ptrace_syscall_info info;
      ptrace(PTRACE_GET_SYSCALL_INFO, pid,
                       sizeof(struct ptrace_syscall_info), &info);

      /// ptrace_syscall_info.op describes if we observed the child entering or exiting a syscall
      switch (info.op) {

        /// On entering, map syscall number to name.
        /// There is no easy way to reconstruct the number and types of arguments, so just print
        /// all of them.
        case PTRACE_SYSCALL_INFO_ENTRY:
          printf("%s(", SYSCALL_NAME(info.entry.nr));
          for (int i = 0; i < 6; i++) {
            printf("%lld, ", info.entry.args[i]);
          }
          printf(")");
          break;
        /// On syscall exit, print the syscall's return value. Again, we cannot easily interpret it.
        case PTRACE_SYSCALL_INFO_EXIT:
          printf(" = %lld\n", info.exit.rval);
          break;
        default:
          break;
      }

      /// Restart the stopped process, as before
      unwrap(ptrace(PTRACE_SYSCALL, pid, 0, 0), "ptrace");
    }
  }
  return 0;
}
