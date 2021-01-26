#include <SDL2/SDL.h>
#include <glm/gtc/matrix_transform.hpp>

#include "game.hpp"
#include "utils/logger.hpp"
#include "exceptions/basegameexception.hpp"
#include "lifeprogram.hpp"

#ifndef NDEBUG // use callgrind profiler
#include <valgrind/callgrind.h>
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

        auto screen_width = utils::getScreenWidth<GLuint>();
        auto screen_height = utils::getScreenHeight<GLuint>();
        auto program = LifeProgram::getInstance();
        program->loadProgram();
        glm::mat4 perspective = glm::perspective(
                glm::radians(45.f),
                (float)screen_width / (float)screen_height, 0.1f, 1000.f);
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

        Camera camera;
        while (isGameRunnable()) {
            glViewport(0.f, 0.f, screen_width, screen_height);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            GLfloat cur_time = SDL_GetTicks();
            delta_time = cur_time - last_frame;
            last_frame = cur_time;

            // Mouse position
            GLfloat lastX = screen_width / 2.f;
            GLfloat lastY = screen_height / 2.f;
            GLfloat yaw = -90.f;
            GLfloat pitch = 0.f;
            GLfloat sensitivity = 0.1f;
            int x;
            int y;
            GLfloat x_offset;
            GLfloat y_offset;
            while (SDL_PollEvent(&e)) {
                switch (e.type) {
                    case SDL_QUIT:
                        setGameRunnable(false);
                    case SDL_MOUSEMOTION:
                        x = e.motion.x;
                        y = e.motion.y;

                        x_offset = x - lastX;
                        y_offset = lastY - y;

                        lastX = x;
                        lastY = y;

                        x_offset *= sensitivity;
                        y_offset *= sensitivity;

                        yaw += x_offset;
                        pitch += y_offset;

                        if (pitch > 89.f)
                            pitch = 89.f;
                        if (pitch < -89.f)
                            pitch = -89.f;

                        glm::vec3 direction;
                        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
                        direction.y = sin(glm::radians(pitch));
                        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
                        camera.setFront(glm::normalize(direction));

                        break;
                    default:
                        break;
                }

                const Uint8* state = SDL_GetKeyboardState(nullptr);

                GLfloat camSpeed = 0.01f * delta_time;
                if (state[SDL_SCANCODE_W])
                    camera.translateForward(camSpeed);
                if (state[SDL_SCANCODE_S])
                    camera.translateBack(camSpeed);
                if (state[SDL_SCANCODE_A])
                    camera.translateLeft(camSpeed);
                if (state[SDL_SCANCODE_D])
                    camera.translateRight(camSpeed);

                program->setView(camera.getView());
                program->updateView();
            }

            game.update(delta_time);
            game.flush();
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

#ifndef NDEBUG
    CALLGRIND_TOGGLE_COLLECT;
    CALLGRIND_STOP_INSTRUMENTATION;
#endif

    return ret_code;
}
