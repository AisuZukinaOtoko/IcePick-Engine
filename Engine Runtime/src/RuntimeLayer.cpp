#include "RuntimeLayer.h"
#include "Scene Systems/SceneRegistry.h"
#include "Scene Systems/SceneCamera.h"
#include "Scene Systems/Components.h"
#include <glm/gtc/matrix_transform.hpp>

RuntimeLayer::RuntimeLayer(IcePick::EngineAPI engineAPI) :
	m_EngineAPI(engineAPI)
{
	auto& sceneRegistry = IcePick::GetActiveSceneRegistry();
	auto sceneCameraView = sceneRegistry.view<IcePick::SceneCamera>();
	for (auto sceneCameraEntity : sceneCameraView) {
		IcePick::SceneCamera& sceneCamera = IcePick::GetComponent<IcePick::SceneCamera>(sceneCameraEntity);
		sceneCamera.cameraPosition = glm::vec3(1.5f, 2.25f, 1.5f);
		sceneCamera.cameraFront = glm::vec3(0.0f);
	}
}

void RuntimeLayer::OnAttach() {
	//entt::entity meshEntity = IcePick::NewEntity();
	//IcePick::MeshRendererComponent mesh = m_EngineAPI.LoadMesh("Game Engine/res/Assets/cube.obj");
	//IcePick::AddComponent<IcePick::MeshRendererComponent>(meshEntity, mesh);

	m_EngineAPI.SetEngineRuntimeState(IcePick::RuntimeState::RUNNING);
}

void RuntimeLayer::OnUpdate(DeltaTime dt) {

}

void RuntimeLayer::OnRender(IcePick::RenderPayload& renderPayload) {
	m_EngineAPI.SetRenderTargetDefault();
	IcePick::MeshRendererComponent mesh = m_EngineAPI.LoadMesh("Game Engine/res/Assets/cube.obj");
	glm::mat4 identityMatrix = glm::mat4(1.0f);
	m_EngineAPI.RenderMesh(mesh, identityMatrix, entt::null);

	//glm::vec3 point1 = glm::vec3(0.0f);
	//glm::vec3 point2 = glm::vec3(4.0f);

	//m_EngineAPI.RenderLine(point1, point2, glm::vec4(0.8f, 0.0f, 0.5f, 1.0f));
	//IcePickRenderer::FlushLineRenderBuffer();
}

void RuntimeLayer::OnDetach() {

}