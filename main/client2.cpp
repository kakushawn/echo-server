#include <unistd.h>

#include <ctime>
#include <fstream>
#include <iostream>
#include <istream>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "client.h"
#include "common.h"

int requests = 1, repititions = 10;
std::string result_fname = "result.txt";

std::mutex rlock;
std::fstream result;

std::string gen_random_string(const int len)
{
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    return tmp_s;
}

void run()
{
    Client client(9002, 1024);
    if (client.Init() < 0) {
        std::cout << "Failed to initialize connection." << std::endl;
    }
    std::vector<std::string> echoed_list;
    for (int32_t i = 0; i < repititions; ++i) {
        std::string msg = gen_random_string(3); //std::to_string(i);
        std::string echoed;
        if (client.EchoNonblocking(msg, echoed) < 0) {
            ErrorLog("EchoNonblocking");
        }
        echoed_list.push_back(msg + " " + echoed);
    }

    std::unique_lock<std::mutex> lock(rlock);
    for (const std::string &e : echoed_list) {
        result << e << std::endl;
    }
}

bool validate()
{
    std::cout << "validating..." << std::endl;

    std::string line;
    std::string msg;
    std::string echoed;
    std::ifstream reader(result_fname);
    while (reader >> msg >> echoed) {
        if (msg != echoed) {
            std::cout << "echo mismatched: " << msg << " != " << echoed << std::endl;
			return false;
        }
    }

    reader.close();
    std::cout << "finished." << std::endl;
	return true;
}

int main(int argc, char **argv)
{
    if (argc == 3) {
        requests = atoi(argv[1]);
        repititions = atoi(argv[2]);
    }
    std::vector<std::thread> threads;
    result.open(result_fname, std::ios::app | std::ios::in);
    for (int i = 0; i < requests; ++i) {
        threads.push_back(std::thread(run));
    }
    for (int i = 0; i < requests; ++i) {
        threads[i].join();
    }
    result.close();

    if(!validate()) {
		return -1;
	}

    return 0;
}