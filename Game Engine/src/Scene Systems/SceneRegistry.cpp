#include "SceneRegistry.h"
#include "Components.h"
#include "SceneCamera.h"
#include "../Utilities/Assert.h"
#include "../LogSystem.h"
#include <string>

static entt::registry IP_SceneRegistry;
static entt::registry IP_PrefabRegistry;
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

template<typename... Component>
static void CopyComponentAcrossRegistries(entt::registry& srcRegistry, entt::entity srcEntity, entt::registry& destRegistry, entt::entity destEntity) {
    if (!srcRegistry.valid(srcEntity) || !destRegistry.valid(destEntity)) {
        IP_ASSERT(false, "Invalid entity.");
        return;
    }

    (
        [&] {
            if (srcRegistry.all_of<Component>(srcEntity)) {
                destRegistry.emplace<Component>(destEntity, srcRegistry.get<Component>(srcEntity));
            }
        }
        (), ...
    );
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
    return GetSceneRegistry(CurrentRegistryType);
}

entt::registry& IcePick::GetSceneRegistry(SceneRegistryTypes registryType) {
#ifndef DIST
    if (registryType == SceneRegistryTypes::DEFAULT)
        return IP_SceneRegistry;

    return IP_TemporaryRegistry;
#else
    return IP_SceneRegistry;
#endif
}

void IcePick::SetActiveSceneRegistry(SceneRegistryTypes registryType) {
    CurrentRegistryType = registryType;
}

entt::registry& IcePick::GetActivePrefabRegistry() {
    return IP_PrefabRegistry;
}

entt::entity IcePick::InstantiatePrefab(entt::entity prefabId) {
    entt::registry& sceneRegistry = GetActiveSceneRegistry();
    entt::registry& prefabRegistry = GetActivePrefabRegistry();

    if (!prefabRegistry.valid(prefabId)) {
        IP_LOG("Cannot instantiate invalid prefab.", IP_ERROR_LOG);
        return entt::null;
    }

    entt::entity prefabInstance = sceneRegistry.create();
    CopyComponentAcrossRegistries<
        TagComponent,
        TransformComponent,
        MeshRendererComponent,
        ScriptComponent,
        RigidBodyComponent
    >(prefabRegistry, prefabId, sceneRegistry, prefabInstance);

    return prefabInstance;
}

void IcePick::DuplicateSceneRegistry(entt::registry& sourceRegistry, entt::registry& targetRegistry) {
    targetRegistry.clear();    

    for (auto sourceEntity : sourceRegistry.storage<entt::entity>()) {
        entt::entity targetEntity = targetRegistry.create(sourceEntity);
        IP_ASSERT(sourceEntity == targetEntity, "Entity IDs do not match.");

        CopyComponentAcrossRegistries<
            TagComponent,
            MeshRendererComponent,
            ScriptComponent,
            TransformComponent,
            PointLightComponent,
            DirectionalLightComponent,
            CameraControllerComponent,
            RigidBodyComponent,
            SceneCamera
        >(sourceRegistry, sourceEntity, targetRegistry, targetEntity);
        
    }
}

void IcePick::DeleteEntity(entt::entity entity) {
    entt::registry& activeRegistry = GetActiveSceneRegistry();
    activeRegistry.destroy(entity);
}

void IcePick::DestroyRegistries() {
    IP_PrefabRegistry.clear();
    IP_SceneRegistry.clear();
#ifndef DIST
    IP_TemporaryRegistry.clear();
#endif
}
