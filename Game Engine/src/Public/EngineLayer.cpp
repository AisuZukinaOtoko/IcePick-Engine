#include "../Render Systems/Renderer.h"
#include "EngineLayer.h"
#include "../LogSystem.h"
#include "EngineAPI.h"
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include "../Utilities/DebugStatistics.h"
#include "../Scene Systems/Components.h"
#include "../Scene Systems/SceneCamera.h"
#include "../Event Systems/EventHandler.h"


IcePick::Input IcePick::EngineLayer::GameInput;

static IcePick::UUID ToneMapperMaterialInstanceId{ IcePick::UUID::Unitialised() };
static IcePick::UUID FrameBufferCopyMaterialInstanceId{ IcePick::UUID::Unitialised() };


namespace IcePick {
	struct DrawCommand {
		glm::mat4 ModelMatrix = glm::mat4(1.0f);
		glm::mat3 NormalMatrix = glm::mat3(1.0f);
		UUID MaterialInstanceId = UUID::Unitialised();
		unsigned int VertexArrayId = 0;
	};
}
static std::vector<IcePick::DrawCommand> NonOpaqueDrawCommandQueue;

void IcePick::EngineLayer::OnAttach() {
	glm::ivec2 windowSize = IcePickRenderer::GetRendererWindowSize();
	IcePickRenderer::TextureSettings colourTextureSettings{ windowSize.x, windowSize.y, IcePickRenderer::TextureSettings::TextureFormat::RGBA16F };
	IcePickRenderer::TextureSettings debugTextureSettings{ windowSize.x, windowSize.y, IcePickRenderer::TextureSettings::TextureFormat::RG32UI };
	IcePickRenderer::TextureSettings depthStencilTextureSettings{ windowSize.x, windowSize.y, IcePickRenderer::TextureSettings::TextureFormat::DEPTH_STENCIL_TEXTURE };
	
	IcePickRenderer::Texture colourTextureOne{ colourTextureSettings };
	IcePickRenderer::Texture colourTextureTwo{ colourTextureSettings };
	IcePickRenderer::Texture debugTexture{ debugTextureSettings };
	IcePickRenderer::Texture depthStencilTexture{ depthStencilTextureSettings };

	m_RenderTextureStorage.push_back(colourTextureOne);
	m_RenderTextureStorage.push_back(colourTextureTwo);
	m_RenderTextureStorage.push_back(debugTexture);
	m_RenderTextureStorage.push_back(depthStencilTexture);

	const unsigned int numTexturesPerFrameBuffer = 2;
	IcePickRenderer::Texture frameBufferOneTextures[numTexturesPerFrameBuffer] = { colourTextureOne, debugTexture };
	IcePickRenderer::Texture frameBufferTwoTextures[numTexturesPerFrameBuffer] = { colourTextureTwo, debugTexture };

	bool transferTextureOwnership = false;
	m_FrameBufferOne.InitWithTargets(frameBufferOneTextures, numTexturesPerFrameBuffer, depthStencilTexture, transferTextureOwnership);
	m_FrameBufferTwo.InitWithTargets(frameBufferTwoTextures, numTexturesPerFrameBuffer, depthStencilTexture, transferTextureOwnership);

	m_ScriptRunner.Init();
	m_AssetLoader.Init();
	m_PhysicsSystem3D.Init();
	m_SceneCameraId = AddSceneCamera();

	// Tone maaping shader setup
	EngineAPI engineAPI{ shared_from_this() };
	ShaderSource shaderSource;
	shaderSource.VertexShaderSource = engineAPI.LoadShaderSourceFile("Game Engine/res/Shaders/fullScreenPass.vert.shader");
	shaderSource.FragmentShaderSource = engineAPI.LoadShaderSourceFile("Game Engine/res/Shaders/pbrNeutralToneMapper.frag.shader");
	UUID toneMapperShaderId = engineAPI.CreateShaderFromSource(shaderSource);

	MaterialBase toneMapperMaterialBase;
	toneMapperMaterialBase.ShaderId = toneMapperShaderId;
	MaterialBaseReadRenderTexture materialBaseSceneTextureData{ MaterialBaseReadRenderTexture::RenderTextureType::COLOUR };
	toneMapperMaterialBase.MaterialReadRenderTextures.push_back(materialBaseSceneTextureData);
	toneMapperMaterialBase.WriteDepthTexture = false;

	MaterialInstance toneMapperMaterialInstance = toneMapperMaterialBase.CreateEmptyInstanceFromBase();

	engineAPI.RegisterMaterialBase(toneMapperMaterialBase);
	ToneMapperMaterialInstanceId = engineAPI.RegisterMaterialInstance(toneMapperMaterialInstance);

	// Copy buffer shader setup
	shaderSource.FragmentShaderSource = engineAPI.LoadShaderSourceFile("Game Engine/res/Shaders/copyFrameTexture.frag.shader");
	UUID copyBufferShaderId = engineAPI.CreateShaderFromSource(shaderSource);

	MaterialBase copyBufferMaterialBase;
	copyBufferMaterialBase.ShaderId = copyBufferShaderId;
	MaterialBaseReadRenderTexture materialBaseActiveTextureData{ MaterialBaseReadRenderTexture::RenderTextureType::ACTIVE_COLOUR };
	copyBufferMaterialBase.MaterialReadRenderTextures.push_back(materialBaseActiveTextureData);
	copyBufferMaterialBase.WriteDepthTexture = false;

	MaterialInstance copyBufferMaterialInstance = copyBufferMaterialBase.CreateEmptyInstanceFromBase();
	engineAPI.RegisterMaterialBase(copyBufferMaterialBase);
	FrameBufferCopyMaterialInstanceId = engineAPI.RegisterMaterialInstance(copyBufferMaterialInstance);
}

