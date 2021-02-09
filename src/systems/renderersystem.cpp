#include <boost/format.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui_impl_sdl.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <iostream>

#include "systems/renderersystem.hpp"
#include "components/spritecomponent.hpp"
#include "components/cellcomponent.hpp"
#include "render/render.hpp"
#include "utils/logger.hpp"
#include "exceptions/glexception.hpp"
#include "lifeprogram.hpp"
#include "game.hpp"
#include "config.hpp"
#include "utils/math.hpp"

using utils::log::Logger;
using boost::format;
using utils::log::program_log_file_name;
using utils::log::Category;
using glm::mat4;
using glm::vec3;
using glm::scale;
using utils::math::operator/;

static GLuint genTexture(GLuint width, GLuint height,
                         bool msaa = false, size_t samples = 4)
{
    GLuint texture;
    glGenTextures(1, &texture);

    if (msaa) {
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples,
                                GL_RGB, width, height, GL_TRUE);
    } else {
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                     GL_UNSIGNED_BYTE, nullptr);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (msaa)
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    else
        glBindTexture(GL_TEXTURE_2D, 0);

    if (GLuint error = glGetError(); error != GL_NO_ERROR)
        throw GLException((format("Unable to generate texture! %1%\n")
                           % gluErrorString(error)).str(),
                          program_log_file_name(), Category::INTERNAL_ERROR);

    return texture;
}

static GLuint genRbo(GLuint width, GLuint height, bool msaa = false,
                     size_t samples = 4)
{
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    if (msaa)
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples,
                                         GL_DEPTH24_STENCIL8, width, height);
    else
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    return rbo;
}

#define CHECK_FRAMEBUFFER_COMPLETE() \
if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) \
throw GLException((boost::format( \
        "Warning: Unable to generate frame buffer. " \
        "GL Error: %s\n") % gluErrorString(glGetError())).str(), \
        utils::log::program_log_file_name(), \
        utils::log::Category::INITIALIZATION_ERROR); \

RendererSystem::RendererSystem() : m_frameBuffer(0), m_videoSettingsOpen(false),
                                   m_colorSettingsOpen(false)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.WantCaptureMouse = true;
    io.WantCaptureKeyboard = true;

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.Alpha = 1.0f;
    style.AntiAliasedLines = false;

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(Game::getWindow(), Game::getGLContext());
    ImGui_ImplOpenGL3_Init();

    int screen_width = utils::getWindowWidth<GLuint>(*Game::getWindow());
    int screen_height = utils::getWindowHeight<GLuint>(*Game::getWindow());
    m_aspectRatio = static_cast<GLfloat>(screen_width) / screen_height;

    bool msaa = Config::getVal<bool>("MSAA");

    if (msaa) {
        // Generate multisampled framebuffer
        glGenFramebuffers(1, &m_frameBufferMSAA);
        glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferMSAA);
        // Create multisampled texture attachment
        m_frameBufTexMSAA = genTexture(screen_width, screen_height, true);
        GLuint rbo = genRbo(screen_width, screen_height, true);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D_MULTISAMPLE, m_frameBufTexMSAA, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                  GL_RENDERBUFFER, rbo);
        CHECK_FRAMEBUFFER_COMPLETE();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Generate intermediate framebuffer
        glGenFramebuffers(1, &m_frameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
        // Create color attachment texture
        m_frameBufTex = genTexture(screen_width, screen_height);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D,
                               m_frameBufTex, 0);
    } else {
        // Generate not multisampled buffer
        glGenFramebuffers(1, &m_frameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
        // Create color texture attachment
        m_frameBufTex = genTexture(screen_width, screen_height);
        GLuint rbo = genRbo(screen_width, screen_height);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, m_frameBufTex, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                  GL_RENDERBUFFER, rbo);
    }

    CHECK_FRAMEBUFFER_COMPLETE();

    auto camera = Camera::getInstance();
    GLfloat cubSize = 20.f;
    int fieldSize = Config::getVal<int>("FieldSize") * (cubSize + 10);
    camera->setPos({fieldSize, fieldSize, fieldSize});
    camera->updateView();
}

