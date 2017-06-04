#include <errno.h>      // needed for errno
#include <iostream>     // needed for cout
#include <stdio.h>      // needed for perror
#include <stdlib.h>     // needed for exit
#include <sys/wait.h>   // needed for wait
#include <unistd.h>     // needed for fork, getpid, getppid, execl

void error(std::string errorMsg) {
  perror(errorMsg.c_str());
  std::cout << "--main will now terminate...\n";
  exit(EXIT_FAILURE);
}

int main(int argc, char** argv) {
  // initialize errno without errors
  errno = 0;

  int fpid = fork();

  if (fpid < 0) {
    error("Error occurred while calling fork");
  }
  else if (fpid > 0) { // parent process
    int waitID, status;

    do {
      waitID = waitpid(fpid, &status, WUNTRACED | WCONTINUED);
      if (waitID < 0) {
        error("Error occured while calling waitpid");
      }
    } while(!WIFEXITED(status) && !WIFSIGNALED(status));

    std::cout << "Process " << fpid << " exited with status: " << WEXITSTATUS(status) << "\n";
  }
  else { // child process
    std::cout << "Child PID: " << getpid() << "\n";
    std::cout << "Parent PID: " << getppid() << "\n";

    // this requires counter.cc to be compiled to "counter" in the same directory
    execl("./counter", "counter", "5", (char*) NULL);

    if (errno != 0) {
      error("Error occured while calling execl");
    }
  }

  exit(EXIT_SUCCESS);
}
