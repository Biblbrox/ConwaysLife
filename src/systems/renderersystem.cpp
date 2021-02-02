#include <boost/format.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>
#include <imgui_internal.h>
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

void RendererSystem::drawLevel()
{
    if (GLenum error = glGetError(); error != GL_NO_ERROR)
        throw GLException((format("\n\tRender while drawing level: %1%\n")
                           % glewGetErrorString(error)).str(),
                          program_log_file_name(), Category::INTERNAL_ERROR);
}

void RendererSystem::drawGui()
{
    auto program = LifeProgram::getInstance();
    // Render texture to window
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    program->useScreenProgram();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(Game::getWindow());
    ImGui::NewFrame();

    GLfloat screen_width = utils::getWindowWidth<GLfloat>(*Game::getWindow());
    GLfloat screen_height = utils::getWindowHeight<GLfloat>(*Game::getWindow());
    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowSize({static_cast<float>(screen_width),
                              static_cast<float>(screen_height)});
    bool open = true;
    GLfloat pad = 10;
    ImGui::Begin("GameWindow", &open, ImGuiWindowFlags_NoResize
                                      | ImGuiWindowFlags_NoScrollbar
                                      | ImGuiWindowFlags_NoScrollWithMouse
                                      | ImGuiWindowFlags_NoTitleBar);
    {
        ImGui::BeginTable("table1", 2, ImGuiTableFlags_Borders
                                       | ImGuiTableFlags_Resizable
                                       | ImGuiTableFlags_NoHostExtendX
                                       | ImGuiTableFlags_NoHostExtendY, {0, -1});
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
//            ImGui::Separator();
            ImGui::Text("Visuals");
//            ImGui::Separator();

            bool some;
            ImGui::Checkbox("Enable Visuals", &some);
            ImGui::Checkbox("Chams", &some);
            ImGui::Checkbox("Skeleton", &some);
            ImGui::Checkbox("Box", &some);

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("Render");
            auto size = ImGui::GetContentRegionAvail();
            std::cout << "Image x: " << size.x << ", y: " << size.y << "\n";
            ImGui::Image((ImTextureID)m_frameBufTex, size, {0, 1}, {1, 0});
        }
        ImGui::EndTable();
    }
    ImGui::End();

    ImGui::Render();
//            ImGui::UpdatePlatformWindows();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void RendererSystem::drawSprites()
{
    auto sprites = getEntitiesByTag<SpriteComponent>();
    auto program = LifeProgram::getInstance();
    // Render to texture
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
    glClearColor(0.2f, 0.0f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    program->useFramebufferProgram();
    for (const auto& [key, en]: sprites) {
        std::shared_ptr<CellComponent> cell;
        if ((cell = en->getComponent<CellComponent>()))
            if (!cell->alive)
                continue;

        const glm::vec3 pos =
                glm::vec3(en->getComponent<PositionComponent>()->x,
                          en->getComponent<PositionComponent>()->y,
                          en->getComponent<PositionComponent>()->z);
        render::drawTexture(*program, *en->getComponent<SpriteComponent>()->sprite,
                            pos, en->getComponent<PositionComponent>()->angle);
    }

    if (GLenum error = glGetError(); error != GL_NO_ERROR)
        throw GLException((format("\n\tRender while drawing sprites: %1%\n")
                           % glewGetErrorString(error)).str(),
                          program_log_file_name(), Category::INTERNAL_ERROR);
}

void RendererSystem::drawText()
{

    if (GLenum error = glGetError(); error != GL_NO_ERROR)
        throw GLException((format("\n\tRender while drawing level: %1%\n")
                           % glewGetErrorString(error)).str(),
                          program_log_file_name(), Category::INTERNAL_ERROR);
}

void RendererSystem::update_state(size_t delta)
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

    drawLevel();
    drawSprites();
    drawText();

    program->useScreenProgram();
    drawGui();
}

static GLuint genFramebufferText(GLuint width, GLuint height)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return texture;
}

static GLuint genRbo(GLuint width, GLuint height)
{
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    return rbo;
}

RendererSystem::RendererSystem() : m_frameBuffer(0)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
//        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    // Setup Dear ImGui style
//        ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(Game::getWindow(), Game::getGLContext());
    ImGui_ImplOpenGL3_Init();

    int screen_width = utils::getWindowWidth<GLuint>(*Game::getWindow());
    int screen_height = utils::getWindowHeight<GLuint>(*Game::getWindow());
    glGenFramebuffers(1, &m_frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
    m_frameBufTex = genFramebufferText(screen_width, screen_height);
    GLuint rbo = genRbo(screen_width, screen_height);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, m_frameBufTex, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, rbo);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw GLException((boost::format(
                "Warning: Unable to generate frame buffer. "
                "GL Error: %s\n") % gluErrorString(glGetError())).str(),
                          program_log_file_name(), Category::INITIALIZATION_ERROR);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RendererSystem::~RendererSystem()
{
    glDeleteFramebuffers(1, &m_frameBuffer);
}
