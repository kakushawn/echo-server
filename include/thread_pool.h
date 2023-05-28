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
    ThreadPool(uint32_t num_workers);
    void Start();
    void Push(std::function<void()> job);
    void Stop();

private:
    uint32_t num_workers;
    bool should_terminate = false;
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> jobs;
    std::mutex queue_mutex;
    std::condition_variable mutex_condition;
    // infine loop
    void Work();
};
#endif