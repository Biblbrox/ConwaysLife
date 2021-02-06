#include <memory>
#include <boost/format.hpp>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <iostream>

#include "base.hpp"
#include "world.hpp"
#include "config.hpp"
#include "game.hpp"
#include "components/positioncomponent.hpp"
#include "components/cellcomponent.hpp"
#include "components/spritecomponent.hpp"
#include "systems/renderersystem.hpp"
#include "systems/movementsystem.hpp"
#include "components/textcomponent.hpp"
#include "systems/keyboardsystem.hpp"
#include "systems/animationsystem.hpp"
#include "systems/collisionsystem.hpp"
#include "systems/physicssystem.hpp"
#include "systems/particlerendersystem.hpp"
#include "utils/random.hpp"
#include "exceptions/sdlexception.hpp"
#include "exceptions/glexception.hpp"
#include "lifeprogram.hpp"

using utils::log::Logger;
using utils::log::program_log_file_name;
using boost::format;
using utils::physics::altitude;
using std::floor;
using std::vector;
using std::make_shared;
using std::sin;
using std::cos;
using std::find_if;
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

World::World() : m_scaled(false), m_wasInit(false),
                 m_cells(boost::extents[6][6][6])
{
//    Config::addVal("FieldSize", 6, "int");
//    Config::addVal("StepTime", 5.f, "float");
//    Config::addVal("NeirCount", 3, "int");
//    Config::addVal("NeirCountDie", 4, "int");
//    Config::addVal("BackgroundColor", glm::vec4(0.2f, 0.f, 0.2f, 1.f), "vec4");
//    Config::addVal("InverseRotation", false, "bool");
//    Config::addVal("MSAA", false, "bool");
//    Config::addVal("Theme", 0, "int");
}

World::~World()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

void World::update(size_t delta)
{
    if constexpr (debug)
        m_fps.update();

    update_text();
    if (getGameState() == GameStates::PAUSE
        && !m_timer.isPaused()) {
        m_timer.pause();
        std::cout << "Timer paused" << std::endl;
    }

    if (getGameState() == GameStates::STOP
        && getPrevGameState() != GameStates::STOP) {
        setGameState(GameStates::STOP);
        m_timer.stop();
        std::cout << "Timer stopped" << std::endl;
    }

    if (getGameState() == GameStates::PLAY
        && getPrevGameState() == GameStates::PAUSE) {
        setGameState(GameStates::PLAY);
        m_timer.unpause();
        std::cout << "Timer unpaused" << std::endl;
    }

    if (getGameState() == GameStates::PLAY
        && getPrevGameState() == GameStates::STOP) {
        setGameState(GameStates::PLAY);
        m_timer.start();
        // reinit field
        init_field();
        std::cout << "Timer started" << std::endl;
    }

    if (getGameState() == GameStates::PLAY) {
        GLfloat stepTime = Config::getVal<GLfloat>("StepTime");
        if (m_timer.getTicks() / 1000.f > stepTime) {
            m_timer.stop();
            m_timer.start();
            update_field();
            std::cout << "Field updated" << std::endl;
        }
    }

    filter_entities();
    for (auto &system: m_systems)
        system.second->update(delta);
}

