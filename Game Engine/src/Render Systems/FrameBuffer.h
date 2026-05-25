#pragma once
#include "Texture.h"

namespace IcePickRenderer {
	constexpr unsigned int FrameBufferMaxColourAttachmentsCount = 2;

	class Texture;

	class FrameBuffer {
	public:
		enum ATTACHMENT {
			COLOUR_TEXTURE = 0,
			DEBUG_TEXTURE,
			COLOUR_ATTACHMENT_COUNT,
		};

		FrameBuffer();
		~FrameBuffer();
		bool InitWithTargets(Texture colourAttachments[], unsigned int attachmentCount, const Texture& depthAttachment, bool transferOwnership);
		void Bind();
		void UnBind();

		// Call Bind First
		void ClearColourTarget();
		void ClearDepthTarget();
		void ClearDebugTarget();

		unsigned int GetID() const;
		
		unsigned int GetAttachmentID(ATTACHMENT attachment) const;
		unsigned int GetDepthTextureID() const;

		void GetEntMatPixelData(int x, int y, void* pixelData);
	private:
		unsigned int m_Width, m_Height;
		unsigned int m_ID = 0;
		bool m_Initialised = false;

		// True when the FrameBuffer object has ownership of the texture targets.
		// This object will be responsible for freein the texture resources.
		unsigned int m_ColourAttachmentCount = 0;
		bool m_TextureOwnership = false;
		Texture m_ColourTextureAttachments[FrameBufferMaxColourAttachmentsCount];
		Texture m_DepthTextureAttachment;
	};
}
