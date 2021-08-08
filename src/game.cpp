#include <GL/glew.h>
#include <SDL.h>
//#include <SDL_image.h>
//#include <SDL_ttf.h>
#include <boost/format.hpp>

#include "game.hpp"
#include "config.hpp"
#include "constants.hpp"
#include "exceptions/sdlexception.hpp"
#include "exceptions/glexception.hpp"
#include "utils/utils.hpp"

//#define INIT_SOUND

#ifdef INIT_SOUND
#include <SDL_mixer.h>
#define FREQUENCY 44100
#define SAMPLE_FORMAT MIX_DEFAULT_FORMAT
#define CHUNK_SIZE 2048
#define NUM_CHANNELS 2
#endif

static bool imgInit = false;
static bool mixerInit = false;

static bool isRun = true;
static GameStates state = GameStates::STOP;
static GameStates prevState = GameStates::STOP;

SDL_Window* Game::m_window = nullptr;
SDL_GLContext Game::m_glcontext = nullptr;

using utils::log::Logger;
using utils::log::Category;
using utils::log::program_log_file_name;
using boost::format;

void quit()
{
    if (Game::getGLContext())
        SDL_GL_DeleteContext(Game::getGLContext());
//    if (TTF_WasInit())
//        TTF_Quit();
//    if (imgInit)
//        IMG_Quit();
    if (Game::getWindow())
        SDL_DestroyWindow(Game::getWindow());
#ifdef INIT_SOUND
//    if (mixerInit)
//        Mix_Quit();
#endif

    SDL_Quit();
}

void setGameRunnable(bool runnable)
{
    isRun = runnable;
}

bool isGameRunnable()
{
    return isRun;
}

GameStates getGameState()
{
    return state;
}

void setGameState(GameStates st)
{
    prevState = state;
    state = st;
}

GameStates getPrevGameState()
{
    return prevState;
}


void Game::initOnceSDL2()
{
    static bool didInit = false;

    if (didInit)
        return;

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        throw SdlException((format("SDL initialization error: %1%\n")
                            % SDL_GetError()).str(),
                           program_log_file_name(),
                           Category::INITIALIZATION_ERROR);

//    if ((IMG_Init(IMG_FLAGS) & IMG_FLAGS) != IMG_FLAGS)
//        throw SdlException((format("SDL_IMG initialization error: %1%\n")
//                            % IMG_GetError()).str(),
//                           program_log_file_name(),
//                           Category::INITIALIZATION_ERROR);
//
    imgInit = true;

#ifdef INIT_SOUND
    if (Mix_OpenAudio(FREQUENCY, SAMPLE_FORMAT, NUM_CHANNELS, CHUNK_SIZE) < 0)
        throw SdlException((format("SDL_Mixer initialization error: %1%\n")
                            % Mix_GetError()).str(),
                           program_log_file_name(),
                           Category::INITIALIZATION_ERROR);

    mixerInit = true;
#endif

//    if (TTF_Init() == -1)
//        throw SdlException((format("SDL_TTF initialization error: %1%\n")
//                            % TTF_GetError()).str(),
//                           program_log_file_name(),
//                           Category::INITIALIZATION_ERROR);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);


    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_RELEASE_BEHAVIOR, 1);
//    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
//    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
//    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
//    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

    if (Config::getVal<bool>("MSAA")) {
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,
                            Config::getVal<int>("MSAASamples"));
    }

    SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "1");

    SDL_ShowCursor(SDL_ENABLE);
    if (SDL_ShowCursor(SDL_QUERY) != SDL_ENABLE)
        Logger::write(program_log_file_name(), Category::INITIALIZATION_ERROR,
                      (format("Warning: Unable to show cursor. "
                              "SDL Error: %s\n")
                       % SDL_GetError()).str());
}


Game::Game() : vsync_supported(false)
{
    Game::m_glcontext = nullptr;
    Game::m_window = nullptr;
}

void Game::update(size_t delta)
{
    m_world.update(delta);
}

void Game::initGL()
{
    auto screenWidth = utils::getDisplayWidth<GLuint>();
    auto screenHeight = utils::getDisplayHeight<GLuint>();

    Game::m_window = SDL_CreateWindow(GAME_NAME.c_str(),
                                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                screenWidth, screenHeight,
                                WINDOW_FLAGS);
    if (!m_window)
        throw SdlException((format("Unable to create window. Error: %1%\n")
                            % SDL_GetError()).str(),
                           program_log_file_name(),
                           Category::INITIALIZATION_ERROR);

    Game::m_glcontext = SDL_GL_CreateContext(Game::m_window);
    // Init OpenGL context
    if (!Game::m_glcontext)
        throw SdlException((format("Unable to create gl context. Error: %1%\n")
                            % SDL_GetError()).str(),
                           program_log_file_name(),
                           Category::INITIALIZATION_ERROR);

    glewExperimental = GL_TRUE;
    GLenum error = glewInit();
    if (error != GLEW_OK)
        throw SdlException((format("Error when initializing GLEW: %s\n")
                            % glewGetErrorString(error)).str(),
                           program_log_file_name(),
                           Category::INITIALIZATION_ERROR);

    if (!GLEW_VERSION_2_1)
        throw SdlException("Your opengl version too old\n",
                           program_log_file_name(),
                           Category::INITIALIZATION_ERROR);

    //Initialize clear color
    glClearColor(0.f, 0.f, 0.f, 1.f);

//    glEnable(GL_LINE_SMOOTH);
//    glEnable(GL_POLYGON_SMOOTH);
//    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
//    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
//    glEnable(GL_BLEND);

    glEnable(GL_CULL_FACE);

    if (Config::getVal<bool>("MSAA"))
        glEnable(GL_MULTISAMPLE);

//    glDisable(GL_DEPTH_CLAMP);
    glEnable(GL_DEPTH_TEST);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //Use Vsync
//    if(SDL_GL_SetSwapInterval(-1) < 0) {
//        Logger::write(program_log_file_name(),
//                      Category::INITIALIZATION_ERROR,
//                      (format("Warning: Unable to enable VSync. "
//                              "VSync will not be used! SDL Error: %s\n")
//                       % SDL_GetError()).str());
////         If can't use vsync try set fixed fps
//        SDL_GL_SetSwapInterval(0);
//        vsync_supported = false;
//    } else {
//        vsync_supported = true;
//    }

    //Check for error
    error = glGetError();
    if(error != GL_NO_ERROR)
        throw GLException((format("Error initializing OpenGL! %s\n")
                           % gluErrorString(error)).str(),
                          program_log_file_name(),
                          Category::INITIALIZATION_ERROR);
}

void Game::flush()
{
//    glFlush();
    SDL_GL_SwapWindow(Game::m_window);
}

void Game::initGame()
{
    m_world.init();
}

Game::~Game()
{

}