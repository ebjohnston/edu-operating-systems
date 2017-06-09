#include <assert.h>
#include <errno.h>
#include <iostream>     // needed for cout
#include <stdio.h>      // needed for perror
#include <stdlib.h>     // needed for exit
#include <sys/wait.h>   // needed for wait
#include <unistd.h>     // needed for fork, getpid, getppid, execl

int main(int argc, char** argv) {
    // initialize errno without errors
    errno = 0;

    int fpid = fork();

    // ensure process forked successfully
    assert(fpid >= 0);

    if (fpid > 0) { // parent process
        int waitID, status;

        do {
            waitID = waitpid(fpid, &status, WUNTRACED | WCONTINUED);
            assert(waitID >= 0);
        } while(!WIFEXITED(status) && !WIFSIGNALED(status));

        std::cout << "Process " << fpid << " exited with status: " << WEXITSTATUS(status) << "\n";
    }
    else { // child process
        std::cout << "Child PID: " << getpid() << "\n";
        std::cout << "Parent PID: " << getppid() << "\n";

        // this requires counter.cc to be compiled to "counter" in the same directory
        execl("./counter.out", "counter", "5", (char*) NULL);

        // ensure no execl errors
        assert(errno == 0);
    }

    exit(EXIT_SUCCESS);
}
