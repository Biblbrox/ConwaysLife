#ifndef BASESYSTEM_HPP
#define BASESYSTEM_HPP

class CesManager;

/**
 * System base class
 */
class BaseSystem
{
public:
    BaseSystem(): m_stopped(false), m_cesManager(nullptr) {};
    virtual ~BaseSystem() = default;

    virtual void setCesManager(CesManager* cesMan) final
    {
        m_cesManager = cesMan;
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

    CesManager* m_cesManager;
    bool m_stopped;
};

#endif //BASESYSTEM_HPP
