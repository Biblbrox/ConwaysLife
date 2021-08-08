#ifndef UTILS_TEXTURE_HPP
#define UTILS_TEXTURE_HPP

namespace utils::texture {
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
    std::vector <GLfloat> loadObj(const std::string &file,
                                  std::string &textureFile);

    /**
     * Load texture from file
     * Return texture id.
     * textureWidth and textureHeight fields used to store texture size
     * @param file
     * @param textureWidth
     * @param textureHeight
     * @return
     */
    GLuint loadTexture(const std::string &file,
                       GLuint *textureWidth, GLuint *textureHeight);

    /**
     * Return Surface format
     * if surface format can't be recognized 0 will be returned
     * @param surface
     * @return
     */
//    constexpr GLenum getSurfaceFormatInfo(const SDL_Surface &surface) noexcept
//    {
//        GLenum format = 0;
//        GLint color_num = surface.format->BytesPerPixel;
//        if (color_num == 4) {     // contains an alpha channel
//            if (surface.format->Rmask == 0x000000ff)
//                format = GL_RGBA;
//            else
//                format = GL_BGRA;
//        } else if (color_num == 3) {     // no alpha channel
//            if (surface.format->Rmask == 0x000000ff)
//                format = GL_RGB;
//            else
//                format = GL_BGR;
//        }
//
//        return format;
//    }

    /**
    * Flip vertically SDL_Surface
    * @param sfc
    * @return
    */
//    inline SDL_Surface* flipVertically(const SDL_Surface* const sfc)
//    {
//        assert(sfc);
//        SDL_Surface *result =
//                SDL_CreateRGBSurface(sfc->flags, sfc->w, sfc->h,
//                                     sfc->format->BytesPerPixel * 8,
//                                     sfc->format->Rmask,
//                                     sfc->format->Gmask,
//                                     sfc->format->Bmask, sfc->format->Amask);
//         Number of pixels per row
//        const auto pitch = sfc->pitch;
//         Total number of pixels
//        const auto pxlength = pitch * (sfc->h - 1);
//         Right end pixels
//        auto pixels = static_cast<unsigned char *>(sfc->pixels) + pxlength;
//         Left pixels
//        auto rpixels = static_cast<unsigned char *>(result->pixels);
//        for (auto line = 0; line < sfc->h; ++line) {
//            memcpy(rpixels, pixels, pitch);
//            pixels -= pitch;
//            rpixels += pitch;
//        }
//
//        return result;
//    }
}
#endif //UTILS_TEXTURE_HPP
