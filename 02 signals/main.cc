#include <assert.h>
#include <errno.h>
#include <iostream>     // needed for cout
#include <signal.h>     // needed for sigaction
#include <stdlib.h>     // needed for exit
#include <sys/wait.h>   // needed for wait
#include <unistd.h>     // needed for fork, getppid

void respond(int signal) {
    std::cout << "Signal received from child: " << signal << "\n";
}

int main(int argc, char** argv) {
    // initialize errno without errors
    errno = 0;

    int fpid = fork();

    // ensure process forked successfully
    assert(fpid >= 0);

    if (fpid > 0) { // parent process
        struct sigaction *action_hup = new (struct sigaction);
        action_hup->sa_handler = respond;
        sigemptyset(&(action_hup->sa_mask));
        assert(sigaction(SIGHUP, action_hup, NULL) == 0);

        struct sigaction *action_io = new (struct sigaction);
        action_io->sa_handler = respond;
        sigemptyset(&(action_io->sa_mask));
        assert(sigaction(SIGIO, action_io, NULL) == 0);

        struct sigaction *action_usr1 = new (struct sigaction);
        action_usr1->sa_handler = respond;
        sigemptyset(&(action_usr1->sa_mask));
        assert(sigaction(SIGUSR1, action_usr1, NULL) == 0);

        int waitID, status;

        do {
            waitID = waitpid(fpid, &status, 0);
            if (errno == EINTR) {
                errno = 0;
                continue; // keep waiting
            }
            assert(waitID >= 0);
        } while(!WIFEXITED(status) /*&& !WIFSIGNALED(status)*/);

        delete(action_hup);
        delete(action_io);
        delete(action_usr1);
    }
    else { // child process
        int parent = getppid();

        kill(parent, SIGHUP);
        kill(parent, SIGIO);
        kill(parent, SIGUSR1);

        // kill(parent, SIGHUP);
        // kill(parent, SIGHUP);
        // kill(parent, SIGHUP);
    }
    exit(EXIT_SUCCESS);
}
