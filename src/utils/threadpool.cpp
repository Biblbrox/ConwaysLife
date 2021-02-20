#include <iostream>

#include "utils/threadpool.hpp"

ThreadPool::ThreadPool(size_t threadsCount) : m_threadsCount(threadsCount),
                                              m_busy(0),
                                              m_terminate(false),
                                              m_stopped(false)
{
    m_pool.reserve(m_threadsCount);
    for (size_t i = 0; i < m_threadsCount; ++i)
        m_pool.emplace_back(&ThreadPool::loop, this);
}


ThreadPool::~ThreadPool()
{
    if (!m_stopped)
        shutdown();
}

void ThreadPool::loop()
{
    while (true) {
        std::unique_lock<std::mutex> lock(m_queueMut);

        m_hasJob.wait(lock, [this]{
            return !m_jobs.empty() || m_terminate;
        });
        if (!m_jobs.empty()) { // m_terminate case
            ++m_busy;
            auto job = m_jobs.front();
            m_jobs.pop();

            lock.unlock();
            job();
            lock.lock();
            --m_busy;

            m_finished.notify_one();
        } else {
            break;
        }
    }
}

void ThreadPool::shutdown()
{
    {
        std::unique_lock<std::mutex> lock(m_poolMut);
        m_terminate = true;
    }

    m_hasJob.notify_all();

    for (auto& thr : m_pool)
        if (thr.joinable())
            thr.join();

    m_pool.clear();
    m_stopped = true;
}

size_t ThreadPool::getThreadsCount() const
{
    return m_pool.size();
}

bool ThreadPool::isNoJob() const
{
    return m_jobs.empty();
}

void ThreadPool::waitForFinish()
{
    std::unique_lock<std::mutex> lock(m_queueMut);
    m_finished.wait(lock, [this](){ return m_jobs.empty() && m_busy == 0; });
}


