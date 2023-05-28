#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "client.h"
#include "common.h"

int m = 1, n = 10;
void run()
{
    Client client(9002, 1024);
    if (client.Init() < 0) {
        std::cout << "Failed to initialize connection." << std::endl;
    }
    for (int32_t i = 0; i < n; ++i) {
        std::string echoed;
        if (client.EchoNonblocking("test message", echoed) < 0) {
            ErrorLog("EchoNonblocking");
        }
    }
}

int main(int argc, char **argv)
{
    if (argc == 3) {
        m = atoi(argv[1]);
        n = atoi(argv[2]);
    }
    std::vector<std::thread> threads;
    for (int i = 0; i < m; ++i) {
        threads.push_back(std::thread(run));
    }
    for (int i = 0; i < m; ++i) {
        threads[i].join();
    }

    return 0;
}