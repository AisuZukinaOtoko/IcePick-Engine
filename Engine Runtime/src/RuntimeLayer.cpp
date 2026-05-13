#include "RuntimeLayer.h"
#include "Scene Systems/SceneRegistry.h"
#include "Scene Systems/SceneCamera.h"

RuntimeLayer::RuntimeLayer(IcePick::EngineAPI engineAPI) :
	m_EngineAPI(engineAPI)
{
	auto& sceneRegistry = IcePick::GetActiveSceneRegistry();
	auto sceneCameraView = sceneRegistry.view<IcePick::SceneCamera>();
	for (auto sceneCameraEntity : sceneCameraView) {
		IcePick::SceneCamera& sceneCamera = IcePick::GetComponent<IcePick::SceneCamera>(sceneCameraEntity);
		sceneCamera.cameraPosition = glm::vec3(1.5f, 2.25f, 1.5f);
	}
}

void RuntimeLayer::OnAttach() {

}

void RuntimeLayer::OnUpdate(DeltaTime dt) {

}

void RuntimeLayer::OnRender(IcePick::RenderPayload& renderPayload) {
	m_EngineAPI.SetRenderTargetDefault();

	glm::vec3 point1 = glm::vec3(0.0f);
	glm::vec3 point2 = glm::vec3(4.0f);

	m_EngineAPI.RenderLine(point1, point2, glm::vec4(0.8f, 0.0f, 0.5f, 1.0f));
	IcePickRenderer::FlushLineRenderBuffer();
}

void RuntimeLayer::OnDetach() {

}