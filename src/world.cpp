#include <memory>
#include <boost/format.hpp>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <thread>

#include "base.hpp"
#include "world.hpp"
#include "config.hpp"
#include "game.hpp"
#include "utils/math.hpp"
#include "components/positioncomponent.hpp"
#include "components/cellcomponent.hpp"
#include "components/spritecomponent.hpp"
#include "systems/renderersystem.hpp"
#include "components/textcomponent.hpp"
#include "systems/keyboardsystem.hpp"
#include "systems/animationsystem.hpp"
#include "systems/physicssystem.hpp"
#include "systems/particlerendersystem.hpp"
#include "utils/random.hpp"
#include "exceptions/sdlexception.hpp"
#include "exceptions/glexception.hpp"
#include "lifeprogram.hpp"

using utils::log::Logger;
using utils::log::program_log_file_name;
using boost::format;
using std::floor;
using std::vector;
using std::make_shared;
using std::sin;
using std::cos;
using std::find_if;
using utils::fix_coords;

const GLfloat cubeSize = 20.f;

World::World() : m_scaled(false), m_wasInit(false),
                 m_cells(boost::extents[6][6][6]),
                 m_pool(get_thread_count())
{
    if (!Config::hasKey("FieldSize"))
        Config::addVal("FieldSize", 6, "int");
    if (!Config::hasKey("StepTime"))
        Config::addVal("StepTime", 5.f, "float");
    if (!Config::hasKey("NeirCount"))
        Config::addVal("NeirCount", 3, "int");
    if (!Config::hasKey("NeirCountDie"))
        Config::addVal("NeirCountDie", 4, "int");
    if (!Config::hasKey("BackgroundColor"))
        Config::addVal("BackgroundColor", glm::vec4(0.2f, 0.f, 0.2f, 1.f), "vec4");
    if (!Config::hasKey("InverseRotation"))
        Config::addVal("InverseRotation", false, "bool");
    if (!Config::hasKey("MSAA"))
        Config::addVal("MSAA", false, "bool");
    if (!Config::hasKey("MSAASamples"))
        Config::addVal("MSAASamples", 4, "int");
    if (!Config::hasKey("Theme"))
        Config::addVal("Theme", 0, "int");
    if (!Config::hasKey("CellColor"))
        Config::addVal("CellColor", glm::vec4(1.f, 0.2f, 0.f, 1.f), "vec4");
    if (!Config::hasKey("CellBorderColor"))
        Config::addVal("CellBorderColor", glm::vec4(1.f, 1.f, 1.f, 1.f), "vec4");
    if (!Config::hasKey("ColoredLife"))
        Config::addVal("ColoredLife", false, "bool");
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
        init();
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

//    filter_entities();
    for (auto &system: m_systems)
        system.second->update(delta);
}

void World::init()
{
    m_systems.clear();
    createSystem<KeyboardSystem>();
    createSystem<RendererSystem>();
    createSystem<AnimationSystem>();
    createSystem<PhysicsSystem>();
    createSystem<ParticleRenderSystem>();

    m_fieldSize = Config::getVal<int>("FieldSize");
    init_field();

    m_wasInit = true;
}

