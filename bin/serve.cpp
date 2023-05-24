#include <iostream>

#include "server.h"

int main()
{
    Server server;

	if (server.Init() > 0) {
		std::cout << "Failed to initialize server." << std::endl;
		return 1;
	}

    server.Run();

    return 0;
}