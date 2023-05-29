#ifndef COMMON_H_
#define COMMON_H_

#include <cstdint>
#include <string>

// Send message chunk by chunk
int SendMessage(int sock_fd, const std::string &message, uint32_t buffer_size);

// Receive message chunk by chunk
int ReceiveMessage(int sock_fd, std::string &message, uint32_t buffer_size);

int SetNonblocking(int sock_fd);

int EpollCtlAdd(int epfd, int fd, uint32_t events);

// Send all message in one go. 
int SendMessage2(int sock_fd, const std::string &message);

// Check if fd is non-blocking.
bool IsNonblocking(int sock_fd);

// Nonblocking receive. sock_fd should be nonblocking
int ReceiveMessageNonblocking(int sock_fd, std::string &message, uint32_t buffer_size);

void ErrorLog(const char *step);

#endif