void IcePick::EngineLayer::OnUpdate(DeltaTime dt) {
	if (m_CurrentRuntimeState != RuntimeState::RUNNING)
		return;

	auto& ActiveSceneRegistry = GetActiveSceneRegistry();
	auto scriptedEntitiesView = ActiveSceneRegistry.view<ScriptComponent>();

	for (entt::entity entity : scriptedEntitiesView) {
		ScriptComponent& entityScriptComponent = ActiveSceneRegistry.get<ScriptComponent>(entity);

		if (!entityScriptComponent.Active && entityScriptComponent.ScriptId != UUID::Unitialised()) //TODO verify conditional logic
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

	IP_CORE_PROFILE_BEGIN("Physics Update");
	m_PhysicsSystem3D.Update();
	IP_CORE_PROFILE_POP();

	auto physicsEntitiesView = ActiveSceneRegistry.view<RigidBodyComponent>();

	for (entt::entity entity : physicsEntitiesView) {
		TransformComponent& transformComponent = ActiveSceneRegistry.get<TransformComponent>(entity);
		RigidBodyComponent& rigidBodyComponent = ActiveSceneRegistry.get<RigidBodyComponent>(entity);
		
		if (rigidBodyComponent.RigidBodyId.IsInvalid()) {
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
	IcePickRenderer::EnableDepthTesting();
	IcePickRenderer::EnableDepthWrites();

	m_FrameBufferOne.Bind();
	m_FrameBufferOne.ClearColourTarget();
	m_FrameBufferOne.ClearDebugTarget();
	m_FrameBufferOne.ClearDepthTarget();

	m_FrameBufferTwo.Bind();
	m_FrameBufferTwo.ClearColourTarget();
}

void IcePick::EngineLayer::OnPreRender() {
	m_CurrentFrameBuffer = FrameBufferEnum::ONE;
	SetRenderTargetFrameBuffer();
	IcePickRenderer::FrameBuffer& currentFrameBuffer = GetFrameBuffer(m_CurrentFrameBuffer);
	const unsigned int colourAndDebugBufferAttachments = 2;
	currentFrameBuffer.EnableRenderAttachments(colourAndDebugBufferAttachments);
}

void IcePick::EngineLayer::OnEvent(Event& event) {

	if ((event.action == IP_PRESS) && (event.code == IP_KEY_S) && (event.mods & GLFW_MOD_SHIFT)) {
		IP_LOG("Shift-S pressed. Does nothing tho :3");
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
	IcePickRenderer::FrameBuffer& currentFrameBuffer = GetFrameBuffer(m_CurrentFrameBuffer);
	currentFrameBuffer.UnBind();
}

void IcePick::EngineLayer::SetRenderTargetFrameBuffer() {
	IcePickRenderer::FrameBuffer& currentFrameBuffer = GetFrameBuffer(m_CurrentFrameBuffer);
	currentFrameBuffer.Bind();
}

void IcePick::EngineLayer::ReloadShaders() {
	m_AssetLoader.ReloadShaderPrograms();
}

void IcePick::EngineLayer::GetEntityMatPixelData(int x, int y, void* pixelData) {
	IcePickRenderer::FrameBuffer& currentFrameBuffer = GetFrameBuffer(m_CurrentFrameBuffer);
	currentFrameBuffer.GetEntMatPixelData(x, y, pixelData);
}

void IcePick::EngineLayer::OnRender(RenderPayload& payload) {
	IP_CORE_PROFILE_BEGIN("Engine layer render.");	
	IcePickRenderer::SetDrawLineShader(m_AssetLoader.GetDefaultShaderProgram(ShaderLoader::LINE_SHADER));

	m_CurrentScene.OnPreRender();
	RenderEntityMeshes();

#ifndef DIST
	IP_CORE_PROFILE_BEGIN("Debug physics render.");
	if (m_RenderDebugPhysics) {
		m_PhysicsSystem3D.DebugRender();
	}
	IP_CORE_PROFILE_POP();
#endif

	IcePickRenderer::FlushLineRenderBuffer();
	IP_CORE_PROFILE_POP();

	m_CurrentFrameBuffer = FrameBufferEnum::TWO;
	SetRenderTargetFrameBuffer();
	PostProcessPass();

	IcePickRenderer::FrameBuffer& currentFrameBuffer = GetFrameBuffer(m_CurrentFrameBuffer);
	payload.FrameBufferID = currentFrameBuffer.GetAttachmentID(IcePickRenderer::FrameBuffer::COLOUR_TEXTURE);
}

void IcePick::EngineLayer::CopyActiveFrameBufferToDefaultFrameBuffer() {
	SetRenderTargetDefault();
	FullScreenPass(FrameBufferCopyMaterialInstanceId);
	SetRenderTargetFrameBuffer();
}

void IcePick::EngineLayer::PostProcessPass() {
	IcePickRenderer::DisableDepthTesting();
	IcePickRenderer::FrameBuffer& currentFrameBuffer = GetFrameBuffer(m_CurrentFrameBuffer);
	const unsigned int colourBufferAttachment = 1;
	currentFrameBuffer.EnableRenderAttachments(colourBufferAttachment);
	FullScreenPass(ToneMapperMaterialInstanceId);
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

		if (EntityMeshRendererComponent.MeshType == IcePick::ImportSettings::MeshType::SKELETAL_MESH) {
			IcePickRenderer::SkinnedMeshData& skinnedMeshData = m_AssetLoader.GetSkinnedMeshData(EntityMeshRendererComponent);
			Skeleton& meshSkeleton = m_AssetLoader.GetSkeletonById(skinnedMeshData.SkeletonId);
			m_Animator.CalculateSkeletonTransforms(meshSkeleton);
			meshSkeleton.Use();
		}

		model = glm::mat4(1.0f);
		model = glm::translate(model, EntityTransformComponent.Position);
		model *= glm::toMat4(EntityTransformComponent.Rotation);
		model = glm::scale(model, EntityTransformComponent.Scale);
		normalMatrix = glm::mat3(1.0f);
		normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));

		IcePickRenderer::SetRenderWorldNormalMatrix(normalMatrix);
		IcePickRenderer::MeshNode& meshData = m_AssetLoader.GetMeshData(EntityMeshRendererComponent);
		RenderMeshNode(meshData, model, EntityMeshRendererComponent.MaterialSlots, entity);
	}

}

IcePickRenderer::FrameBuffer& IcePick::EngineLayer::GetFrameBuffer(IcePick::EngineLayer::FrameBufferEnum frameBufferEnum) {
	if (frameBufferEnum == FrameBufferEnum::ONE) {
		return m_FrameBufferOne;
	}
	else {
		return m_FrameBufferTwo;
	}
}

void IcePick::EngineLayer::RenderMeshNode(const IcePickRenderer::MeshNode& parent, glm::mat4 parentTransform, const std::vector<UUID>& materialSlots, const entt::entity entityId) {
	glm::mat4 meshWorldTransform = parentTransform * parent.NodeTransform;

	for (unsigned int i = 0; i < parent.VertexArrayIds.size(); i++) {
		IcePickRenderer::VertexArray vertexArray = m_AssetLoader.GetMeshVertexArray(parent.VertexArrayIds[i]);
		unsigned int materialSlotIndex = parent.MaterialSlotIndices[i];

		UUID meshMaterialInstanceId = (materialSlotIndex != -1) ? materialSlots[materialSlotIndex] : UUID::Unitialised();
		const MaterialInstance& meshMaterialInstance = m_AssetLoader.GetMaterialInstance(meshMaterialInstanceId);
		MaterialBase& meshMaterialBase = m_AssetLoader.GetMaterialBase(meshMaterialInstance.MaterialBaseId);

		meshMaterialBase.BindMaterialInstanceParameters(shared_from_this(), meshMaterialInstance);


		ShaderProgram& materialBaseShader = m_AssetLoader.GetShaderProgram(meshMaterialBase.ShaderId);
#ifndef DIST // Editor uniforms
		materialBaseShader.SetUniformUint32("u_EntityId", (uint32_t)entityId);
		materialBaseShader.SetUniformUint32("u_MaterialSlotIndex", (uint32_t)materialSlotIndex);
#endif
		materialBaseShader.Use();
		glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(meshWorldTransform)));
		IcePickRenderer::SetRenderWorldNormalMatrix(normalMatrix);
		IcePickRenderer::DrawMesh(vertexArray, meshWorldTransform, materialBaseShader);
	}

	for (const IcePickRenderer::MeshNode& meshNode : parent.ChildNodes) {
		RenderMeshNode(meshNode, meshWorldTransform, materialSlots, entityId);
	}
}

