#include <memory>
#include <boost/format.hpp>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <thread>
#include <omp.h>

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

Field World::m_cells;

World::World() : m_wasInit(false),
//                 m_cells(boost::extents[6][6][6]),
                 m_pool(get_thread_count()) {
    if (!Config::hasKey("FieldSize"))
        Config::addVal("FieldSize", 6, "int");
    if (!Config::hasKey("StepTime"))
        Config::addVal("StepTime", 5.f, "float");
    if (!Config::hasKey("NeirCount"))
        Config::addVal("NeirCount", 3, "int");
    if (!Config::hasKey("NeirCountDie"))
        Config::addVal("NeirCountDie", 4, "int");
    if (!Config::hasKey("BackgroundColor"))
        Config::addVal("BackgroundColor", glm::vec4(0.2f, 0.f, 0.2f, 1.f),
                       "vec4");
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
        Config::addVal("CellBorderColor", glm::vec4(1.f, 1.f, 1.f, 1.f),
                       "vec4");
    if (!Config::hasKey("ColoredLife"))
        Config::addVal("ColoredLife", false, "bool");
}

World::~World() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

void World::update(size_t delta) {
//    if constexpr (debug)
//        m_fps.update();

    if (getGameState() == GameStates::PAUSE
        && !m_timer.isPaused()) {
        m_timer.pause();
    }

    if (getGameState() == GameStates::STOP
        && getPrevGameState() != GameStates::STOP) {
        setGameState(GameStates::STOP);
        m_timer.stop();
    }

    if (getGameState() == GameStates::PLAY
        && getPrevGameState() == GameStates::PAUSE) {
        setGameState(GameStates::PLAY);
        m_timer.unpause();
    }

    if (getGameState() == GameStates::PLAY
        && getPrevGameState() == GameStates::STOP) {
        setGameState(GameStates::PLAY);
        m_timer.start();
        // reinit field
        init();
    }

    if (getGameState() == GameStates::PLAY) {
        GLfloat stepTime = Config::getVal<GLfloat>("StepTime");
        if (m_timer.getTicks() / 1000.f > stepTime) {
            m_timer.stop();
            m_timer.start();
            update_field_omp();
        }
    }
//    filter_entities();
    for (auto &system: m_systems)
        system.second->update(delta);
}

