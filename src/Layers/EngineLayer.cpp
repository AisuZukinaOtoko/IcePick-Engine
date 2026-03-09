#include "../Render Systems/Renderer.h"
#include "EngineLayer.h"
#include "../LogSystem.h"
#include "EngineAPI.h"
#include "glm/gtc/matrix_transform.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"
#include "../Utilities/DebugStatistics.h"
#include "../Scene Systems/SceneCamera.h"


IcePick::Input IcePick::EngineLayer::GameInput;

void IcePick::EngineLayer::OnAttach() {
	glm::ivec2 windowSize = IcePickRenderer::GetRendererWindowSize();
	m_FrameBuffer.Init(windowSize.x, windowSize.y, FrameBuffer::DEFERRED);
	m_ScriptRunner.Init();
	m_AssetLoader.Init();
	m_PhysicsSystem3D.Init();
	m_SceneCameraId = AddSceneCamera();
	//m_CurrentScene.OnBegin();
}

void IcePick::EngineLayer::OnUpdate(DeltaTime dt) {
	if (m_CurrentRuntimeState != RuntimeState::RUNNING)
		return;

	auto& ActiveSceneRegistry = GetActiveSceneRegistry();
	auto scriptedEntitiesView = ActiveSceneRegistry.view<ScriptComponent>();

	for (entt::entity entity : scriptedEntitiesView) {
		ScriptComponent& entityScriptComponent = ActiveSceneRegistry.get<ScriptComponent>(entity);

		if (!entityScriptComponent.Active && entityScriptComponent.ScriptId != UUID::Unitialised())
			continue;

		Script& entityScript = m_ScriptRunner.GetScriptById(entityScriptComponent.ScriptId);

		if (!entityScript.IsValid)
			continue;

		sol::protected_function_result result = entityScript.OnUpdateFunction(entityScriptComponent.Self, dt.GetDelta());
		if (!result.valid()) {
			sol::error err = result;
			IP_LOG(err.what(), IP_ERROR_LOG);
			entityScriptComponent.Active = false;
		}
	}

	//m_CurrentScene.OnUpdate(dt);
	IP_CORE_PROFILE_BEGIN("Physics Update");
	m_PhysicsSystem3D.Update();
	IP_CORE_PROFILE_POP();

	auto physicsEntitiesView = ActiveSceneRegistry.view<RigidBodyComponent>();

	for (entt::entity entity : physicsEntitiesView) {
		TransformComponent& transformComponent = ActiveSceneRegistry.get<TransformComponent>(entity);
		RigidBodyComponent& rigidBodyComponent = ActiveSceneRegistry.get<RigidBodyComponent>(entity);
		
		if (rigidBodyComponent.RigidBodyId.IsInvalid()) {
			//IP_LOG("Rigid body ID is invalid.", IP_WARN_LOG);
			continue;
		}

		transformComponent.Position = m_PhysicsSystem3D.GetBodyPosition(rigidBodyComponent.RigidBodyId);
		transformComponent.Rotation = m_PhysicsSystem3D.GetBodyRotation(rigidBodyComponent.RigidBodyId);
	}

	SceneCamera& sceneCamera = GetComponent<SceneCamera>(m_SceneCameraId);
	sceneCamera.OnUpdate(GameInput, dt);
	IcePickRenderer::SetRenderCameraWorldPosition(sceneCamera.cameraPosition);
	IcePickRenderer::SetRenderViewProjectionMatrix(sceneCamera.GetViewProjectionMatrix());
}

void IcePick::EngineLayer::OnNewFrame() {
	m_FrameBuffer.Clear();
}

void IcePick::EngineLayer::OnPreRender() {

}

void IcePick::EngineLayer::OnEvent(Event& event) {

	if ((event.action == IP_PRESS) && (event.code == IP_KEY_S) && (event.mods & GLFW_MOD_SHIFT)) {
		IP_LOG("Shift-S pressed. Does nothing tho :)");
		event.flags |= IP_EVENT_HANDLED;
	}

	GameInput.OnEvent(event);
}

unsigned int IcePick::EngineLayer::CreateTexture(std::filesystem::path texturePath) {
	return m_AssetLoader.LoadTexture(texturePath);
}

unsigned int IcePick::EngineLayer::GetTextureRenderId(UUID textureId) {
	return m_AssetLoader.GetTextureRenderId(textureId);
}

IcePick::UUID IcePick::EngineLayer::LoadTextureFromAsset(std::filesystem::path assetPath) {
	return m_AssetLoader.LoadTextureFromAsset(assetPath);
}

void IcePick::EngineLayer::SetRenderTargetDefault() {
	m_FrameBuffer.UnBind();
}

void IcePick::EngineLayer::SetRenderTargetFrameBuffer() {
	m_FrameBuffer.Bind();
}

void IcePick::EngineLayer::ReloadShaders() {
	m_AssetLoader.ReloadShaderPrograms();
}

void IcePick::EngineLayer::GetEntityMatPixelData(int x, int y, void* pixelData) {
	m_FrameBuffer.GetEntMatPixelData(x, y, pixelData);
}

