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
    glm::vec3 color;
    char neirCount;
};

typedef boost::multi_array<CellComponent, 3> Field;
typedef boost::multi_array<CellComponent, 3> FieldState;
typedef Field::index CellIndex;

class World: public ecs::EcsManager
{
public:
    World();
    ~World();

    void init() override;
    void update(size_t delta) override;

    static Field m_cells;
private:
    utils::Timer m_timer;
    utils::Fps m_fps;

    void update_field();
    void update_field_omp();
    void init_field();

    /**
     * Remove all entities that not alive
     */
    void filter_entities();

    size_t m_fieldSize;

    Field m_newState;
    ThreadPool m_pool;

    bool m_wasInit;
};

#endif //MOONLANDER_WORLD_HPP
