#ifndef UTILS_H
#define UTILS_H

#include <GL/glew.h>
#include <string>
#include <glm/vec2.hpp>
#include <typeinfo>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <SDL.h>
#include <SDL_video.h>

#include "render/camera.hpp"
#include "constants.hpp"

using glm::vec2;

namespace utils
{
    struct Position
    {
        GLfloat x;
        GLfloat y;
        GLfloat z;
        GLfloat angle;
    };

    struct Rect
    {
        GLfloat x;
        GLfloat y;
        GLfloat z;
        GLfloat w;
        GLfloat h;
        GLfloat d;
    };

    struct RectPoints
    {
        vec2 a;
        vec2 b;
        vec2 c;
        vec2 d;
    };

    /**
     * Split long string to multiple lines.
     * @param str
     * @return
     */
    inline std::vector<std::string> split_to_lines(const std::string& str)
    {
        std::vector<std::string> res;
        boost::split(res, str, boost::is_any_of("\n"), boost::token_compress_on);
        return res;
    }

    /**
     * Return corrected cords. It's needed because when camera
     * moves all non-static objects moves also.
     * @param coords
     * @param camPos
     * @return
     */
    constexpr glm::vec2 fix_coords(const vec2& coords, const vec2& camPos)
    {
        return {coords.x + camPos.x, coords.y + camPos.y};
    }

    /**
     * Return Surface format
     * if surface format can't be recognized 0 will be returned
     * @param surface
     * @return
     */
    constexpr GLenum getSurfaceFormatInfo(const SDL_Surface &surface) noexcept
    {
        GLenum format = 0;
        GLint color_num = surface.format->BytesPerPixel;
        if (color_num == 4) {     // contains an alpha channel
            if (surface.format->Rmask == 0x000000ff)
                format = GL_RGBA;
            else
                format = GL_BGRA;
        } else if (color_num == 3) {     // no alpha channel
            if (surface.format->Rmask == 0x000000ff)
                format = GL_RGB;
            else
                format = GL_BGR;
        }

        return format;
    }

    template<typename T>
    constexpr size_t type_id() noexcept
    {
        return typeid(T).hash_code();
    }

    /**
     * Divide clip by multiple small clips.
     * Number of clips = num_x * num_y.
     * @param clip
     * @param num_x
     * @param num_y
     * @return
     */
    template <int num_x, int num_y>
    std::array<Rect, num_x * num_y>
    constexpr generate_clips(Rect clip)
    {
        GLfloat part_width = clip.w / num_x;
        GLfloat part_height = clip.h / num_y;

        std::array<Rect, num_x * num_y> clips;
        size_t i = 0;
        for (GLfloat y = clip.y; y < clip.y + clip.h; y += part_height)
            for (GLfloat x = clip.x; x < clip.x + clip.w; x += part_width)
                clips[++i] = {x, y, part_width, part_height};

        return clips;
    }

    constexpr GLfloat units(const GLfloat val)
    {
        return val * gl::unitSize;
    }

    namespace physics
    {
        /**
         * Return altitude at (x, y). Can be used also to get y coordinate
         * of altitude y at point x.
         * @param points
         * @param x
         * @param y
         * @return
         */
        GLfloat altitude(const std::vector<vec2>& points, GLfloat x, GLfloat y);
    }

    /**
     * Return full path to resource fileName
     * @param fileName
     * @return
     */
    inline std::string getResourcePath(const std::string &fileName)
    {
        return std::string(RESOURCE_PATH + fileName);
    }

    /**
     * Return full path to shader fileName
     * @param fileName
     * @return
     */
    inline std::string getShaderPath(const std::string &fileName)
    {
        return std::string(SHADER_PATH + fileName);
    }

    /**
     * Add padding to line
     * @param line
     * @param pad
     */
    void padLine(std::string& line, size_t pad);

    /**
     * Load opengl texture from pixels to GPU with specific format.
     * Result texture has RGBA format.
     * If function can't load texture exception will be thrown.
     * @param pixels
     * @param width
     * @param height
     * @param texture_format
     * @return textureID
     */
    GLuint
    loadTextureFromPixels32(const GLuint *pixels, GLuint width, GLuint height,
                            GLenum textureType = GL_RGBA);

    /**
     * Parse obj file
     * Return vector with such elements in row:
     * <ver.x, ver.y, ver.z, uv.x, uv.y>
     * @param file
     * @return
     */
    std::vector<GLfloat> loadObj(const std::string& file,
                                 std::string& textureFile);

    /**
     * Load texture from file
     * Return texture id.
     * textureWidth and textureHeight fields used to store texture size
     * @param file
     * @param textureWidth
     * @param textureHeight
     * @return
     */
    GLuint loadTexture(const std::string& file,
                       GLuint* textureWidth, GLuint* textureHeight);

    /**
    * Load shader from file by specific path
    * shaderType param may of the supported shader types
    * If shader can't be loaded (file not found or bad read access)
    * or can't be compiled std::runtime_error exception will be thrown.
    * @param path
    * @param shaderType
    * @return
    */
    GLuint loadShaderFromFile(const std::string &path, GLenum shaderType);

    /**
     * Both functions getScreenWidth and getScreenHeight must
     * be called after SDL2 initialization. Otherwise 0 will be returned
     * @tparam T
     * @return
     */
    template<typename T>
    inline T getScreenWidth() noexcept
    {
        static_assert(std::is_arithmetic_v<T>, "Template parameter "
                                               "must be arithmetical");
        if (SDL_WasInit(SDL_INIT_VIDEO) != SDL_INIT_VIDEO)
            return 0;

        SDL_DisplayMode dm;
        SDL_GetCurrentDisplayMode(0, &dm);
        return dm.w;
    }

    template<typename T>
    inline T getScreenHeight() noexcept
    {
        static_assert(std::is_arithmetic_v<T>, "Template parameter "
                                               "must be arithmetical");

        if (SDL_WasInit(SDL_INIT_VIDEO) != SDL_INIT_VIDEO)
            return 0;

        SDL_DisplayMode dm;
        SDL_GetCurrentDisplayMode(0, &dm);
        return dm.h;
    }

    /**
     * Flip vertically SDL_Surface
     * @param sfc
     * @return
     */
    inline SDL_Surface* flipVertically(const SDL_Surface* const sfc)
    {
        assert(sfc);
        SDL_Surface *result =
                SDL_CreateRGBSurface(sfc->flags, sfc->w, sfc->h,
                                     sfc->format->BytesPerPixel * 8,
                                     sfc->format->Rmask,
                                     sfc->format->Gmask,
                                     sfc->format->Bmask, sfc->format->Amask);
        // Number of pixels per row
        const auto pitch = sfc->pitch;
        // Total number of pixels
        const auto pxlength = pitch * (sfc->h - 1);
        // Right end pixels
        auto pixels = static_cast<unsigned char *>(sfc->pixels) + pxlength;
        // Left pixels
        auto rpixels = static_cast<unsigned char *>(result->pixels);
        for (auto line = 0; line < sfc->h; ++line) {
            memcpy(rpixels, pixels, pitch);
            pixels -= pitch;
            rpixels += pitch;
        }

        return result;
    }
}

#endif //UTILS_H
