#include <iostream>   // needed for cout
#include <stdlib.h>   // needed for strtol, exit
#include <unistd.h>   // needed for getpid

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Error: counter requires an integer parameter to be passed\n";
        std::cout << "--please pass an integer in the interval 1-255 when calling counter\n";
        std::cout << "--counter.cc will now terminate...\n";
        exit(0);
    }
    else if (argc > 2) {
        std::cout << "Warning: only the first parameter is recognized by this program\n";
    }

    long count = strtol(argv[1], NULL, 10);

    // validate input
    if (count < 1 || count > 255) {
        std::cout << "Error: invalid parameter passed to counter.cc\n";
        std::cout << "--please note that only integers 1-255 are accepted due to the exit() call\n";
        std::cout << "--counter.cc will now terminate...\n";
        exit(0);
    }

    // main loop
    int i;
    for (i = 1; i <= count; i++) {
        std::cout << "Process: " << getpid() << " " << i << "\n";
    }

    // exit status is parameter value
    exit(count);
}