void World::update_field()
{
    FieldState new_state(boost::extents[m_fieldSize][m_fieldSize][m_fieldSize]);

    size_t neirCountToDie = Config::getVal<int>("NeirCountDie");
    size_t neirCountToLife = Config::getVal<int>("NeirCount");
    auto func = [this, &new_state, neirCountToDie, neirCountToLife](int start, int end){
        using namespace utils::math;

        for (CellIndex i = start; i < end; ++i) {
            for (CellIndex j = start; j < end; ++j) {
                for (CellIndex k = start; k < end; ++k) {
                    const auto& cell = m_cells[i][j][k];
                    size_t neirCount = 0;
                    glm::vec4 color = {0, 0, 0, 0};
                    bool hasUp =
                            (j != end - 1) ?
                            m_cells[i][j + 1][k]->alive : false;
                    if (hasUp) {
                        ++neirCount;
                        color += m_cells[i][j + 1][k]->color;
                    }

                    bool hasDown =
                            (j != 0) ?
                            m_cells[i][j - 1][k]->alive : false;
                    if (hasDown) {
                        ++neirCount;
                        color += m_cells[i][j - 1][k]->color;
                    }
                    bool hasLeft =
                            (i != 0) ?
                            m_cells[i - 1][j][k]->alive : false;
                    if (hasLeft) {
                        color += m_cells[i - 1][j][k]->color;
                        ++neirCount;
                    }
                    bool hasRight =
                            (i != end - 1) ?
                            m_cells[i + 1][j][k]->alive : false;
                    if (hasRight) {
                        color += m_cells[i + 1][j][k]->color;
                        ++neirCount;
                    }
                    bool hasForward =
                            (k != end - 1) ?
                            m_cells[i][j][k + 1]->alive : false;
                    if (hasForward) {
                        color += m_cells[i][j][k + 1]->color;
                        ++neirCount;
                    }
                    bool hasBack =
                            (k != 0) ?
                            m_cells[i][j][k - 1]->alive : false;
                    if (hasBack) {
                        color += m_cells[i][j][k - 1]->color;
                        ++neirCount;
                    }
                    bool hasUpCor1 =
                            (i != 0 && k != 0 && j != end - 1) ?
                            m_cells[i - 1][j + 1][k - 1]->alive : false;
                    if (hasUpCor1) {
                        color += m_cells[i - 1][j + 1][k - 1]->color;
                        ++neirCount;
                    }
                    bool hasUpCor2 =
                            (i != end - 1 && k != 0 && j != end - 1) ?
                            m_cells[i + 1][j + 1][k - 1]->alive : false;
                    if (hasUpCor2) {
                        color += m_cells[i + 1][j + 1][k - 1]->color;
                        ++neirCount;
                    }
                    bool hasUpCor3 =
                            (i != end - 1 && k != end - 1 && j != end - 1) ?
                            m_cells[i + 1][j + 1][k + 1]->alive : false;
                    if (hasUpCor3) {
                        color += m_cells[i + 1][j + 1][k + 1]->color;
                        ++neirCount;
                    }
                    bool hasUpCor4 =
                            (i != 0 && k != end - 1 && j != end - 1) ?
                            m_cells[i - 1][j + 1][k + 1]->alive : false;
                    if (hasUpCor4) {
                        color += m_cells[i - 1][j + 1][k + 1]->color;
                        ++neirCount;
                    }
                    bool hasDownCor1 =
                            (i != 0 && k != 0 && j != 0) ?
                            m_cells[i - 1][j - 1][k - 1]->alive : false;
                    if (hasDownCor1) {
                        color += m_cells[i - 1][j - 1][k - 1]->color;
                        ++neirCount;
                    }
                    bool hasDownCor2 =
                            (i != end - 1 && k != 0 && j != 0) ?
                            m_cells[i + 1][j - 1][k - 1]->alive : false;
                    if (hasDownCor2) {
                        color += m_cells[i + 1][j - 1][k - 1]->color;
                        ++neirCount;
                    }
                    bool hasDownCor3 =
                            (i != end - 1 && k != end - 1 && j != 0) ?
                            m_cells[i + 1][j - 1][k + 1]->alive : false;
                    if (hasDownCor3) {
                        color += m_cells[i + 1][j - 1][k + 1]->color;
                        ++neirCount;
                    }
                    bool hasDownCor4 =
                            (i != 0 && k != end - 1 && j != 0) ?
                            m_cells[i - 1][j - 1][k + 1]->alive : false;
                    if (hasDownCor4) {
                        color += m_cells[i - 1][j - 1][k + 1]->color;
                        ++neirCount;
                    }

                    // Dead case
                    if (!cell->alive) {
                        if (neirCount >= neirCountToLife) {
                            new_state[i][j][k].alive = true;
                            new_state[i][j][k].color = color / neirCount;
                        }

                        continue;
                    }

                    // Life case
                    if (neirCount < neirCountToLife
                        || neirCount >= neirCountToDie)
                        new_state[i][j][k].alive = false;
                }
            }
        }
    };

    int threadCount = m_pool.getThreadsCount();
    int partSize = threadCount / m_fieldSize;
    int rem = threadCount % m_fieldSize;
    size_t i;
    for (i = 0; i < threadCount; ++i) {
        if (i == threadCount - 1 && rem != 0)
            m_pool.addJob(func, i * partSize, m_fieldSize);
        else
            m_pool.addJob(func, i * partSize, (i + 1) * partSize);
    }

    m_pool.waitForFinish();

    for (i = 0; i < m_fieldSize; ++i) {
        for (size_t j = 0; j < m_fieldSize; ++j) {
            for (size_t k = 0; k < m_fieldSize; ++k) {
                m_cells[i][j][k]->alive = new_state[i][j][k].alive;
                m_cells[i][j][k]->color = new_state[i][j][k].color;
            }
        }
    }
}

