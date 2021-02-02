#include <imgui_impl_sdl.h>
#include "systems/keyboardsystem.hpp"
#include "game.hpp"
#include "lifeprogram.hpp"

void KeyboardSystem::update_state(size_t delta)
{
    auto handles = getEntities();
    for (auto& [key, en] : handles)
        en->getComponent<KeyboardComponent>()->event_handler(
                SDL_GetKeyboardState(nullptr));

    const Uint8* state = SDL_GetKeyboardState(nullptr);

    if (state[SDL_SCANCODE_ESCAPE])
        setGameRunnable(false);

    int screen_width = utils::getWindowWidth<int>(*Game::getWindow());
    int screen_height = utils::getWindowHeight<int>(*Game::getWindow());

    GLfloat x_offset;
    GLfloat y_offset;
    SDL_Event e;
    auto camera = Camera::getInstance();
    auto program = LifeProgram::getInstance();
    while (SDL_PollEvent(&e)) {
        program->useFramebufferProgram();
        ImGui_ImplSDL2_ProcessEvent(&e);
        switch (e.type) {
            case SDL_QUIT:
                setGameRunnable(false);
                break;
            case SDL_MOUSEMOTION:
                if (m_middlePressed) {
                    x_offset = e.motion.xrel;
                    y_offset = e.motion.yrel;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (e.button.button == SDL_BUTTON_MIDDLE)
                    m_middlePressed = true;

                break;
            case SDL_MOUSEBUTTONUP:
                if (e.button.button == SDL_BUTTON_MIDDLE)
                    m_middlePressed = false;
                break;
            case SDL_MOUSEWHEEL:
                camera->processScroll(e.wheel.y / 10.f);
                program->setProjection(camera->getProjection(screen_width,
                                                             screen_height));
                program->updateProjection();
                break;
            case SDL_WINDOWEVENT:
                if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                    glViewport(0.f, 0.f, screen_width, screen_height);
                    program->setProjection(camera->getProjection(screen_width,
                                                                 screen_height));
                    program->updateProjection();
                }
                break;
            default:
                break;
        }

        if (m_middlePressed) {
            camera->processMovement(x_offset, y_offset, false);
            program->setView(camera->getView());
            program->updateView();
        }
    }
}

KeyboardSystem::KeyboardSystem() : m_middlePressed(false)
{

}
