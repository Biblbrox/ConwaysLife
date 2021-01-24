#ifndef MOONLANDER_MOONLANDERPROGRAM_HPP
#define MOONLANDER_MOONLANDERPROGRAM_HPP

#include <glm/mat4x4.hpp>
#include <memory>

#include "render/shaderprogram.hpp"

class LifeProgram: public ShaderProgram
{
protected:
    static std::shared_ptr<LifeProgram> instance;
public:
    LifeProgram();

    //TODO: fix public constructor and make_shared problem
    //TODO: fix ugly design
    static std::shared_ptr<LifeProgram> getInstance()
    {
        if (!instance)
            instance = std::make_shared<LifeProgram>();

        return instance;
    }

    ~LifeProgram() override;

    void setTexture(int texture);

    /**
     * Update uniform variables locations
     */
    void rebindUniforms();

    /**
     * Write specific uniform to current program
     */
    void updateProjection() override;
    void updateView() override;
    void updateModel() override;

    /**
     * Init programs
     */
    void loadProgram() override;

private:
    glm::vec4 m_color;

    GLint m_texLoc;
    GLint m_colorLoc;

    GLuint m_vertexShader = 0;
    GLuint m_fragmentShader = 0;

    GLuint m_matricesUBO;
    GLuint m_textureDataUBO;

    int m_isTextureRender;

    /**
     * Utility cleanup functions
     */
    void remove_programs();
    void remove_shaders();
    void free_buffers();
};

#endif //MOONLANDER_MOONLANDERPROGRAM_HPP