void World::filter_entities()
{
    for (auto it = m_entities.begin(); it != m_entities.end();)
        it = !it->second->isActivate() ? m_entities.erase(it) : ++it;
}

void World::init_field()
{
    using utils::math::cantor_pairing;

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
            {0, 0, 5},

            {4, 0, 0},
            {6, 0, 0},
            {8, 0, 0},
            {10, 0, 0},
            {12, 1, 0},
            {14, 2, 0},
            {16, 3, 0},
            {18, 0, 1},
            {20, 0, 2},
            {13, 10, 3},
            {14, 10, 5},
            {4, 1, 0},
            {6, 1, 0},
            {8, 1, 0},
            {10, 1, 0},
            {12, 2, 0},
            {14, 3, 0},
            {16, 4, 0},
            {18, 1, 1},
            {20, 1, 2},
            {13, 11, 3},
            {14, 11, 5},
    };

    GLfloat init_x = 0.f;
    GLfloat init_y = 0.f;
    GLfloat init_z = 0.f;

    m_cells.resize(boost::extents[0][0][0]); // clear array if reinit
    m_cells.resize(boost::extents[m_fieldSize][m_fieldSize][m_fieldSize]);

    std::shared_ptr<Sprite> sprite_com = std::make_shared<Sprite>();
    sprite_com->addTexture(getResourcePath("cube.obj"), cubeSize,
                               cubeSize, cubeSize);
    sprite_com->generateDataBuffer();
    glm::vec4 color = Config::getVal<glm::vec4>("CellColor");
    for (CellIndex i = 0; i < m_fieldSize; ++i) {
        for (CellIndex j = 0; j < m_fieldSize; ++j) {
            for (CellIndex k = 0; k < m_fieldSize; ++k) {
                utils::Random rand;
                auto cell = createEntity(cantor_pairing(i, j, k));
                cell->activate();
                cell->addComponents<SpriteComponent, CellComponent, PositionComponent>();

                auto pos = cell->getComponent<PositionComponent>();
                pos->x = init_x + cubeSize * i;
                pos->y = init_y + cubeSize * j;
                pos->z = init_z + cubeSize * k;

                auto sprite = cell->getComponent<SpriteComponent>();
                sprite->sprite = sprite_com;

                auto cellComp = cell->getComponent<CellComponent>();
                cellComp->color =
                        {rand.generateu<GLfloat>(0.f, 1.f),
                         rand.generateu<GLfloat>(0.f, 1.f),
                         rand.generateu<GLfloat>(0.f, 1.f),
                         1.f};
                std::cout << cellComp->color.x
                          << cellComp->color.y
                          << cellComp->color.z << "\n";
                if (std::count(initial_cells.cbegin(),
                               initial_cells.cend(),
                               std::array<size_t, 3>{(size_t)i, (size_t)j, (size_t)k}) != 0) {
                    cellComp->alive = true;
                } else {
                    cellComp->alive = false;
                }

                m_cells[i][j][k] = cellComp;
            }
        }
    }

    // TODO: fix bug
    auto camera = Camera::getInstance();
    GLfloat pos = m_fieldSize * (cubeSize + 40);
    camera->setPos({pos, pos, pos});
    camera->updateView();
    auto program = LifeProgram::getInstance();
    program->setView(camera->getView());
    program->updateView();
}
