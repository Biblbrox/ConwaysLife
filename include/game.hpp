#ifndef MOONLANDER_GAME_HPP
#define MOONLANDER_GAME_HPP

#include <GL/glew.h>
#include <memory>

#include "world.hpp"

#define WINDOW_FLAGS (SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE)
#define IMG_FLAGS IMG_INIT_PNG

enum class GameStates {
    NORMAL,
    WIN,
    FAIL,
    NEED_REPLAY
};

void quit();
void setGameRunnable(bool runnable);
bool isGameRunnable();
void setGameState(GameStates state);
GameStates getGameState();
GameStates getPrevGameState();
class Game
{
public:
    Game();
    ~Game();

    void initOnceSDL2();
    void initGL();
    void initGame();

    void flush();

    void update(size_t delta);

    static SDL_Window* getWindow()
    {
        return Game::m_window;
    }

    static SDL_GLContext getGLContext()
    {
        return Game::m_glcontext;
    }

private:
    GLuint m_screenWidth;
    GLuint m_screenHeight;
public:
    bool vsync_supported;
private:
    World m_world;
    static SDL_Window* m_window;
    static SDL_GLContext m_glcontext;
};

#endif //MOONLANDER_GAME_HPP
