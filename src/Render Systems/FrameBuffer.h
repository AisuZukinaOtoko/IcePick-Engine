#pragma once

// Serves as the target for rendering
class FrameBuffer {
public:
	enum ATTACHMENT {
		COLOUR_TEXTURE = 0,
		NORMAL_TEXTURE,
		ENTITY_MAT_TEXTURE,
		ATTACHMENT_COUNT,
	};

	FrameBuffer();
	~FrameBuffer();
	bool Init(int width, int height);
	void Bind();
	void UnBind();
	void Clear();
	unsigned int GetID() const;
	unsigned int GetColourTextureID() const;
	unsigned int GetAttachmentID(ATTACHMENT attachment);
	unsigned int GetDepthTextureID() const;

	void GetEntMatPixelData(int x, int y, void* pixelData);
private:
	int m_Width, m_Height;
	unsigned int m_ID = 0;
	unsigned int m_AttachmentIDs[ATTACHMENT_COUNT];
	unsigned int m_DepthTexID = 0;
	bool m_DepthTestEnabled = false;
};