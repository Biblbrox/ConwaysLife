cmake_minimum_required(VERSION 3.16)

set(GAME_NAME "LifeGame")
project(${GAME_NAME})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake_modules/")

set(CMAKE_CXX_FLAGS_DEBUG_INIT "-D_FORTIFY_SOURCE=2" "-D_GLIBCXX_ASSERTIONS"
        "-fasynchronous-unwind-tables" "-fexceptions" "-fpie" "-Wl,-pie" "-fpic"
        "-shared" "-fstack-clash-protection" "-fstack-protector"
        "-fstack-protector-strong" "-g" "-grecord-gcc-switches" "-fcf-protection"
        "-pipe" "-Wall" "-Werror=format-security" "-Wl,-z,defs" "-Wl,-z,now"
        "-O0" "-Wl,-z,relro" "-std=c++17" "-fno-inline-functions")

set(CMAKE_CXX_FLAGS_RELEASE_INIT "-fasynchronous-unwind-tables" "-fexceptions"
        "-fpie" "-Wl,-pie" "-fpic" "-shared" "-fstack-clash-protection"
        "-fstack-protector" "-fstack-protector-strong" "-fcf-protection" "-pipe"
        "-Wall" "-Werror=format-security" "-Wl,-z,defs" "-Wl,-z,now" "-Wl,-z,relro"
        "-O3" "-std=c++17")

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)
find_package(Boost REQUIRED)

set(EXT_PATH "cmake_modules/")

add_library(imgui
        # Main Imgui files
        ${EXT_PATH}/imgui/imgui.cpp ${EXT_PATH}/imgui/imgui_draw.cpp
        ${EXT_PATH}/imgui/imgui_tables.cpp ${EXT_PATH}/imgui/imgui_widgets.cpp
        # SDL2+OpenGL-specific files
        ${EXT_PATH}/imgui/backends/imgui_impl_sdl.cpp
        ${EXT_PATH}/imgui/backends/imgui_impl_opengl3.cpp)

# Make SDL2 available to Imgui
target_include_directories(imgui PUBLIC ${SDL2_INCLUDE_DIRS})
# imgui/backends/ contains the SDL implementation
target_include_directories(imgui PUBLIC ${EXT_PATH}/imgui/ ${EXT_PATH}/imgui/backends/)
# Configure Imgui to use OpengGL through SDL2
target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_CUSTOM=<SDL_opengl.h>  GL_GLEXT_PROTOTYPES=1)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIR}
        ${SDL2_TTF_INCLUDE_DIR} ${GLEW_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS}
        ${GLM_INCLUDE_DIRS} ${Boost_INCLUDE_DIR})

set(CMAKE_CXX_STANDARD 20)
FILE(GLOB_RECURSE CPP RELATIVE ${CMAKE_SOURCE_DIR} "src/*.cpp")
FILE(GLOB_RECURSE HPP RELATIVE ${CMAKE_SOURCE_DIR} "include/*.hpp" "include/*.h")

set(SOURCES ${HPP} ${CPP})
set(SDL2_TTF_LIBRARIES SDL2_ttf)

add_executable(${GAME_NAME} ${SOURCES})
target_link_libraries(${GAME_NAME} ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES}
        ${SDL2_TTF_LIBRARIES} ${SDL2_MIXER_LIBRARIES} ${Boost_LIBRARIES} GLEW
        libGLEW.so libGLU.so libGL.so imgui)

target_include_directories(${GAME_NAME} PRIVATE include)
