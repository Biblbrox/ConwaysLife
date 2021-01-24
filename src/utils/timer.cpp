#include <SDL_timer.h>

#include "utils/timer.hpp"

utils::Timer::Timer() noexcept : m_startTicks(0), m_pausedTicks(0),
                        m_paused(false), m_started(false)
{

}

void utils::Timer::start() noexcept
{
    m_started = true;
    m_paused = false;

    m_startTicks = SDL_GetTicks();
    m_pausedTicks = 0;
}

void utils::Timer::stop() noexcept
{
    m_started = false;
    m_paused = false;

    m_startTicks = 0;
    m_pausedTicks = 0;
}

void utils::Timer::pause() noexcept
{
    if (m_started && !m_paused) {
        m_paused = true;

        m_pausedTicks = SDL_GetTicks() - m_startTicks;
        m_startTicks = 0;
    }
}

void utils::Timer::unpause() noexcept
{
    if (m_started && m_paused) {
        m_paused = false;

        m_startTicks = SDL_GetTicks() - m_pausedTicks;
    }
}

uint32_t utils::Timer::getTicks() const noexcept
{
    Uint32 time = 0;

    if (m_started)
        time = m_paused ? m_pausedTicks : (SDL_GetTicks() - m_startTicks);

    return time;
}

bool utils::Timer::isStarted() const noexcept
{
    return m_started;
}

bool utils::Timer::isPaused() const noexcept
{
    return m_paused && m_started;
}