void World::init()
{
    if (!m_wasInit) {
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

//    if (m_timer.isStarted() || m_timer.isPaused())
//        m_timer.stop();
//
//    m_timer.start();
}

void World::init_sound()
{

}

void World::update_field()
{
    GLuint fieldSize = Config::getVal<int>("FieldSize");
    FieldState new_state(boost::extents[fieldSize][fieldSize][fieldSize]);
    for (CellIndex i = 0; i < fieldSize; ++i) {
        for (CellIndex j = 0; j < fieldSize; ++j) {
            for (CellIndex k = 0; k < fieldSize; ++k) {
                auto cell = m_cells[i][j][k];
                auto cellComp = cell->getComponent<CellComponent>();
                size_t neirCount = 0;
                bool hasUp =
                        (j != fieldSize - 1) ?
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
                        (i != fieldSize - 1) ?
                        m_cells[i + 1][j][k]->getComponent<CellComponent>()->alive : false;
                if (hasRight)
                    ++neirCount;
                bool hasForward =
                        (k != fieldSize - 1) ?
                        m_cells[i][j][k + 1]->getComponent<CellComponent>()->alive : false;
                if (hasForward)
                    ++neirCount;
                bool hasBack =
                        (k != 0) ?
                        m_cells[i][j][k - 1]->getComponent<CellComponent>()->alive : false;
                if (hasBack)
                    ++neirCount;
                bool hasUpCor1 =
                        (i != 0 && k != 0 && j != fieldSize - 1) ?
                        m_cells[i - 1][j + 1][k - 1]->getComponent<CellComponent>()->alive : false;
                if (hasUpCor1)
                    ++neirCount;
                bool hasUpCor2 =
                        (i != fieldSize - 1 && k != 0 && j != fieldSize - 1) ?
                        m_cells[i + 1][j + 1][k - 1]->getComponent<CellComponent>()->alive : false;
                if (hasUpCor2)
                    ++neirCount;
                bool hasUpCor3 =
                        (i != fieldSize - 1 && k != fieldSize - 1 && j != fieldSize - 1) ?
                        m_cells[i + 1][j + 1][k + 1]->getComponent<CellComponent>()->alive : false;
                if (hasUpCor3)
                    ++neirCount;
                bool hasUpCor4 =
                        (i != 0 && k != fieldSize - 1 && j != fieldSize - 1) ?
                        m_cells[i - 1][j + 1][k + 1]->getComponent<CellComponent>()->alive : false;
                if (hasUpCor4)
                    ++neirCount;
                bool hasDownCor1 =
                        (i != 0 && k != 0 && j != 0) ?
                        m_cells[i - 1][j - 1][k - 1]->getComponent<CellComponent>()->alive : false;
                if (hasDownCor1)
                    ++neirCount;
                bool hasDownCor2 =
                        (i != fieldSize - 1 && k != 0 && j != 0) ?
                        m_cells[i + 1][j - 1][k - 1]->getComponent<CellComponent>()->alive : false;
                if (hasDownCor2)
                    ++neirCount;
                bool hasDownCor3 =
                        (i != fieldSize - 1 && k != fieldSize - 1 && j != 0) ?
                        m_cells[i + 1][j - 1][k + 1]->getComponent<CellComponent>()->alive : false;
                if (hasDownCor3)
                    ++neirCount;
                bool hasDownCor4 =
                        (i != 0 && k != fieldSize - 1 && j != 0) ?
                        m_cells[i - 1][j - 1][k + 1]->getComponent<CellComponent>()->alive : false;
                if (hasDownCor4)
                    ++neirCount;

                if (!cellComp->alive) {
                    if (neirCount == Config::getVal<int>("NeirCount"))
                        new_state[i][j][k] = true;

                    continue;
                }
                // Life case
                if (neirCount == 0 || neirCount == 1)
                    new_state[i][j][k] = false;

                if (neirCount >= Config::getVal<int>("NeirCountDie"))
                    new_state[i][j][k] = false;
            }
        }
    }

    for (size_t i = 0; i < fieldSize; ++i) {
        for (size_t j = 0; j < fieldSize; ++j) {
            for (size_t k = 0; k < fieldSize; ++k) {
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
    GLuint fieldSize = Config::getVal<int>("FieldSize");

    const std::vector<std::array<size_t, 3>> initial_cells = {
            {0, 0, 0},
            {1, 0, 0},
            {2, 0, 0},
            {3, 0, 0},
            {0, 1, 0},
            {0, 2, 0},
            {0, 3, 0},
            {0, 0, 1},
            {0, 0, 2},
            {0, 0, 3},
    };

    GLfloat init_x = 0.f;
    GLfloat init_y = 0.f;
    GLfloat init_z = 0.f;

    m_cells.resize(boost::extents[0][0][0]); // clear array if reinit
    m_cells.resize(boost::extents[fieldSize][fieldSize][fieldSize]);

    for (CellIndex i = 0; i < fieldSize; ++i) {
        for (CellIndex j = 0; j < fieldSize; ++j) {
            for (CellIndex k = 0; k < fieldSize; ++k) {
                auto cell = createEntity("cell" + std::to_string(i)
                                         + std::to_string(j)
                                         + std::to_string(k));
                cell->activate();
                cell->addComponents<SpriteComponent, CellComponent, PositionComponent>();

                auto pos = cell->getComponent<PositionComponent>();
                pos->x = init_x + cubeSize * i;
                pos->y = init_y + cubeSize * j;
                pos->z = init_z + cubeSize * k;

                auto sprite = cell->getComponent<SpriteComponent>();
                sprite->sprite = make_shared<Sprite>();
                sprite->sprite->addTexture(getResourcePath("cube.obj"), cubeSize,
                                           cubeSize, cubeSize);
                sprite->sprite->generateDataBuffer();
                m_cells[i][j][k] = cell;

                auto cellComp = cell->getComponent<CellComponent>();
                if (std::count(initial_cells.cbegin(),
                               initial_cells.cend(),
                               std::array<size_t, 3>{(size_t)i, (size_t)j, (size_t)k}) != 0) {
                    cellComp->alive = true;
                } else {
                    cellComp->alive = false;
                }
            }
        }
    }

    // TODO: fix bug
    auto camera = Camera::getInstance();
    GLfloat pos = fieldSize * (cubeSize + 40);
    camera->setPos({pos, pos, pos});
    camera->updateView();
    auto program = LifeProgram::getInstance();
    program->setView(camera->getView());
    program->updateView();
}
