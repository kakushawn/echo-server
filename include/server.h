#include <cstdint>

#ifndef SERVER_H_
#define SERVER_H_

class Server
{
public:
    Server(uint32_t port = 9002, uint32_t buffer_size = 8, uint32_t backlog = 10);

    int Init();

    void Run();

private:
    int sock_fd;
    uint32_t port;
    uint32_t buffer_size;
    uint32_t backlog;
    uint32_t connection_id;

    // Show log given a step string
    void ErrorLog(const char *step);
};

#endif
