#include "RuntimeLayer.h"
#include "Scene Systems/SceneRegistry.h"
#include "Scene Systems/SceneCamera.h"
#include "Scene Systems/Components.h"
#include <glm/gtc/matrix_transform.hpp>

RuntimeLayer::RuntimeLayer(IcePick::EngineAPI engineAPI) :
	m_EngineAPI(engineAPI)
{
	
}

void RuntimeLayer::OnAttach() {
	auto& sceneRegistry = IcePick::GetActiveSceneRegistry();
	auto sceneCameraView = sceneRegistry.view<IcePick::SceneCamera>();
	for (auto sceneCameraEntity : sceneCameraView) {
		IcePick::SceneCamera& sceneCamera = IcePick::GetComponent<IcePick::SceneCamera>(sceneCameraEntity);
		sceneCamera.cameraPosition = glm::vec3(1.5f, 2.25f, 1.5f);
		sceneCamera.cameraFront = glm::vec3(0.0f);
	}

	entt::entity tempEntity = IcePick::NewEntity();
	IcePick::MeshRendererComponent mesh = m_EngineAPI.LoadMesh("Game Engine/res/Assets/cube.obj");
	IcePick::AddComponent<IcePick::MeshRendererComponent>(tempEntity, mesh);

	m_EngineAPI.SetEngineRuntimeState(IcePick::RuntimeState::RUNNING);
}

void RuntimeLayer::OnUpdate(DeltaTime dt) {

}

void RuntimeLayer::OnRender(IcePick::RenderPayload& renderPayload) {
	m_EngineAPI.CopyFrameBufferResultToDefaultBuffer();
}

void RuntimeLayer::OnDetach() {

}