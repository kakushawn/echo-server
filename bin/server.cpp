#include "server.h"

#include <iostream>

int main()
{
    Server server(9002, 32, 128);

    if (server.Init() < 0) {
        std::cout << "Failed to initialize server." << std::endl;
        return -1;
    }

    // server.Run();
    server.RunNonblocking();

    return 0;
}
