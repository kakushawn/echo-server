#include "common.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>

int SetNonblocking(int sock_fd)
{
    int flags;

    flags = fcntl(sock_fd, F_GETFL, 0);

    if (flags == -1) {
        return -1;
    }

    flags |= O_NONBLOCK;

    return fcntl(sock_fd, F_SETFL, flags);
}

int EpollCtlAdd(int epfd, int fd, uint32_t events)
{
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) < 0) {
        return -1;
    }
    return 0;
}

int SendMessage(int sock_fd, const std::string &message)
{
    uint32_t total_sent = 0;
    uint32_t sending_size = message.size();

    while (total_sent < sending_size) {
        uint32_t bytes_left = sending_size - total_sent;
        int sent = write(sock_fd, message.substr(total_sent).c_str(), bytes_left);
        if (sent <= 0) {
            break;
        }
        total_sent += sent;
    }
    if (sending_size != total_sent) {
        perror("sending size");
        return -1;
    }
    return 0;
}

bool IsNonblocking(int sock_fd)
{
    int check;
    if ((check = fcntl(sock_fd, F_GETFL, 0)) < 0) {
        perror("fcntl");
        return -1;
    }
    return check & O_NONBLOCK;
}

int ReceiveMessageNonblocking(int sock_fd, std::string &message, uint32_t buffer_size)
{
    if (!IsNonblocking(sock_fd)) {
        ErrorLog("IsNonblocking");
        return -1;
    }

    char buffer[buffer_size + 1] = {0};
    message.clear();
    while ((read(sock_fd, buffer, buffer_size)) > 0) {
        message += buffer;
    }
    if (errno != EAGAIN) {
        perror("read:");
        return -1;
    }
    return 0;
}

void ErrorLog(const char *step)
{
    std::cout << "Failed at " << step << ". errno: " << errno << std::endl;
}