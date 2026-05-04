#pragma once
#include "Public/EngineAPI.h"
#include "Viewport/EditorCamera.h"

class EditorRenderer {
public:
	EditorRenderer() = delete;
	EditorRenderer(IcePick::EngineAPI engineAPI);
	void Init(unsigned int width, unsigned int height);
	void RenderMesh(IcePick::MeshRendererComponent& mesh, glm::mat4& modelMatrix);
	void Clear();

	void Use();
	void UnBind();
	unsigned int GetRenderTexture();
	void Destroy();
	~EditorRenderer();
	EditorCamera editorCamera;
private:
	IcePick::EngineAPI m_EngineAPI;
	FrameBuffer m_FrameBuffer;
};