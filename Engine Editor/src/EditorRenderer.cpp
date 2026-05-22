#include "EditorRenderer.h"

EditorRenderer::EditorRenderer(IcePick::EngineAPI engineAPI) :
	m_EngineAPI(engineAPI)
{

}

void EditorRenderer::Init(unsigned int width, unsigned int height) {
	m_FrameBuffer.Init(width, height, IcePickRenderer::FrameBuffer::FORWARD);
}

void EditorRenderer::RenderMesh(IcePick::MeshRendererComponent& mesh, glm::mat4& modelMatrix) {
	m_FrameBuffer.Bind();

	glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
	IcePickRenderer::SetRenderWorldNormalMatrix(normalMatrix);
	IcePickRenderer::SetRenderViewProjectionMatrix(editorCamera.GetViewProjectionMatrix());
	IcePickRenderer::SetRenderCameraWorldPosition(editorCamera.cameraPosition);

	m_EngineAPI.RenderMesh(mesh, modelMatrix, entt::null);
	m_FrameBuffer.UnBind();
}

void EditorRenderer::Clear() {
	m_FrameBuffer.Clear();
}

void EditorRenderer::Use() {
	m_FrameBuffer.Bind();
}

void EditorRenderer::UnBind() {
	m_FrameBuffer.UnBind();
}

unsigned int EditorRenderer::GetRenderTexture() {
	return m_FrameBuffer.GetColourTextureID();
}

void EditorRenderer::Destroy() {

}

EditorRenderer::~EditorRenderer() {

}