void World::init() {
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

void World::update_field() {
    size_t neirCountToDie = Config::getVal<int>("NeirCountDie");
    size_t neirCountToLife = Config::getVal<int>("NeirCount");
    auto task = [this, neirCountToDie, neirCountToLife](int start, int end) {
        using namespace utils::math;
        glm::vec3 color = {0.f, 0.f, 0.f};
        unsigned char neirCount = 0;
        CellIndex i, j, k;
        bool alive;
        int indices_offsets[][3] = {
                {0,  1,  0},
                {0,  -1, 0},
                {-1, 0,  0},
                {1,  0,  0},
                {0,  0,  1},
                {-0, 0,  -1},
                {-1, 1,  -1},
                {1,  1,  -1},
                {1,  1,  1},
                {-1, 1,  1},
                {-1, -1, -1},
                {1,  -1, -1},
                {1,  -1, 1},
                {-1, -1, 1}
        };

        for (i = start; i < end; ++i) {
            for (j = start; j < end; ++j) {
                for (k = start; k < end; ++k) {
                    color = {0.f, 0.f, 0.f};
                    neirCount = 0;
                    alive = m_cells[i][j][k].alive;
                    auto &cell = m_cells[i][j][k];
                    for (const auto&[x, y, z]: indices_offsets) {
                        if ((i + x >= end || i + x < start)
                            || (j + y >= end || j + y < start)
                            || (k + z >= end || k + z < start))
                            continue;

                        if (m_cells[i + x][j + y][k + z].alive) {
                            ++neirCount;
                            color += m_cells[i + x][j + y][k + z].color;
                        }

                        if ((alive && neirCount >= neirCountToDie))
                            break;
                    }

                    // Dead case
                    if (!alive) {
                        if (neirCount >= neirCountToLife) { // Now alive!!!
                            m_newState[i][j][k].alive = true;
                            m_newState[i][j][k].pos = cell.pos;
                            m_newState[i][j][k].color = color / neirCount;
                        }

                        continue;
                    }

                    // Life case
                    if (neirCount >= neirCountToDie)
                        m_newState[i][j][k].alive = false;
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
            m_pool.addJob(task, i * partSize, m_fieldSize);
        else
            m_pool.addJob(task, i * partSize, (i + 1) * partSize);
    }

    m_pool.waitForFinish();

    m_cells = m_newState;
}

void World::update_field_omp()
{
    size_t neirCountToDie = Config::getVal<int>("NeirCountDie");
    size_t neirCountToLife = Config::getVal<int>("NeirCount");
    using namespace utils::math;

    int indices_offsets[][3] = {
            {0,  1,  0},
            {0,  -1, 0},
            {-1, 0,  0},
            {1,  0,  0},
            {0,  0,  1},
            {-0, 0,  -1},
            {-1, 1,  -1},
            {1,  1,  -1},
            {1,  1,  1},
            {-1, 1,  1},
            {-1, -1, -1},
            {1,  -1, -1},
            {1,  -1, 1},
            {-1, -1, 1}
    };
#pragma omp parallel for collapse(3)
    for (CellIndex i = 1; i < m_fieldSize + 1; ++i) {
        for (CellIndex j = 1; j < m_fieldSize + 1; ++j) {
            for (CellIndex k = 1; k < m_fieldSize + 1; ++k) {
                glm::vec3 color = {0.f, 0.f, 0.f};
                int neirCount = 0;

                bool alive = m_cells[i][j][k].alive;
                auto &cell = m_cells[i][j][k];
                for (const auto&[x, y, z]: indices_offsets) {
                    if (m_cells[i + x][j + y][k + z].alive) {
                        ++neirCount;
                        color += m_cells[i + x][j + y][k + z].color;
                    }

                    if ((alive && neirCount >= neirCountToDie))
                        break;
                }

                // Dead case
                if (!alive) {
                    if (neirCount >= neirCountToLife) // Now alive!!!
                        m_newState[i][j][k] = {true, color / neirCount, cell.pos};

                    continue;
                }

                // Life case
                if (neirCount >= neirCountToDie)
                    m_newState[i][j][k].alive = false;
            }
        }
    }

#pragma omp parallel for collapse(3)
    for (CellIndex i = 1; i < m_fieldSize + 1; ++i)
        for (CellIndex j = 1; j < m_fieldSize + 1; ++j)
            for (CellIndex k = 1; k < m_fieldSize + 1; ++k)
                m_cells[i][j][k] = m_newState[i][j][k];

//    m_pool.waitForFinish();

//    m_cells = m_newState;
}

void World::filter_entities()
{
    for (auto it = m_entities.begin(); it != m_entities.end();)
        it = !it->second->isActivate() ? m_entities.erase(it) : ++it;
}

void World::init_field() {
    using utils::math::cantor_pairing;

    const std::vector<std::array<size_t, 3>> initial_cells = {
            {0,  0,  0},
            {1,  0,  0},
            {2,  0,  0},
            {3,  0,  0},
            {0,  1,  0},
            {0,  2,  0},
            {0,  3,  0},
            {0,  0,  1},
            {0,  0,  2},
            {0,  0,  3},
            {0,  0,  5},

            {4,  0,  0},
            {6,  0,  0},
            {8,  0,  0},
            {10, 0,  0},
            {12, 1,  0},
            {14, 2,  0},
            {16, 3,  0},
            {18, 0,  1},
            {20, 0,  2},
            {13, 10, 3},
            {14, 10, 5},
            {4,  1,  0},
            {6,  1,  0},
            {8,  1,  0},
            {10, 1,  0},
            {12, 2,  0},
            {14, 3,  0},
            {16, 4,  0},
            {18, 1,  1},
            {20, 1,  2},
            {13, 11, 3},
            {14, 11, 5},
    };

    GLfloat init_x = 0.f;
    GLfloat init_y = 0.f;
    GLfloat init_z = 0.f;

    m_cells.resize(boost::extents[0][0][0]); // clear array if reinit
    m_cells.resize(boost::extents[m_fieldSize + 2][m_fieldSize + 2][m_fieldSize + 2]);

    std::shared_ptr<Sprite> sprite_com = std::make_shared<Sprite>();
    sprite_com->addTexture(getResourcePath("cube.obj"), cubeSize,
                           cubeSize, cubeSize);
    sprite_com->generateDataBuffer();
    CellComponent::sprite = sprite_com;
    utils::Random rand;
    CellComponent cellComp;
    for (CellIndex i = 1; i < m_fieldSize + 1; ++i) {
        for (CellIndex j = 1; j < m_fieldSize + 1; ++j) {
            for (CellIndex k = 1; k < m_fieldSize + 1; ++k) {
                cellComp.pos.x = init_x + cubeSize * i;
                cellComp.pos.y = init_y + cubeSize * j;
                cellComp.pos.z = init_z + cubeSize * k;
                cellComp.color =
                        {rand.generateu<GLfloat>(0.f, 1.f),
                         rand.generateu<GLfloat>(0.f, 1.f),
                         rand.generateu<GLfloat>(0.f, 1.f)};
                if (std::count(initial_cells.cbegin(),
                               initial_cells.cend(),
                               std::array<size_t, 3>{(size_t) i, (size_t) j,
                                                     (size_t) k}) != 0) {
                    cellComp.alive = true;
                } else {
                    cellComp.alive = false;
                }

                m_cells[i][j][k] = cellComp;
            }
        }
    }

    m_newState.resize(boost::extents[0][0][0]); // clear array if reinit
    m_newState.resize(boost::extents[m_fieldSize + 2][m_fieldSize + 2][m_fieldSize + 2]);
    m_newState = m_cells;

    // TODO: fix bug
    auto camera = Camera::getInstance();
    GLfloat pos = m_fieldSize * (cubeSize + 40);
    camera->setPos({pos, pos, pos});
    camera->updateView();
    auto program = LifeProgram::getInstance();
    program->setView(camera->getView());
    program->updateView();
}
