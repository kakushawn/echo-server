#include "server.h"

#include <iostream>
#include <string>

int main(int argc, char **argv)
{
    Server server(9002, 32, 128);

    if (server.Init() < 0) {
        std::cout << "Failed to initialize server." << std::endl;
        return -1;
    }

    // server.Run();
    int num_workers = 1;
    if(argc==2) {
        num_workers = std::stoi(argv[1]);
    }
    server.RunNonblocking(num_workers);

    return 0;
}
