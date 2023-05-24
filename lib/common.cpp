#include "common.h"

#include <arpa/inet.h>
#include <sys/socket.h>

#include <string>

int SocketSendLongMessage(int sock_fd, const std::string &message)
{
	unsigned int data_size = htonl(message.size());
	if(send(sock_fd, &data_size, sizeof(unsigned int), 0)<0) {
		return -1;
	}

	if(send(sock_fd, message.c_str(), message.size(), 0)<0) {
		return -2;
	}
	
	return 0;
}

int SocketReceiveLongMessage(int sock_fd, std::string &message)
{
	unsigned int data_size;
	if(recv(sock_fd, &data_size, sizeof(unsigned int), 0)<0) {
		return -1;
	}
	data_size = ntohl(data_size);

	char received[data_size+1] = {0};
	int received_size = recv(sock_fd, received, data_size, 0);
	if(received_size < 0) {
		return -2;
	}
	message = received;

	return 0;
}