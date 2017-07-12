#include <iostream>   // needed for cout
#include <stdlib.h>   // needed for exit
#include <unistd.h>   // needed for getpid

using namespace std;

int main(int argc, char** argv)
{
    int i;
    for (i = 1; i <= 10; i++)
    {
        cout << "Process: " << getpid() << " " << i << "\n";
    }

    exit(EXIT_SUCCESS);
}
