#include <cstdint>
#include <string>

#ifndef CLIENT_H_
#define CLIENT_H_

class Client
{
public:
    Client(uint32_t port = 9002, uint32_t buffer_size = 16);

    int Init();

    int Echo(const std::string &msg);

private:
    int sock_fd;
    uint32_t port;
    uint32_t buffer_size;
    void ErrorLog(const char *step);
};

#endif