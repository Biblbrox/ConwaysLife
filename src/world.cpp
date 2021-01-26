#include <memory>
#include <glm/gtc/constants.hpp>
#include <boost/format.hpp>
#include <glm/trigonometric.hpp>

#include "base.hpp"
#include "world.hpp"
#include "components/positioncomponent.hpp"
#include "components/cellcomponent.hpp"
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

const GLfloat cubeSize = 20.f;

void World::update_text()
{
//    if constexpr (debug) {
//        auto fpsEntity = m_entities["fpsText"];
//        auto textFps = fpsEntity->getComponent<TextComponent>();
//        textFps->texture->setText((format("FPS: %+3d") % m_fps.get_fps()).str());
//    }
}

void World::update(size_t delta)
{
    if constexpr (debug)
        m_fps.update();

    update_text();
    GLfloat stepTime = 1.f;
    if (m_timer.getTicks() / 1000.f > stepTime) {
        m_timer.stop();
        m_timer.start();
        update_field();
    }

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
        init_text();
        init_sound();
        init_field();

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

void World::update_field()
{
    std::array<std::array<std::array<bool, mapSize>, mapSize>, mapSize> new_state;
    for (size_t i = 0; i < mapSize; ++i) {
        for (size_t j = 0; j < mapSize; ++j) {
            for (size_t k = 0; k < mapSize; ++k) {
                auto cell = m_cells[i][j][k];
                auto cellComp = cell->getComponent<CellComponent>();
                size_t neirCount = 0;
                bool hasUp =
                        (j != mapSize - 1) ?
                        m_cells[i][j + 1][k]->getComponent<CellComponent>()->alive : false;
                if (hasUp)
                    ++neirCount;
                bool hasDown =
                        (j != 0) ?
                        m_cells[i][j - 1][k]->getComponent<CellComponent>()->alive : false;
                if (hasDown)
                    ++neirCount;
                bool hasLeft =
                        (i != 0) ?
                        m_cells[i - 1][j][k]->getComponent<CellComponent>()->alive : false;
                if (hasLeft)
                    ++neirCount;
                bool hasRight =
                        (i != mapSize - 1) ?
                        m_cells[i + 1][j][k]->getComponent<CellComponent>()->alive : false;
                if (hasRight)
                    ++neirCount;
                bool hasForward =
                        (k != mapSize - 1) ?
                        m_cells[i][j][k + 1]->getComponent<CellComponent>()->alive : false;
                if (hasForward)
                    ++neirCount;
                bool hasBack =
                        (k != 0) ?
                        m_cells[i][j][k - 1]->getComponent<CellComponent>()->alive : false;
                if (hasBack)
                    ++neirCount;
                bool hasUpCor1 =
                        (i != 0 && k != 0 && j != mapSize - 1) ?
                        m_cells[i - 1][j + 1][k - 1]->getComponent<CellComponent>()->alive : false;
                if (hasUpCor1)
                    ++neirCount;
                bool hasUpCor2 =
                        (i != mapSize - 1 && k != 0 && j != mapSize - 1) ?
                        m_cells[i + 1][j + 1][k - 1]->getComponent<CellComponent>()->alive : false;
                if (hasUpCor2)
                    ++neirCount;
                bool hasUpCor3 =
                        (i != mapSize - 1 && k != mapSize - 1 && j != mapSize - 1) ?
                        m_cells[i + 1][j + 1][k + 1]->getComponent<CellComponent>()->alive : false;
                if (hasUpCor3)
                    ++neirCount;
                bool hasUpCor4 =
                        (i != 0 && k != mapSize - 1 && j != mapSize - 1) ?
                        m_cells[i - 1][j + 1][k + 1]->getComponent<CellComponent>()->alive : false;
                if (hasUpCor4)
                    ++neirCount;
                bool hasDownCor1 =
                        (i != 0 && k != 0 && j != 0) ?
                        m_cells[i - 1][j - 1][k - 1]->getComponent<CellComponent>()->alive : false;
                if (hasDownCor1)
                    ++neirCount;
                bool hasDownCor2 =
                        (i != mapSize - 1 && k != 0 && j != 0) ?
                        m_cells[i + 1][j - 1][k - 1]->getComponent<CellComponent>()->alive : false;
                if (hasDownCor2)
                    ++neirCount;
                bool hasDownCor3 =
                        (i != mapSize - 1 && k != mapSize - 1 && j != 0) ?
                        m_cells[i + 1][j - 1][k + 1]->getComponent<CellComponent>()->alive : false;
                if (hasDownCor3)
                    ++neirCount;
                bool hasDownCor4 =
                        (i != 0 && k != mapSize - 1 && j != 0) ?
                        m_cells[i - 1][j - 1][k + 1]->getComponent<CellComponent>()->alive : false;
                if (hasDownCor4)
                    ++neirCount;

                if (!cellComp->alive) {
                    if (neirCount == 9)
                        new_state[i][j][k] = true;

                    continue;
                }
                // Life case
                if (neirCount == 0 || neirCount == 1)
                    new_state[i][j][k] = false;

                if (neirCount >= 4)
                    new_state[i][j][k] = false;
            }
        }
    }

    for (size_t i = 0; i < mapSize; ++i) {
        for (size_t j = 0; j < mapSize; ++j) {
            for (size_t k = 0; k < mapSize; ++k) {
                m_cells[i][j][k]->getComponent<CellComponent>()->alive
                        = new_state[i][j][k];
            }
        }
    }
}

void World::init_text()
{
//    if constexpr (debug) {
//        // Fps entity
//        Entity &fpsText = createEntity("fpsText");
//        fpsText.addComponents<TextComponent, PositionComponent>();
//        fpsText.activate();
//
//        auto fspTexture = fpsText.getComponent<TextComponent>();
//        TTF_Font *font = open_font(msgFont, 14);
//        fspTexture->texture = make_shared<TextTexture>("FPS: 000", font,
//                                                       fontColor);
//
//        auto fpsPos = fpsText.getComponent<PositionComponent>();
//        fpsPos->x = m_screenWidth - m_screenWidth / 4.2f;
//        fpsPos->y = m_screenHeight / 15.f;
//        fpsPos->scallable = false;
//    }
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

void World::init_field()
{
    const std::vector<std::array<size_t, 3>> initial_cells = {
            {1, 3, 3}, {2, 3, 3}, {3, 3, 3}
    };

    for (size_t i = 0; i < mapSize; ++i) {
        for (size_t j = 0; j < mapSize; ++j) {
            for (size_t k = 0; k < mapSize; ++k) {
                auto cell = createEntity("cell" + std::to_string(i)
                                         + std::to_string(j)
                                         + std::to_string(k));
                cell->activate();
                cell->addComponents<SpriteComponent, CellComponent, PositionComponent>();

                auto pos = cell->getComponent<PositionComponent>();
                pos->x = 2.f * i;
                pos->y = 2.f * j;
                pos->z = 2.f * k;

                auto sprite = cell->getComponent<SpriteComponent>();
                sprite->sprite = make_shared<Sprite>();
                sprite->sprite->addTexture(utils::getResourcePath("cube.obj"), cubeSize,
                                           cubeSize, cubeSize);
                sprite->sprite->generateDataBuffer();
                m_cells[i][j][k] = cell;

                auto cellComp = cell->getComponent<CellComponent>();
                if (std::count(initial_cells.cbegin(),
                               initial_cells.cend(), std::array<size_t, 3>{i, j, k}) != 0) {
                    cellComp->alive = true;
                } else {
                    cellComp->alive = false;
                }
            }
        }
    }
}

