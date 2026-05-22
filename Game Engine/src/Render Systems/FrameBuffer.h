#pragma once

namespace IcePickRenderer {
	class Texture;

	class FrameBuffer {
	public:
		enum ATTACHMENT {
			COLOUR_TEXTURE = 0,
			NORMAL_TEXTURE,
			ENTITY_MAT_TEXTURE,
			ATTACHMENT_COUNT,
		};

		enum Type {
			DEFERRED = 0,
			FORWARD
		};

		FrameBuffer();
		~FrameBuffer();
		bool Init(int width, int height, Type type);
		bool Init(Texture colourAttachments[], unsigned int attachmentCount, const Texture& depthAttachment);
		void Bind();
		void UnBind();
		void Clear();
		unsigned int GetID() const;
		unsigned int GetColourTextureID() const;
		unsigned int GetAttachmentID(ATTACHMENT attachment);
		unsigned int GetDepthTextureID() const;

		void GetEntMatPixelData(int x, int y, void* pixelData);
	private:
		Type m_Type;
		unsigned int m_Width, m_Height;
		unsigned int m_AttachmentCount = 0;
		unsigned int m_ID = 0;
		unsigned int m_AttachmentIDs[ATTACHMENT_COUNT];
		unsigned int m_DepthTexID = 0;
		bool m_DepthTestEnabled = false;
		bool m_Initialised = false;
	};
}
