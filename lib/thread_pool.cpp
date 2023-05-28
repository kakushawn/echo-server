#include "thread_pool.h"

#include <iostream>

ThreadPool::ThreadPool(uint32_t num_workers, std::function<void(int, uint32_t)> runner)
    : num_workers(num_workers),
      runner(runner)
{
    should_terminate = false;
}

void ThreadPool::Start()
{
    workers.resize(num_workers);
    for (uint32_t i = 0; i < num_workers; ++i) {
        workers[i] = std::thread(&ThreadPool::Work, this);
    }
    std::cout << "Created " << num_workers << " workers in pool." << std::endl;
}

void ThreadPool::Push(std::pair<int, uint32_t> task)
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        tasks.push(task);
    }
    mutex_condition.notify_one();
}

void ThreadPool::Work()
{
    std::cout << "start working\n";
    while (true) {
        std::pair<int, uint32_t> task;
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            mutex_condition.wait(lock, [this] {
                return should_terminate || !tasks.empty();
            });
            if (should_terminate) {
                break;
            }
            task = tasks.front();
            tasks.pop();
        }
        runner(task.first, task.second);
    }
    std::cout << "bye\n";
}

bool ThreadPool::Busy()
{
    return false;
}

void ThreadPool::Stop()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        should_terminate = true;
    }
    mutex_condition.notify_all();
    for (std::thread &worker : workers) {
        worker.join();
    }
    workers.clear();
}