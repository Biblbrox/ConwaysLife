#ifndef MOONLANDER_WORLD_HPP
#define MOONLANDER_WORLD_HPP

#include <unordered_map>
#include <memory>
#include <string>
#include <SDL_ttf.h>
#include <boost/multi_array.hpp>

#include "utils/fps.hpp"
#include "utils/timer.hpp"
#include "utils/utils.hpp"
#include "ecs/basesystem.hpp"
#include "ecs/entity.hpp"
#include "render/camera.hpp"
#include "utils/audio.hpp"
#include "ecs/ecsmanager.hpp"
#include "utils/threadpool.hpp"
#include "components/cellcomponent.hpp"

/**
 * To avoid circular including
 */
class Component;

struct CellState
{
    bool alive;
    glm::vec4 color;
};

typedef boost::multi_array<std::shared_ptr<CellComponent>, 3> Field;
typedef boost::multi_array<CellState, 3> FieldState;
typedef Field::index CellIndex;

class World: public ecs::EcsManager
{
public:
    World();
    ~World();

    void init() override;
    void update(size_t delta) override;

private:
    utils::Timer m_timer;
    utils::Fps m_fps;

    void update_field();
    void init_field();

    /**
     * Remove all entities that not alive
     */
    void filter_entities();

    Field m_cells;
    size_t m_fieldSize;

    ThreadPool m_pool;

    bool m_wasInit;
};

#endif //MOONLANDER_WORLD_HPP
