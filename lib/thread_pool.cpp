#include "thread_pool.h"

#include <iostream>

ThreadPool::ThreadPool(uint32_t num_workers) : num_workers(num_workers)
{
    should_terminate = false;
}

void ThreadPool::Start()
{
    workers.resize(num_workers);
    for (uint32_t i = 0; i < num_workers; ++i) {
        workers[i] = std::thread(&ThreadPool::Work, this);
    }
}

void ThreadPool::Push(std::function<void()> job)
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        jobs.push(job);
    }
    mutex_condition.notify_one();
}

void ThreadPool::Work()
{
    std::cout << std::this_thread::get_id() << ": start working" << std::endl;
    while (true) {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            mutex_condition.wait(lock, [this] {
                return should_terminate || !jobs.empty();
            });
            if (should_terminate) {
                break;
            }
            job = jobs.front();
            jobs.pop();
        }
        job();
    }
    std::cout << std::this_thread::get_id() << ": bye" << std::endl;
}

void ThreadPool::Stop()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        should_terminate = true;
    }
    mutex_condition.notify_all();
    for (std::thread &worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    workers.clear();
}