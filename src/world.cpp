#include <memory>
#include <glm/gtc/constants.hpp>
#include <boost/format.hpp>
#include <glm/trigonometric.hpp>

#include "base.hpp"
#include "world.hpp"
#include "components/positioncomponent.hpp"
#include "components/spritecomponent.hpp"
#include "components/velocitycomponent.hpp"
#include "components/levelcomponent.hpp"
#include "systems/renderersystem.hpp"
#include "systems/movementsystem.hpp"
#include "components/textcomponent.hpp"
#include "systems/keyboardsystem.hpp"
#include "systems/animationsystem.hpp"
#include "systems/collisionsystem.hpp"
#include "systems/physicssystem.hpp"
#include "systems/particlerendersystem.hpp"
#include "utils/random.hpp"
#include "components/lifetimecomponent.hpp"
#include "game.hpp"
#include "exceptions/sdlexception.hpp"

using utils::log::Logger;
using utils::getResourcePath;
using utils::log::program_log_file_name;
using boost::format;
using utils::physics::altitude;
using std::floor;
using std::vector;
using std::make_shared;
using std::sin;
using std::cos;
using glm::half_pi;
using std::find_if;
using glm::pi;
using utils::fix_coords;


const char* const msgFont = "kenvector_future2.ttf";
const SDL_Color fontColor = {0xFF, 0xFF, 0xFF, 0xFF};

void World::update_level()
{

}

void World::update_text()
{
    if constexpr (debug) {
        auto fpsEntity = m_entities["fpsText"];
        auto textFps = fpsEntity->getComponent<TextComponent>();
        textFps->texture->setText((format("FPS: %+3d") % m_fps.get_fps()).str());
    }
}

void World::update(size_t delta)
{
    if constexpr (debug)
        m_fps.update();

    if (getGameState() == GameStates::WIN
        && getPrevGameState() != GameStates::WIN) {
        m_systems[type_id<MovementSystem>()]->stop();
        m_entities["ship"]->removeComponent<KeyboardComponent>();
    }

    update_text();
    update_level();
    update_sprites();

    filter_entities();
    for (auto &system: m_systems)
        system.second->update(delta);
}

void World::init()
{
    if (!m_wasInit) {
        m_screenWidth = utils::getScreenWidth<GLuint>();
        m_screenHeight = utils::getScreenHeight<GLuint>();
        m_frameWidth = m_screenWidth;
        m_frameHeight = m_screenHeight;

        createSystem<RendererSystem>();
        createSystem<MovementSystem>();
        createSystem<KeyboardSystem>();
        createSystem<AnimationSystem>();
        createSystem<CollisionSystem>();
        createSystem<PhysicsSystem>();
        createSystem<ParticleRenderSystem>();

        // Order of initialization is matter
        init_level();
        init_sprites();
        init_text();
        init_sound();

        m_wasInit = true;
    } else {

    }

    if (m_timer.isStarted() || m_timer.isPaused())
        m_timer.stop();

    m_timer.start();
}

void World::init_sound()
{

}

void World::init_sprites()
{
    const int SHIP_WIDTH = 20;
    const int SHIP_HEIGHT = 21;

    using namespace utils::physics;
    // Ship entity
    Entity &ship = createEntity("ship");
    ship.addComponents<PositionComponent, SpriteComponent, LifeTimeComponent>();
    ship.activate();

    auto shipSprite = ship.getComponent<SpriteComponent>();
    shipSprite->sprite = make_shared<Sprite>(
            utils::getResourcePath("lunar_lander_bw.png"));
    shipSprite->sprite->addClipSprite({0, 32, SHIP_WIDTH, SHIP_HEIGHT});
    shipSprite->sprite->addClipSprite({20, 32, SHIP_WIDTH, SHIP_HEIGHT});
    shipSprite->sprite->addClipSprite({40, 32, SHIP_WIDTH, SHIP_HEIGHT});
    shipSprite->sprite->generateDataBuffer();

    auto shipPos = ship.getComponent<PositionComponent>();
    shipPos->x = 0.f;
    shipPos->y = 0.f;
    shipPos->angle = glm::pi<GLfloat>();

    auto fuel = ship.getComponent<LifeTimeComponent>();
    fuel->time = 1500;

//    auto shipVel = ship.getComponent<VelocityComponent>();
//    shipVel->x = 2.f;
//    auto shipAnim = ship.getComponent<AnimationComponent>();
//
//    auto keyboardComponent = ship.getComponent<KeyboardComponent>();
//    keyboardComponent->event_handler = [shipVel, shipPos, shipAnim, fuel, this]
//            (const Uint8 *state) {
//        if (state[SDL_SCANCODE_UP] && fuel->time > 0) {
//            shipVel->y += -engine_force / weight *
//                          sin(shipPos->angle + half_pi<GLfloat>());
//            shipVel->x += -engine_force / weight *
//                          cos(shipPos->angle + half_pi<GLfloat>());
//            shipAnim->cur_state = (SDL_GetTicks() / 100) % 2 + 1;
//            fuel->time -= 1;
//        } else {
//            shipAnim->cur_state = 0;
//        }
//
//        if (state[SDL_SCANCODE_LEFT])
//            shipVel->angle -= rot_step;
//
//        if (state[SDL_SCANCODE_RIGHT])
//            shipVel->angle += rot_step;
//    };
}

void World::init_text()
{
    if constexpr (debug) {
        // Fps entity
        Entity &fpsText = createEntity("fpsText");
        fpsText.addComponents<TextComponent, PositionComponent>();
        fpsText.activate();

        auto fspTexture = fpsText.getComponent<TextComponent>();
        TTF_Font *font = open_font(msgFont, 14);
        fspTexture->texture = make_shared<TextTexture>("FPS: 000", font,
                                                       fontColor);

        auto fpsPos = fpsText.getComponent<PositionComponent>();
        fpsPos->x = m_screenWidth - m_screenWidth / 4.2f;
        fpsPos->y = m_screenHeight / 15.f;
        fpsPos->scallable = false;
    }
}

void World::init_level()
{

}

void World::filter_entities()
{
    for (auto it = m_entities.begin(); it != m_entities.end();)
        it = !it->second->isActivate() ? m_entities.erase(it) : ++it;
}

TTF_Font* World::open_font(const std::string& fontName, size_t fontSize)
{
    TTF_Font* font = TTF_OpenFont(getResourcePath(fontName).c_str(), fontSize);
    if (!font)
        throw SdlException((format("Unable to load font %s\nError: %s\n")
                            % fontName % TTF_GetError()).str(),
                           program_log_file_name(), Category::INTERNAL_ERROR);

    return font;
}

void World::update_sprites()
{
    using utils::physics::altitude;
    using utils::Position;

    auto ship = m_entities["ship"];
//    auto shipPos = ship->getComponent<PositionComponent>();
//    auto shipVel = ship->getComponent<VelocityComponent>();

    auto renderSystem = std::dynamic_pointer_cast<RendererSystem>(
            m_systems[type_id<RendererSystem>()]
    );

//    if ((shipPos->x >= m_frameWidth - m_frameWidth / 4.f)
//        || (shipPos->x < m_frameWidth / 4.f)) { // Horizontal edges
//        m_camera.translate(shipVel->x, 0.f);
//    }

//    if ((shipPos->y >= m_frameHeight - m_frameHeight / 4.f)
//        || (shipPos->y < m_frameHeight / 4.f)) { // Vertical edges
//        m_camera.translate(0.f, shipVel->y);
//    }
}

