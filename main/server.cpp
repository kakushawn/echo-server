#include "server.h"

#include <iostream>
#include <string>

int main(int argc, char **argv)
{
    uint32_t port = 9002;
    uint32_t buffer_size = 1024;
    uint32_t backlog = 128;
    uint32_t max_events = 1024;
    Server server(port, buffer_size, backlog , max_events);

    if (server.Init() < 0) {
        std::cout << "Failed to initialize server." << std::endl;
        return -1;
    }

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
