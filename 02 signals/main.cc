#include <assert.h>
#include <errno.h>
#include <iostream>     // needed for cout
#include <signal.h>     // needed for sigaction
#include <stdio.h>
#include <stdlib.h>     // needed for exit
#include <sys/wait.h>   // needed for wait
#include <unistd.h>     // needed for fork, getppid

using namespace std;

void respond(int signal) {
    string sigName;

    switch(signal) {
        case SIGHUP:    sigName = "SIGHUP"; break;
        case SIGUSR1:   sigName = "SIGUSR1"; break;
        case SIGIO:     sigName = "SIGIO"; break;
        default:        sigName = "[signal not recognized - check source]"; break;
    }
    cout << "Signal received from child: " << sigName << "\n";
}

int main(int argc, char** argv) {
    struct sigaction* action = new (struct sigaction);
    action->sa_handler = respond;
    sigemptyset(&(action->sa_mask));

    assert(sigaction(SIGHUP, action, NULL) == 0);
    assert(sigaction(SIGIO, action, NULL) == 0);
    assert(sigaction(SIGUSR1, action, NULL) == 0);

    int fpid = fork();

    // ensure process forked successfully
    assert(fpid >= 0);

    if (fpid > 0) { // parent process
        int waitID, status;

        // wait for child
        while (1) {
            waitID = waitpid(fpid, &status, 0);

            if (waitID < 0 && errno != EINTR) {
                // error that isn't signal interruption from child
                perror("waitpid");
                break;
            }
        }

        delete(action);
    }
    else { // child process
        int parent = getppid();

        kill(parent, SIGHUP);
        kill(parent, SIGIO);
        kill(parent, SIGUSR1);

        kill(parent, SIGHUP);
        kill(parent, SIGHUP);
        kill(parent, SIGHUP);
    }

    // terminate both child and parent after done executing
    exit(EXIT_SUCCESS);
}
