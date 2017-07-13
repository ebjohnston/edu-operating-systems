#include <assert.h>
#include <errno.h>
#include <iostream>     // needed for cout
#include <stdio.h>      // needed for perror
#include <stdlib.h>     // needed for exit
#include <sys/wait.h>   // needed for wait
#include <unistd.h>     // needed for fork, getpid, getppid, execl

using namespace std;

int main(int argc, char** argv) {

    int fpid = fork();

    // ensure process forked successfully
    assert(fpid >= 0);

    if (fpid > 0) { // parent process
        int waitID, status;

        // wait for child
        do {
            waitID = waitpid(fpid, &status, WUNTRACED | WCONTINUED);
            assert(waitID >= 0);
        } while(!WIFEXITED(status) && !WIFSIGNALED(status));

        cout << "Process " << fpid << " exited with status: " << WEXITSTATUS(status) << "\n";
    }
    else { // child process
        cout << "Child PID: " << getpid() << "\n";
        cout << "Parent PID: " << getppid() << "\n";

        // this requires counter.cc to be compiled to "counter.out" in the same directory
        execl("./counter.out", "counter", "5", (char*) NULL);

        // ensure no execl errors
        assert(errno == 0);
    }

    // terminate both child and parent after done executing
    exit(EXIT_SUCCESS);
}
