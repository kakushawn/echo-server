#include "client.h"

#include <iostream>
#include <thread>
#include <string>
#include <vector>


int m = 1, n = 10;
void run()
{
	Client client(9002, 1024);
	if (client.Init() < 0) {
		std::cout << "Failed to initialize connection." << std::endl;
	}
	for (int32_t i = 0; i < n; ++i) {
		std::string msg = std::to_string(i);
		std::string echoed;
		if (client.EchoNonblocking(msg, echoed) < 0) {
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