RendererSystem::~RendererSystem()
{
    glDeleteFramebuffers(1, &m_frameBuffer);
}

void RendererSystem::drawSprites()
{
//    auto sprites = getEntitiesByTag<SpriteComponent>();
    auto sprites = m_cesManager->getEntities();
    auto program = LifeProgram::getInstance();
    for (const auto& [key, en]: sprites) {
        std::shared_ptr<CellComponent> cell;
        if ((cell = en->getComponentInsert<CellComponent>()))
            if (!cell->alive)
                continue;

        const glm::vec4 borderColor = Config::getVal<glm::vec4>("CellBorderColor");
        const glm::vec4 cellColor = Config::getVal<glm::vec4>("CellColor");

        auto posComp = en->getComponentInsert<PositionComponent>();
        const glm::vec3 pos = glm::vec3(posComp->x, posComp->y, posComp->z);
        program->setVec4("Color", cellColor);
        program->setVec4("OutlineColor", borderColor);
        render::drawTexture(*program, *en->getComponentInsert<SpriteComponent>()->sprite, pos);
    }

    if (GLenum error = glGetError(); error != GL_NO_ERROR)
        throw GLException((format("\n\tRender while drawing sprites: %1%\n")
                           % glewGetErrorString(error)).str(),
                          program_log_file_name(), Category::INTERNAL_ERROR);
}

void RendererSystem::update_state(size_t delta)
{
    drawToFramebuffer();
    drawGui();
}

void RendererSystem::drawToFramebuffer()
{
    auto program = LifeProgram::getInstance();
    program->useFramebufferProgram();

    int screen_width = utils::getDisplayWidth<int>();
    int screen_height = utils::getDisplayHeight<int>();

    auto camera = Camera::getInstance();
    glViewport(0.f, 0.f, screen_width, screen_height);
    program->setProjection(camera->getProjection(screen_width,
                                                 screen_height));
    program->updateProjection();

    // Render to texture
    if (Config::getVal<bool>("MSAA"))
        glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferMSAA);
    else
        glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);

    glm::vec4 color = Config::getVal<glm::vec4>("BackgroundColor");
    glClearColor(color.x, color.y, color.z, color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    drawSprites();
}

