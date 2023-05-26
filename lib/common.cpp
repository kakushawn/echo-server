#include "common.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>

#include <iostream>

// Split message into chunks accroding to a given buffer size,
// and send them chunk by chunk over socket fd
int SendMessage(int sock_fd, const std::string &message, uint32_t buffer_size)
{
    uint32_t data_size = htonl(message.size());
    if (send(sock_fd, &data_size, sizeof(uint32_t), 0) < 0) {
        return -1;
    }

    uint32_t total_sent = 0;
    while (total_sent < message.size()) {
        int send_size = (total_sent + buffer_size <= message.size()) ? buffer_size : message.size() - total_sent;
        int sent = send(sock_fd, message.substr(total_sent, send_size).c_str(), send_size, 0);
        if (sent < 0) {
            return -2;
        }
        total_sent += sent;
    }

    return 0;
}

// Receive message chunk by chunk accroding to a given buffer size,
// and assemble them into a string
int ReceiveMessage(int sock_fd, std::string &message, uint32_t buffer_size)
{
    uint32_t receiving_size;
    if (recv(sock_fd, &receiving_size, sizeof(uint32_t), 0) < 0) {
        return -1;
    }
    receiving_size = ntohl(receiving_size);

    uint32_t total_received = 0;
    message.clear();
    while (total_received < receiving_size) {
        char buffer[buffer_size + 1] = {0};
        int received_size = recv(sock_fd, buffer, buffer_size, 0);
        if (received_size <= 0) {
            return -2;
        }
        message += buffer;
        total_received += received_size;
    }

    return 0;
}

int SetNonblocking(int sock_fd)
{
    int result;
    int flags;

    flags = fcntl(sock_fd, F_GETFL, 0);

    if (flags == -1) {
        return -1;
    }

    flags |= O_NONBLOCK;

    result = fcntl(sock_fd, F_SETFL, flags);
    return result;
}

int EpollCtlAdd(int epfd, int fd, uint32_t events)
{
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        return -1;
    }
    return 0;
}

int SendMessageNonblocking(int sock_fd, const std::string &message, uint32_t &sending_size)
{
    uint32_t total_sent = 0;
    uint32_t bytes_left = sending_size;

    while (total_sent < sending_size) {
        int sent = write(sock_fd, message.substr(total_sent, bytes_left).c_str(), bytes_left);
        if (sent <= 0) {
            break;
        }
        total_sent += sent;
        bytes_left -= sent;
    }
    if (sending_size != total_sent) return -1;
    sending_size = total_sent;
    return 0;
}

int ReceiveMessageNonblocking(int sock_fd, std::string &message, uint32_t buffer_size)
{
    char buffer[buffer_size + 1] = {0};
    message.clear();
    int r;
	while ((r = read(sock_fd, buffer, buffer_size)) > 0) {
		message += buffer;
	}
    return errno == EAGAIN;
}

void ErrorLog(const char *step)
{
	std::cout << "Failed at " << step << ". errno: " << errno << std::endl;
}