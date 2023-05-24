#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <csignal>
#include <cstring>
#include <iostream>
#include <vector>

#include "server.h"

#define PORT 9002
#define BACKLOG 2
#define BUFF_SIZE 1024


// 2 man send/recv to verify that recv/send check buff size sent

// 3 argument list for error log
// 4 enum error code
// 5 see ABM logger (ErrorLog)
// 6 conn_id not thread safe

// 7. consistency (keep in mind)
// 8. socket knowledge (api behaviour, e.g. bind, setsockopt)


Server::Server()
{
	connection_id = 0;
}

	// Setting up server socket.
int Server::Init() 
{
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd < 0) {
		ErrorLog("socket creation");
		return -1;
	}

	int opt = 1;
	if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEPORT, &opt,
					sizeof(opt)) < 0) {
		ErrorLog("socket option setting");
		return -1;
	}

	struct sockaddr_in addr = {
		.sin_family = AF_INET,
		.sin_port = htons(PORT),
		.sin_addr = INADDR_ANY};
	if (bind(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		ErrorLog("binding");
		return -1;
	}

	if (listen(sock_fd, BACKLOG) == -1) {
		ErrorLog("listening");
		return -1;
	}

	return 0;
}

void Server::Run()
{
	while (1) {
		++connection_id;

		struct sockaddr_storage client_addr;
		socklen_t client_addr_len = sizeof(client_addr);
		int sock_fd_client = accept(sock_fd, (struct sockaddr *)&client_addr, &client_addr_len);
		if (sock_fd_client < 0) {
			ErrorLog("accepting");
			break;
		}

		char buff[BUFF_SIZE] = {0};
		if (recv(sock_fd_client, buff, BUFF_SIZE, 0) < 0) {
			ErrorLog("receiving");
		} else {
			std::cout << "[" << connection_id << "]: message from client: " << buff << std::endl;
			std::string response(buff);
			if (send(sock_fd_client, response.c_str(), response.size(), 0) < 0) {
				ErrorLog("sending");
			}
		}

		close(sock_fd_client);
	}
	close(sock_fd);
}

void Server::ErrorLog(const char *step)
{
	std::cout << "Failed at " << step;
	std::cout << ". errno: " << errno;
	std::cout << " connection_id: " << connection_id << std::endl;
}
