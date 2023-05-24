#ifndef COMMON_H_
#define COMMON_H_

#include <string>

int SocketSendLongMessage(int sock_fd, const std::string &message);
int SocketReceiveLongMessage(int sock_fd, std::string &message);

#endif