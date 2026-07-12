/* ThreadPool.h */
#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads);

    ~ThreadPool();

    // takes any type of void function/lambda/functor
    void Submit(std::function<void()> task);
private:
    // loop for each thread
    void WorkerLoop();

    std::vector<std::thread> m_workers; // vector of the threads
    std::queue<std::function<void()>> m_tasks; // queue (FIFO) of tasks
    std::mutex m_mutex; // guards access to m_tasks and m_stop to prevent 
    std::condition_variable m_cv; // sleeps worker threads when no work available, Submit() wakes them up
    bool m_stop; // exit var
};



#endif