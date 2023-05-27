#ifndef THREAD_POOL_
#define THREAD_POOL_

#include <condition_variable>
#include <cstdint>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>
#include <cstdint>

class ThreadPool
{
public:
    ThreadPool(uint32_t num_workers, std::function<void(int, uint32_t)> runner);
    void Start();
    void Push(std::pair<int, uint32_t> task);
    bool Busy();
    void Stop();

private:
    // infine loop
    void Work();
    uint32_t num_workers;
    bool should_terminate = false;
    std::vector<std::thread> workers;
    std::queue<std::pair<int, uint32_t>> tasks;
    std::function<void(int, uint32_t)> runner;
    std::mutex queue_mutex;
    std::condition_variable mutex_condition;
};
#endif