void IcePick::EngineLayer::ProcessMaterialsAndPipeline(MaterialBase& materialBase, MaterialInstance& materialInstance) {
	EngineAPI engineAPI{ shared_from_this() };
	materialBase.BindMaterialInstanceParameters(engineAPI, materialInstance);

	IcePickRenderer::FrameBuffer& activeFrameBuffer = (m_CurrentFrameBuffer == FrameBufferEnum::ONE) ? m_FrameBufferOne : m_FrameBufferTwo;
	IcePickRenderer::FrameBuffer& backgroundFrameBuffer = (m_CurrentFrameBuffer == FrameBufferEnum::TWO) ? m_FrameBufferOne : m_FrameBufferTwo;

	ShaderProgram materialBaseShader = m_AssetLoader.GetShaderProgram(materialBase.ShaderId);
	for (size_t i{ 0 }; i < materialBase.MaterialReadRenderTextures.size(); i++) {
		auto& readRenderTexture = materialBase.MaterialReadRenderTextures[i];
		switch (readRenderTexture.TextureType) {
		case MaterialBaseReadRenderTexture::RenderTextureType::COLOUR:
		{
			const unsigned int colourTextureId = backgroundFrameBuffer.GetAttachmentID(IcePickRenderer::FrameBuffer::ATTACHMENT::COLOUR_TEXTURE);
			const unsigned int textureSlot = i + (unsigned int)materialBase.MaterialTextures.size();
			IcePickRenderer::Texture::Bind(colourTextureId, textureSlot);
			materialBaseShader.SetUniformInt32("u_SceneTextureSlot", textureSlot);
			break;
		}
		case MaterialBaseReadRenderTexture::RenderTextureType::ACTIVE_COLOUR:
		{
			const unsigned int colourTextureId = activeFrameBuffer.GetAttachmentID(IcePickRenderer::FrameBuffer::ATTACHMENT::COLOUR_TEXTURE);
			const unsigned int textureSlot = i + (unsigned int)materialBase.MaterialTextures.size();
			IcePickRenderer::Texture::Bind(colourTextureId, textureSlot);
			materialBaseShader.SetUniformInt32("u_SceneTextureSlot", textureSlot);
			break;
		}
		case MaterialBaseReadRenderTexture::RenderTextureType::DEPTH_STENCIL:
		{
			IP_ASSERT(!materialBase.WriteDepthTexture, "Reading from and writing to the depth texture simultaneously is undefined behaviour.");
			const unsigned int depthTextureId = activeFrameBuffer.GetDepthTextureID();
			const unsigned int textureSlot = i + (unsigned int)materialBase.MaterialTextures.size();
			IcePickRenderer::Texture::Bind(depthTextureId, textureSlot);
			materialBaseShader.SetUniformInt32("u_DepthTextureSlot", textureSlot);
			break;
		}
		default:
			break;
		}
	}

	if (materialBase.WriteDepthTexture) {
		IcePickRenderer::EnableDepthWrites();
	}
	else {
		IcePickRenderer::DisableDepthWrites();
	}


}

