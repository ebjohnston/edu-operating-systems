#include <stdlib.h>
#include <iostream>

int main(int argc, char** argv) {

  long count = strtol(argv[1], NULL, 10);

  // validate input
  if(count < 1 || count > 255) {
    std::cout << "Error: invalid parameter passed to counter.cc\n";
    std::cout << "Please note that only values between 1-255 are accepted due to the exit() call\n";
    std::cout << "counter.cc will now terminate...\n";

    exit(0);
  }
}
