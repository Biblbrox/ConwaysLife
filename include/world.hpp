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
#include "ces/basesystem.hpp"
#include "ces/entity.hpp"
#include "render/camera.hpp"
#include "utils/audio.hpp"
#include "ces/cesmanager.hpp"

/**
 * To avoid circular including
 */
//class Entity;
class Component;

using utils::type_id;

typedef boost::multi_array<std::shared_ptr<Entity>, 3> Field;
typedef boost::multi_array<bool, 3> FieldState;
typedef Field::index CellIndex;

class World: public CesManager
{
public:
    World();
    ~World();

    void init() override;
    void update(size_t delta) override;

private:
    utils::Timer m_timer;
    utils::Fps m_fps;

    void update_text();
    void update_field();
    void init_sound();
    void init_text();
    void init_field();

    TTF_Font* open_font(const std::string& font, size_t fontSize);

    /**
     * Remove all entities that not alive
     */
    void filter_entities();

    bool m_scaled;
    const GLfloat m_scaleFactor = 1.5f;
    utils::audio::Audio m_audio;


//    std::vector<std::vector<std::vector<std::shared_ptr<Entity>>>> m_cells;

    Field m_cells;
//    std::array<std::array<std::array<std::shared_ptr<Entity>,
//            mapSize>, mapSize>, mapSize> m_cells;

    bool m_wasInit;
};

#endif //MOONLANDER_WORLD_HPP
