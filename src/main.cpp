#include <SDL2/SDL.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>
#include <iostream>
#include <boost/format.hpp>

#include "game.hpp"
#include "utils/logger.hpp"
#include "exceptions/basegameexception.hpp"
#include "exceptions/glexception.hpp"
#include "lifeprogram.hpp"
#include "config.hpp"

#ifndef NDEBUG // use callgrind profiler
#include <valgrind/callgrind.h>
#endif

using utils::log::program_log_file_name;
using utils::log::Category;

void updateWindowSize(int& width, int& height)
{
    glm::vec<2, int> size = utils::getWindowSize<int>(*Game::getWindow());

    width = size.x;
    height = size.y;
}

int main(int argc, char *args[])
{
    int ret_code = 0;
    try {
        Game game;
        game.initOnceSDL2();
        game.initGL();
        game.initGame();
        auto camera = Camera::getInstance();

        int screen_width = utils::getWindowWidth<int>(*Game::getWindow());
        int screen_height = utils::getWindowHeight<int>(*Game::getWindow());
        auto program = LifeProgram::getInstance();
        program->loadProgram();
        glm::mat4 perspective = camera->getProjection(screen_width, screen_height);
        program->setProjection(perspective);
        program->setModel(glm::mat4(1.f));
        program->setView(glm::mat4(1.f));
        program->updateModel();
        program->updateView();
        program->updateProjection();
        program->setTexture(0);

        SDL_Event e;
        GLfloat delta_time = 0.f;
        GLfloat last_frame = 0.f;

#ifndef NDEBUG
        CALLGRIND_START_INSTRUMENTATION;
        CALLGRIND_TOGGLE_COLLECT;
#endif

        program->useFramebufferProgram();
        camera->setPos({0.f, 0.f, -40});
        program->setView(camera->getView());
        program->updateView();
        bool middle_pressed = false;
        bool firstRun = true;

        while (isGameRunnable()) {
            GLfloat cur_time = SDL_GetTicks();
            delta_time = cur_time - last_frame;
            last_frame = cur_time;

            // Draw scene
            game.update(delta_time);
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
    quit();

#ifndef NDEBUGp
    CALLGRIND_TOGGLE_COLLECT;
    CALLGRIND_STOP_INSTRUMENTATION;
#endif

    return ret_code;
}