void RendererSystem::drawGui()
{
    auto screen_width = utils::getWindowWidth<GLfloat>(*Game::getWindow());
    auto screen_height = utils::getWindowHeight<GLfloat>(*Game::getWindow());

    auto program = LifeProgram::getInstance();
    bool msaa = Config::getVal<bool>("MSAA");
    if (msaa) {
        // Render texture to window
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_frameBufferMSAA);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_frameBuffer);
        glBlitFramebuffer(0, 0, screen_width, screen_height, 0, 0,
                          screen_width, screen_height,
                          GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    program->useScreenProgram();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(Game::getWindow());
    ImGui::NewFrame();

    switch (Config::getVal<int>("Theme")) {
        case 0:
            ImGui::StyleColorsLight();
            break;
        case 1:
            ImGui::StyleColorsClassic();
            break;
        case 2:
            ImGui::StyleColorsDark();
            break;
        default:
            break;
    }

    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowSize({static_cast<float>(screen_width),
                              static_cast<float>(screen_height)});
    bool open = true;
    ImGui::Begin("GameWindow", &open, ImGuiWindowFlags_NoResize
                                      | ImGuiWindowFlags_NoScrollbar
                                      | ImGuiWindowFlags_NoScrollWithMouse
                                      | ImGuiWindowFlags_NoTitleBar
                                      | ImGuiWindowFlags_NoBringToFrontOnFocus);
    {
        ImGui::BeginTable("table1", 2, ImGuiTableFlags_Borders
                                       | ImGuiTableFlags_Resizable
                                       | ImGuiTableFlags_NoHostExtendX
                                       | ImGuiTableFlags_NoHostExtendY, {0, -1});
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Settings");
            ImGui::Separator();

            ImGui::Text("Field size: ");
            ImGui::SameLine();
            ImGui::InputInt("##field_size", &Config::getVal<int>("FieldSize"));

            ImGui::Text("Neighbours count to live");
            ImGui::SameLine();
            ImGui::InputInt("##neir_count", &Config::getVal<int>("NeirCount"));

            ImGui::Text("Neighbours count to die");
            ImGui::SameLine();
            ImGui::InputInt("##neir_count_die", &Config::getVal<int>("NeirCountDie"));

            ImGui::Text("Step time");
            ImGui::SameLine();
            ImGui::InputFloat("##step_time", &Config::getVal<GLfloat>("StepTime"));

            ImGui::Checkbox("Inverse rotation", &Config::getVal<bool>("InverseRotation"));

            if (ImGui::Button("Start simulation"))
                setGameState(GameStates::PLAY);

            if (ImGui::Button("Pause simulation"))
                setGameState(GameStates::PAUSE);

            if (ImGui::Button("Stop simulation"))
                setGameState(GameStates::STOP);

            if (ImGui::Button("Save simulation"))
                Config::save(Config::getVal<const char*>("ConfigFile"));

            if (ImGui::Button("Load simulation"))
                Config::load(Config::getVal<const char*>("ConfigFile"));

            if (ImGui::Button("Color Settings"))
                m_colorSettingsOpen = true;

            if (m_colorSettingsOpen) {
                ImGui::Begin("Color settings", &m_colorSettingsOpen);
                ImGui::Separator();
                ImGui::Text("Background color");
                ImGui::ColorPicker4("##Background color", glm::value_ptr(
                        Config::getVal<glm::vec4>("BackgroundColor")));
                ImGui::Separator();
                ImGui::Text("Cell color");
                ImGui::ColorPicker4("##Cell color", glm::value_ptr(
                        Config::getVal<glm::vec4>("CellColor")));
                ImGui::Separator();
                ImGui::Text("Cell border color");
                ImGui::ColorPicker4("##Cell border color", glm::value_ptr(
                        Config::getVal<glm::vec4>("CellBorderColor")));
                ImGui::Separator();
                ImGui::Checkbox("Enable color game(Need simulation restart)",
                                &Config::getVal<bool>("ColoredLife"));
                ImGui::End();
            }

            if (ImGui::Button("Video settings"))
                m_videoSettingsOpen = true;

            if (m_videoSettingsOpen) {
                const char* items[] = { "Light", "Classic", "Dark" };
                ImGui::Begin("Video settings", &m_videoSettingsOpen);
                bool msaaEnabled = Config::getVal<bool>("MSAA");
                if(ImGui::Checkbox("Enable antialiasing(Need game restart)",
                                   &Config::getVal<bool>("MSAA"))
                   || msaaEnabled) {
                    ImGui::Text("MSAA Samples");
                    ImGui::SameLine();
                    ImGui::InputInt("##msaa_samples",
                                    &Config::getVal<int>("MSAASamples"));
                }
                ImGui::Text("Application theme:");
                ImGui::SameLine();
                ImGui::ListBox("", &Config::getVal<int>("Theme"), items, 3);
                ImGui::End();
            }

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("Render");
            auto size = ImGui::GetContentRegionAvail();
            GLfloat image_height = size.x / m_aspectRatio;
            size.y = image_height;
            if (getGameState() != GameStates::STOP) {
                ImGui::Image((ImTextureID) m_frameBufTex, size, {0, 1}, {1, 0});
            }
        }
        ImGui::EndTable();
    }
    ImGui::End();

    ImGui::Render();
//            ImGui::UpdatePlatformWindows();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
