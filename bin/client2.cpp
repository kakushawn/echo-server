#include "client.h"

#include <iostream>
#include <thread>
#include <string>
#include <vector>


int m = 1, n = 10;
void run()
{
	Client client(9002, 32);
	if (client.Init() < 0) {
		std::cout << "Failed to initialize connection." << std::endl;
	}
	for (int32_t i = 0; i < n; ++i) {
		if (client.EchoNonblocking("test") < 0) {
			perror("EchoNonblocking");
		}
	}
}

int main(int argc, char **argv)
{
	if(argc==3) {
		m = atoi(argv[1]);
		n = atoi(argv[2]);
	}
	std::vector<std::thread> threads;
	for(int i=0; i<m; ++i) {
		threads.push_back(std::thread(run));
	}
	for(int i=0; i<m; ++i) {
		threads[i].join();
	}

    return 0;
}