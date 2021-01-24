#include "systems/animationsystem.hpp"

void AnimationSystem::update_state(size_t delta)
{
    auto entities = getEntities();
    for (auto& [key, en]: entities) {
        en->getComponent<SpriteComponent>()->sprite->setIdx(
                en->getComponent<AnimationComponent>()->cur_state);
    }
}
