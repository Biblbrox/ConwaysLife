#ifndef FPS_HPP
#define FPS_HPP

#include <cstddef>
#include <cstring>
#include <SDL_timer.h>

#define FRAMETIMES 10

namespace utils
{
    class Fps
    {
    public:
        Fps()
        {
            std::memset(m_frameTimes, 0, sizeof(m_frameTimes));
            m_lastTicks = SDL_GetTicks();
            m_frameCount = 0;
            m_fps = 0;
        }

        void update() noexcept
        {
            size_t ticks = SDL_GetTicks();
            size_t frame_time = ticks - m_lastTicks;

            m_frameTimes[m_frameCount % FRAMETIMES] = frame_time;
            m_frameCount++;

            m_lastTicks = ticks;

            size_t count;
            if (m_frameCount < FRAMETIMES)
                count = m_frameCount;
            else
                count = FRAMETIMES;

            m_fps = 0;
            for (size_t i = 0; i < count; ++i)
                m_fps += m_frameTimes[i];

            m_fps /= count;
            m_fps = 1000.f / m_fps;
        }

        size_t get_fps() const noexcept
        {
            return m_fps;
        }

    private:
        size_t m_frameTimes[FRAMETIMES];
        size_t m_lastTicks;
        size_t m_frameCount;
        size_t m_fps;
    };
}

#endif //FPS_HPP
