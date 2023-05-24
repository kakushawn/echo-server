#ifndef CLIENT_H_
#define CLIENT_H_

class Client
{
public:
    Client();
	// Send message and show echo from server
    void ToServer(const std::string &msg);

private:
    unsigned int port;
	unsigned int buff_size;
	int sockfd;
    void ErrorLog(const char *step);
};

#endif