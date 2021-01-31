#include <boost/format.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "systems/renderersystem.hpp"
#include "components/spritecomponent.hpp"
#include "components/cellcomponent.hpp"
#include "render/render.hpp"
#include "utils/logger.hpp"
#include "exceptions/glexception.hpp"
#include "lifeprogram.hpp"

using utils::log::Logger;
using boost::format;
using utils::log::program_log_file_name;
using utils::log::Category;
using glm::mat4;
using glm::vec3;
using glm::scale;

void RendererSystem::drawLevel()
{

    if (GLenum error = glGetError(); error != GL_NO_ERROR)
        throw GLException((format("\n\tRender while drawing level: %1%\n")
                           % glewGetErrorString(error)).str(),
                          program_log_file_name(), Category::INTERNAL_ERROR);
}

void RendererSystem::drawSprites()
{
    auto sprites = getEntitiesByTag<SpriteComponent>();
    auto program = LifeProgram::getInstance();
    for (const auto& [key, en]: sprites) {
        std::shared_ptr<CellComponent> cell;
        if ((cell = en->getComponent<CellComponent>()))
            if (!cell->alive)
                continue;

        const glm::vec3 pos =
                glm::vec3(en->getComponent<PositionComponent>()->x,
                          en->getComponent<PositionComponent>()->y,
                          en->getComponent<PositionComponent>()->z);
        render::drawTexture(*program, *en->getComponent<SpriteComponent>()->sprite,
                            pos, en->getComponent<PositionComponent>()->angle);
    }

    if (GLenum error = glGetError(); error != GL_NO_ERROR)
        throw GLException((format("\n\tRender while drawing sprites: %1%\n")
                           % glewGetErrorString(error)).str(),
                          program_log_file_name(), Category::INTERNAL_ERROR);
}

void RendererSystem::drawText()
{

    if (GLenum error = glGetError(); error != GL_NO_ERROR)
        throw GLException((format("\n\tRender while drawing level: %1%\n")
                           % glewGetErrorString(error)).str(),
                          program_log_file_name(), Category::INTERNAL_ERROR);
}

void RendererSystem::update_state(size_t delta)
{
    drawLevel();
    drawSprites();
    drawText();
}

RendererSystem::RendererSystem()
{

}
