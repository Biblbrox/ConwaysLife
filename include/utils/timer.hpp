#ifndef TIMER_HPP
#define TIMER_HPP

namespace utils
{
    class Timer
    {
    public:
        Timer() noexcept;

        void start() noexcept;
        void stop() noexcept;
        void pause() noexcept;
        void unpause() noexcept;

        uint32_t getTicks() const noexcept;

        bool isStarted() const noexcept;
        bool isPaused() const noexcept;

    private:
        uint32_t m_startTicks;
        uint32_t m_pausedTicks;

        bool m_paused;
        bool m_started;
    };
}


#endif //TIMER_HPP
