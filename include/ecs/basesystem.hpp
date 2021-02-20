#ifndef BASESYSTEM_HPP
#define BASESYSTEM_HPP

namespace ecs
{
    class EcsManager;

    /**
     * System base class
     */
    class BaseSystem
    {
    public:
        BaseSystem() : m_stopped(false), m_ecsManager(nullptr)
        {};

        virtual ~BaseSystem() = default;

        virtual void setEcsManager(EcsManager *ecs) final
        {
            m_ecsManager = ecs;
        }

        virtual void update(size_t delta) final
        {
            if (!m_stopped)
                update_state(delta);
        }

        /**
         * Deactivate system
         */
        virtual void stop() final
        {
            m_stopped = true;
        }

        /**
         * Activate system
         */
        virtual void start() final
        {
            m_stopped = false;
        }

    protected:
        virtual void update_state(size_t delta) = 0;

        EcsManager *m_ecsManager;
        bool m_stopped;
    };
};

#endif //BASESYSTEM_HPP
