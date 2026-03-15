#include "SceneRegistry.h"
#include "SceneCamera.h"
#include "../Utilities/Assert.h"
#include <string>

static entt::registry IP_SceneRegistry;
#ifndef DIST
static entt::registry IP_TemporaryRegistry; // Can be swapped to by editor for temporary simulation
#endif

static IcePick::SceneRegistryTypes CurrentRegistryType = IcePick::SceneRegistryTypes::DEFAULT;
static int DefaultTagCount = 0;

static IcePick::TagComponent GetDefaultTag() {
    IcePick::TagComponent returnTag{};
    returnTag.value = "Object: " + std::to_string(DefaultTagCount);
    DefaultTagCount++;
    return returnTag;
}

entt::entity IcePick::NewEntity() {
    entt::registry& activeRegistry = GetActiveSceneRegistry();
    entt::entity newEntity = activeRegistry.create();

    AddComponent<TagComponent>(newEntity, GetDefaultTag());
    AddComponent<TransformComponent>(newEntity);    
    return newEntity;
}

entt::entity IcePick::NewCameraController() {
    entt::registry& activeRegistry = GetActiveSceneRegistry();
    entt::entity newEntity = activeRegistry.create();

    TagComponent tag = GetDefaultTag();
    tag.Type = TagComponent::EntityType::CAMERA_CONTROLLER;
    AddComponent<TagComponent>(newEntity, tag);

    AddComponent<CameraControllerComponent>(newEntity);
    return newEntity;
}

entt::entity IcePick::NewPointLight() {
    entt::registry& activeRegistry = GetActiveSceneRegistry();
    entt::entity newEntity = activeRegistry.create();

    TagComponent tag = GetDefaultTag();
    tag.Type = TagComponent::EntityType::POINT_LIGHT;
    AddComponent<TagComponent>(newEntity, tag);

    AddComponent<TransformComponent>(newEntity);
    AddComponent<PointLightComponent>(newEntity);
    return newEntity;
}

entt::entity IcePick::NewDirectionalLight() {
    entt::registry& activeRegistry = GetActiveSceneRegistry();
    entt::entity newEntity = activeRegistry.create();
    
    TagComponent tag = GetDefaultTag();
    tag.Type = TagComponent::EntityType::DIRECTIONAL_LIGHT;
    AddComponent<TagComponent>(newEntity, tag);

    AddComponent<DirectionalLightComponent>(newEntity);
    return newEntity;
}

entt::entity IcePick::NewTerrain() {
    entt::registry& activeRegistry = GetActiveSceneRegistry();
    entt::entity newEntity = activeRegistry.create();

    TagComponent tag = GetDefaultTag();
    tag.Type = TagComponent::EntityType::TERRAIN;
    AddComponent<TagComponent>(newEntity, tag);

    return newEntity;
}

entt::entity IcePick::AddSceneCamera() {
    entt::registry& activeRegistry = GetActiveSceneRegistry();
    entt::entity newEntity = activeRegistry.create();

    TagComponent tag;
    tag.value = "Scene Camera";
    tag.Type = TagComponent::EntityType::CAMERA;
    AddComponent<TagComponent>(newEntity, tag);

    AddComponent<SceneCamera>(newEntity);
    return newEntity;
}

//entt::entity IcePick::DuplicateEntity(entt::entity sourceEntity) {
//    entt::registry& activeRegistry = GetActiveSceneRegistry();
//    if (!activeRegistry.valid(sourceEntity))
//        return entt::null;
//
//    entt::entity duplicatedEntity = activeRegistry.create();
//
//
//    return entt::entity();
//}

entt::entity IcePick::FindEntityByTag(TagComponent& tag) {
    entt::registry& activeRegistry = GetActiveSceneRegistry();
    auto taggedEntities = activeRegistry.view<TransformComponent>();

    for (auto entity : taggedEntities) {
        TagComponent& entityTag = activeRegistry.get<TagComponent>(entity);
        if (entityTag.value == tag.value)
            return entity;
    }

    IP_ASSERT(false, "Entity tag does not exist.");
    return entt::null;
}

entt::registry& IcePick::GetActiveSceneRegistry() {
#ifndef DIST
    if (CurrentRegistryType == SceneRegistryTypes::DEFAULT)
        return IP_SceneRegistry;

    return IP_TemporaryRegistry;
#else
    return IP_SceneRegistry;
#endif
}

void IcePick::SetActiveSceneRegistry(SceneRegistryTypes registryType) {
    CurrentRegistryType = registryType;
}

void IcePick::DeleteEntity(entt::entity entity) {
    entt::registry& activeRegistry = GetActiveSceneRegistry();
    activeRegistry.destroy(entity);
}
