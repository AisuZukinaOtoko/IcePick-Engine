#include "SceneRegistry.h"
#include "SceneCamera.h"
#include "../Utilities/Assert.h"
#include <string>

static entt::registry IP_SceneRegistry;
static int DefaultTagCount = 0;

static IcePick::TagComponent GetDefaultTag() {
    IcePick::TagComponent returnTag{};
    returnTag.value = "Object: " + std::to_string(DefaultTagCount);
    DefaultTagCount++;
    return returnTag;
}

entt::entity IcePick::NewEntity() {
    entt::entity newEntity = IP_SceneRegistry.create();

    AddComponent<TagComponent>(newEntity, GetDefaultTag());
    AddComponent<TransformComponent>(newEntity);    
    return newEntity;
}

entt::entity IcePick::NewCameraController() {
    entt::entity newEntity = IP_SceneRegistry.create();

    TagComponent tag = GetDefaultTag();
    tag.Type = TagComponent::EntityType::CAMERA_CONTROLLER;
    AddComponent<TagComponent>(newEntity, tag);

    AddComponent<CameraControllerComponent>(newEntity);
    return newEntity;
}

entt::entity IcePick::NewPointLight() {
    entt::entity newEntity = IP_SceneRegistry.create();

    TagComponent tag = GetDefaultTag();
    tag.Type = TagComponent::EntityType::POINT_LIGHT;
    AddComponent<TagComponent>(newEntity, tag);

    AddComponent<TransformComponent>(newEntity);
    AddComponent<PointLightComponent>(newEntity);
    return newEntity;
}

entt::entity IcePick::NewDirectionalLight() {
    entt::entity newEntity = IP_SceneRegistry.create();
    
    TagComponent tag = GetDefaultTag();
    tag.Type = TagComponent::EntityType::DIRECTIONAL_LIGHT;
    AddComponent<TagComponent>(newEntity, tag);

    AddComponent<DirectionalLightComponent>(newEntity);
    return newEntity;
}

entt::entity IcePick::NewTerrain() {
    entt::entity newEntity = IP_SceneRegistry.create();

    TagComponent tag = GetDefaultTag();
    tag.Type = TagComponent::EntityType::TERRAIN;
    AddComponent<TagComponent>(newEntity, tag);

    return newEntity;
}

entt::entity IcePick::AddSceneCamera() {
    entt::entity newEntity = IP_SceneRegistry.create();

    TagComponent tag;
    tag.value = "Scene Camera";
    tag.Type = TagComponent::EntityType::CAMERA;
    AddComponent<TagComponent>(newEntity, tag);

    AddComponent<SceneCamera>(newEntity);
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
    IP_SceneRegistry.destroy(entity);
}
