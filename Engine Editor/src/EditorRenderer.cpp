#include "EditorRenderer.h"

EditorRenderer::EditorRenderer(IcePick::EngineAPI engineAPI) :
	m_EngineAPI(engineAPI)
{

}

void EditorRenderer::Init(unsigned int width, unsigned int height) {
	IcePickRenderer::TextureSettings colourTextureSettings{ width, height, IcePickRenderer::TextureSettings::TextureFormat::RGBA16 };
	IcePickRenderer::TextureSettings depthStencilTextureSettings{ width, height, IcePickRenderer::TextureSettings::TextureFormat::DEPTH_STENCIL_TEXTURE };

	IcePickRenderer::Texture colourTextureOne{ colourTextureSettings };
	IcePickRenderer::Texture depthStencilTexture{ depthStencilTextureSettings };

	const unsigned int numTexturesPerFrameBuffer = 1;
	IcePickRenderer::Texture frameBufferOneTextures[numTexturesPerFrameBuffer] = { colourTextureOne };

	bool transferTextureOwnership = true;
	m_FrameBuffer.InitWithTargets(frameBufferOneTextures, numTexturesPerFrameBuffer, depthStencilTexture, transferTextureOwnership);
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
	m_FrameBuffer.Bind();
	m_FrameBuffer.ClearColourTarget();
	m_FrameBuffer.ClearDepthTarget();
}

void EditorRenderer::Use() {
	m_FrameBuffer.Bind();
}

void EditorRenderer::UnBind() {
	m_FrameBuffer.UnBind();
}

unsigned int EditorRenderer::GetRenderTexture() {
	return m_FrameBuffer.GetAttachmentID(IcePickRenderer::FrameBuffer::COLOUR_TEXTURE);
}

void EditorRenderer::Destroy() {

}

EditorRenderer::~EditorRenderer() {

}
