#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include <cstddef>
#include <memory>
#include <future>
#include <functional>
#include <queue>

class ThreadPool
{
public:
    explicit ThreadPool(size_t threadsCount);
    ~ThreadPool();

    void loop();

    void shutdown();

    bool isNoJob() const;

    size_t getThreadsCount() const;

    template <typename F, typename... Args>
    void addJob(F&& f, Args&& ...args)
    {
        {
            std::unique_lock<std::mutex> lock(m_queueMut);
            std::function<void()> func{
                    std::bind(std::forward<F>(f), std::forward<Args>(args)...)};
            m_jobs.push(func);
        }

        m_hasJob.notify_one();
    }

    void waitForFinish();

private:
    size_t m_threadsCount;
    std::queue<std::function<void()>> m_jobs;
    std::vector<std::thread> m_pool;
    std::mutex m_queueMut;
    std::mutex m_poolMut;

    std::condition_variable m_hasJob;
    std::condition_variable m_finished;


    bool m_terminate;
    bool m_stopped;

    int m_busy;
};

#endif //THREADPOOL_HPP
