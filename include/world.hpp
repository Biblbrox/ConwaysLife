#ifndef MOONLANDER_WORLD_HPP
#define MOONLANDER_WORLD_HPP

#include <unordered_map>
#include <memory>
#include <string>
#include <SDL_ttf.h>

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

inline constexpr size_t mapSize = 6;

class World: public CesManager
{
public:
    World() : m_scaled(false), m_wasInit(false) {};
    ~World();

    void init() override;
    void update(size_t delta) override;

private:
    std::unordered_map<std::string, std::shared_ptr<Entity>> m_nonStatic;
    Camera m_camera;
    GLuint m_screenHeight;
    GLuint m_screenWidth;
    GLfloat m_frameHeight;
    GLfloat m_frameWidth;

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

    std::array<std::array<std::array<std::shared_ptr<Entity>,
            mapSize>, mapSize>, mapSize> m_cells;

    bool m_wasInit;
};

#endif //MOONLANDER_WORLD_HPP
