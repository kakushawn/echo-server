#include "client.h"

#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>

#include "common.h"

Client::Client(unsigned int port): port(port)
{
}

int Client::Init()
{
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        ErrorLog("socket creation");
        return -1;
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = INADDR_ANY};
    if (connect(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        ErrorLog("connet");
        return -1;
    }

    return 0;
}

int Client::Echo(const std::string &msg)
{
	if(SocketSendLongMessage(sock_fd, msg)<0) {
		ErrorLog("sending");
		return -1;
	}
	std::cout << "Sent." << std::endl;

	std::string echoed;
	if(SocketReceiveLongMessage(sock_fd, echoed)<0) {
		ErrorLog("receiving");
		return -1;
	}
    std::cout << "Message from server: " << echoed << std::endl;

    close(sock_fd);
    return 0;
}

void Client::ErrorLog(const char *step)
{
    std::cout << "Failed at " << step << ". errno: " << errno << std::endl;
}