void IcePick::EngineLayer::FullScreenPass(UUID materialInstanceId) {
	MaterialInstance& materialInstance = m_AssetLoader.GetMaterialInstance(materialInstanceId);
	MaterialBase& materialBase = m_AssetLoader.GetMaterialBase(materialInstance.MaterialBaseId);
	ProcessMaterialsAndPipeline(materialBase, materialInstance);

	ShaderProgram& materialBaseShader = m_AssetLoader.GetShaderProgram(materialBase.ShaderId);
	materialBaseShader.Use();

	IcePickRenderer::FullScreenPass();
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
	auto& activeSceneRegistry = GetActiveSceneRegistry();
	auto rigidBodiesView = activeSceneRegistry.view<RigidBodyComponent>();

	for (entt::entity entity : rigidBodiesView) {
		RigidBodyComponent& entityRigidBody = GetComponent<RigidBodyComponent>(entity);
		m_PhysicsSystem3D.MultiRemoveBodyPrepare(entityRigidBody);
	}
	m_PhysicsSystem3D.MultiRemoveBodiesFinalize();
}

void IcePick::EngineLayer::OnDetach() {
	m_AssetLoader.ShutDown();
	m_ScriptRunner.ShutDown();
	m_PhysicsSystem3D.Shutdown();
	DestroyRegistries();

	for (auto& renderTexture : m_RenderTextureStorage) {
		renderTexture.Destroy();
	}
	m_RenderTextureStorage.clear();
}
