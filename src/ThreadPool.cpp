/* ThreadPool.cpp */
#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t numThreads) {
    m_stop = false;

    for(size_t i = 0; i < numThreads; ++i) {
        m_workers.push_back(std::thread(&ThreadPool::WorkerLoop, this));
    }
}

ThreadPool::~ThreadPool() {
    // scope out to destruct lock_guard before notifying all threads - joining the threads doesnt need the lock_guard
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_stop = true;
    }

    m_cv.notify_all();
    for(auto& thread : m_workers) {
        thread.join();
    }
}

void ThreadPool::Submit(std::function<void()> task) {
    // scope the lock_guard again so it destructs and unlocks
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_tasks.push(std::move(task)); // (technically a pushback to the end of the queue)
    }
    m_cv.notify_one();
}

// PRIVATE
void ThreadPool::WorkerLoop() {
        while (true) {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_cv.wait(lock, [this]{ return m_stop || !m_tasks.empty(); });
            // wait and lock the mutex if m_stop is true or the task queue isn't empty

            if(m_stop && m_tasks.empty()) {
                return; // terminate the thread if told to stop
            }

            // task must be in the queue if the thread got past this check
            // get a task and then pop it off the stack - must use std::move()
            std::function<void()> task = std::move(m_tasks.front());
            m_tasks.pop();
            
            // unlock the mutex then execute the task
            lock.unlock();
            task();
        }
    }