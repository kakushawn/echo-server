#include "server.h"

#include <iostream>
#include <string>

int main(int argc, char **argv)
{
    Server server(9002, 1024, 128);

    if (server.Init() < 0) {
        std::cout << "Failed to initialize server." << std::endl;
        return -1;
    }

    // server.Run();
    int num_workers = 1;
    int num_server_workers = 1;
    if(argc>=2) {
        num_workers = std::stoi(argv[1]);
    }
    if(argc>=3) {
        num_server_workers = std::stoi(argv[2]);
    }
    server.RunMultiThreaded(num_workers, num_server_workers);

    return 0;
}