void IcePick::EngineLayer::OnRender(RenderPayload& payload) {
	IP_CORE_PROFILE_BEGIN("Engine layer render.");
	SetRenderTargetFrameBuffer();
	m_CurrentScene.OnPreRender();
	payload.FrameBufferID = m_FrameBuffer.GetColourTextureID();
	RenderEntityMeshes();
	IcePickRenderer::FlushLineRenderBuffer(m_AssetLoader.GetDefaultShaderProgram(ShaderLoader::LINE_SHADER));
	IP_CORE_PROFILE_POP();
}

void IcePick::EngineLayer::RenderEntityMeshes() {
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat3 normalMatrix = glm::mat3(1.0f);

	auto& ActiveSceneRegistry = GetActiveSceneRegistry();
	auto sceneView = ActiveSceneRegistry.view<MeshRendererComponent>();

	for (entt::entity entity: sceneView) {
		MeshRendererComponent& EntityMeshRendererComponent = ActiveSceneRegistry.get<MeshRendererComponent>(entity);
		TransformComponent& EntityTransformComponent = ActiveSceneRegistry.get<TransformComponent>(entity);

		if (!EntityMeshRendererComponent.MeshVisible)
			continue;

		if (!EntityMeshRendererComponent.MeshLoaded && !EntityMeshRendererComponent.MeshFilePath.empty()) {
			EntityMeshRendererComponent = m_AssetLoader.LoadMesh(EntityMeshRendererComponent.MeshFilePath);
		}

		model = glm::mat4(1.0f);
		model = glm::translate(model, EntityTransformComponent.Position);
		//glm::quat q = glm::quat(glm::radians(EntityTransformComponent.Rotation));
		//model *= glm::toMat4(q);
		model *= glm::toMat4(EntityTransformComponent.Rotation);
		model = glm::scale(model, EntityTransformComponent.Scale);
		normalMatrix = glm::mat3(1.0f);
		normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));

		IcePickRenderer::SetRenderWorldNormalMatrix(normalMatrix);
		RenderMeshNode(EntityMeshRendererComponent.RootMeshNode, model, EntityMeshRendererComponent.MaterialSlots, entity);
	}

}

void IcePick::EngineLayer::RenderMeshNode(const MeshNode& parent, glm::mat4 parentTransform, const std::vector<UUID>& materialSlots, const entt::entity entityId) {
	glm::mat4 meshWorldTransform = parentTransform * parent.NodeTransform;
	for (unsigned int i = 0; i < parent.VertexArrayIDs.size(); i++) {
		unsigned int vertexArrayID = parent.VertexArrayIDs[i];
		unsigned int materialSlotIndex = parent.MaterialSlotIndices[i];
		MeshComponent mesh = { vertexArrayID, -1, MeshComponent::STATIC };

		UUID meshMaterialInstanceId = (materialSlotIndex != -1) ? materialSlots[materialSlotIndex] : UUID::Unitialised();
		const MaterialInstance& meshMaterialInstance = m_AssetLoader.GetMaterialInstance(meshMaterialInstanceId);
		MaterialBase& meshMaterialBase = m_AssetLoader.GetMaterialBase(meshMaterialInstance.MaterialBaseId);

		meshMaterialBase.BindMaterialInstanceParameters(shared_from_this(), meshMaterialInstance);


		ShaderProgram& materialBaseShader = m_AssetLoader.GetShaderProgram(meshMaterialBase.ShaderId);
		materialBaseShader.SetUniformUint32("u_EntityId", (uint32_t)entityId);
		materialBaseShader.SetUniformUint32("u_MaterialSlotIndex", (uint32_t)materialSlotIndex);

		materialBaseShader.Use();
		glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(meshWorldTransform)));
		IcePickRenderer::SetRenderWorldNormalMatrix(normalMatrix);
		IcePickRenderer::DrawMesh(mesh, meshWorldTransform, materialBaseShader);
	}

	for (const MeshNode& meshNode : parent.Children) {
		RenderMeshNode(meshNode, meshWorldTransform, materialSlots, entityId);
	}
}

void IcePick::EngineLayer::OnBeginScene() {
	auto& activeSceneRegistry = GetActiveSceneRegistry();
	auto rigidBodiesView = activeSceneRegistry.view<RigidBodyComponent>();
	
	for (entt::entity entity : rigidBodiesView) {
		TransformComponent& entityTransform = GetComponent<TransformComponent>(entity);
		RigidBodyComponent& entityRigidBody = GetComponent<RigidBodyComponent>(entity);
		m_PhysicsSystem3D.MultiCreateAndAddBodyPrepare(entityTransform, entityRigidBody);
	}
	m_PhysicsSystem3D.MultiAddBodiesFinalize();
}

void IcePick::EngineLayer::OnEndScene() {

}

void IcePick::EngineLayer::OnDetach() {
	m_AssetLoader.ShutDown();
	m_ScriptRunner.ShutDown();
	m_PhysicsSystem3D.Shutdown();
	auto& SceneRegistry = GetActiveSceneRegistry();
	SceneRegistry.clear();
}
