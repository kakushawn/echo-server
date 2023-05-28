#ifndef SERVER_H_
#define SERVER_H_

#include <cstdint>

class Server
{
public:
    Server(uint32_t port = 9002, uint32_t buffer_size = 16, uint32_t backlog = 10);
    int Init();
    void Run();
    void RunMultiThreaded(int num_workers = 1);
	~Server();

private:
    int sock_fd;
    uint32_t port;
    uint32_t buffer_size;
    uint32_t backlog;
};

#endif
