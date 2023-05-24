#include <string>

#ifndef CLIENT_H_
#define CLIENT_H_

class Client
{
public:
    Client(unsigned int port=9002);
	int Init();
    // Send message and show echo from server
    int Echo(const std::string &msg);

private:
    unsigned int port;
    int sock_fd;
    void ErrorLog(const char *step);
};

#endif