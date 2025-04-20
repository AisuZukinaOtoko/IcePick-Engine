#include "SceneRegistry.h"
#include "../Utilities/Assert.h"
#include <string>

static entt::registry IP_SceneRegistry;
static int EntityCount = 0;

entt::entity IcePick::NewEntity() {
    entt::entity newEntity = IP_SceneRegistry.create();

#ifndef RELEASE
    std::string newEntityTag = "Object: " + std::to_string(EntityCount);
    IP_SceneRegistry.emplace<TagComponent>(newEntity, newEntityTag);
#else
    IP_ASSERT(false, "Not implemented");
    IP_SceneRegistry.emplace<TagComponent>(newEntity);
#endif
    IP_SceneRegistry.emplace<TransformComponent>(newEntity);
    EntityCount++;
    return newEntity;
}

entt::entity IcePick::FindEntityByTag(TagComponent& tag) {
    auto taggedEntities = IP_SceneRegistry.view<TransformComponent>();

    for (auto entity : taggedEntities) {
        TagComponent& entityTag = IP_SceneRegistry.get<TagComponent>(entity);
        if (entityTag.value == tag.value)
            return entity;
    }

    IP_ASSERT(false, "Entity tag does not exist.");
    return entt::null;
}

entt::registry& IcePick::GetActiveSceneRegistry() {
    return IP_SceneRegistry;
}

void IcePick::DeleteEntity(entt::entity entity) {

}
