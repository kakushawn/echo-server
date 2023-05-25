#include <cstdint>
#include <string>

#ifndef COMMON_H_
#define COMMON_H_

// Send message chunk by chunk
int SendMessage(int sock_fd, const std::string &message, uint32_t buffer_size);

// Receive message chunk by chunk
int ReceiveMessage(int sock_fd, std::string &message, uint32_t buffer_size);

#endif