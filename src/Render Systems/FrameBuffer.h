#pragma once

// Serves as the target for rendering
class FrameBuffer {
public:
	enum Type {
		COLOUR = 0, DEPTH, STENCIL
	};

	FrameBuffer();
	~FrameBuffer();
	bool Init();
	void Bind();
	void UnBind();
	unsigned int GetID() const;
	unsigned int GetColourTextureID() const;
private:
	unsigned int m_ID = 0;
	unsigned int m_TextureID = 0;
	unsigned int m_ColourTexID = 0;
	unsigned int m_DepthTexID = 0;
	bool m_DepthTestEnabled = false;
	Type m_Type;
};