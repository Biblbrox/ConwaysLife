#include <SDL2/SDL.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/epsilon.hpp>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>
#include <iostream>
#include <boost/format.hpp>

#include "game.hpp"
#include "utils/logger.hpp"
#include "exceptions/basegameexception.hpp"
#include "lifeprogram.hpp"

#ifndef NDEBUG // use callgrind profiler
#include <valgrind/callgrind.h>
#include <glm/gtx/euler_angles.hpp>

#endif

using utils::log::program_log_file_name;
using utils::log::Category;

int main(int argc, char *args[])
{
    int ret_code = 0;
    try {
        Game game;
        game.initOnceSDL2();
        game.initGL();
        game.initGame();
        Camera camera;

        auto screen_width = utils::getScreenWidth<GLuint>();
        auto screen_height = utils::getScreenHeight<GLuint>();
        auto program = LifeProgram::getInstance();
        program->loadProgram();
        glm::mat4 perspective = camera.getProjection(screen_width, screen_height);
        program->setProjection(perspective);
        program->setModel(glm::mat4(1.f));
        program->setView(glm::mat4(1.f));
        program->updateModel();
        program->updateView();
        program->updateProjection();
        program->setTexture(0);

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        // Setup Platform/Renderer backends
        ImGui_ImplSDL2_InitForOpenGL(m_window, m_glcontext);
        ImGui_ImplOpenGL3_Init();

        SDL_Event e;
        GLfloat delta_time = 0.f;
        GLfloat last_frame = 0.f;

#ifndef NDEBUG
        CALLGRIND_START_INSTRUMENTATION;
        CALLGRIND_TOGGLE_COLLECT;
#endif

        camera.setPos({0.f, 0.f, -40});
        program->setView(camera.getView());
        program->updateView();
        bool middle_pressed = false;
        bool firstRun = true;
        while (isGameRunnable()) {
            GLfloat cur_time = SDL_GetTicks();
            delta_time = cur_time - last_frame;
            last_frame = cur_time;

            GLfloat x_offset;
            GLfloat y_offset;
            while (SDL_PollEvent(&e)) {
                ImGui_ImplSDL2_ProcessEvent(&e);
                switch (e.type) {
                    case SDL_QUIT:
                        setGameRunnable(false);
                        break;
                    case SDL_MOUSEMOTION:
                        if (middle_pressed) {
                            x_offset = e.motion.xrel;
                            y_offset = e.motion.yrel;
                        }
                        break;
                    case SDL_MOUSEBUTTONDOWN:
                        if (e.button.button == SDL_BUTTON_MIDDLE)
                            middle_pressed = true;

                        break;
                    case SDL_MOUSEBUTTONUP:
                        if (e.button.button == SDL_BUTTON_MIDDLE)
                            middle_pressed = false;
                        break;
                    case SDL_MOUSEWHEEL:
                        camera.processScroll(e.wheel.y / 10.f);
                        program->setProjection(camera.getProjection(screen_width,
                                                                    screen_height));
                        program->updateProjection();
                        break;
                    default:
                        break;
                }

                if (middle_pressed) {
                    camera.processMovement(x_offset, y_offset, false);
                    program->setView(camera.getView());
                    program->updateView();
                }
            }

            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame(m_window);
            ImGui::NewFrame();

            // Render logic
            bool some = true;
            ImGui::Begin("Another Window", &some);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                some = false;
            ImGui::End();

            ImGui::Render();
            glViewport(0.f, 0.f, screen_width, screen_height);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            game.update(delta_time);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            game.flush();

            if (firstRun) {
                if (SDL_CaptureMouse(SDL_TRUE) != 0)
                    utils::log::Logger::write(program_log_file_name(),
                                              Category::INITIALIZATION_ERROR,
                                              (boost::format(
                                                      "Warning: Unable to capture mouse. "
                                                      "SDL Error: %s\n")
                                               % SDL_GetError()).str());
                firstRun = false;
            }
            // End render logic
        }

    } catch (const BaseGameException& e) {
        utils::log::Logger::write(e.fileLog(), e.categoryError(), e.what());
        ret_code = EXIT_FAILURE;
    } catch (const std::exception& e) {
        utils::log::Logger::write(program_log_file_name(),
                                  Category::UNEXPECTED_ERROR, e.what());
        ret_code = EXIT_FAILURE;
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    quit();

#ifndef NDEBUGp
    CALLGRIND_TOGGLE_COLLECT;
    CALLGRIND_STOP_INSTRUMENTATION;
#endif

    return ret_code;
}
