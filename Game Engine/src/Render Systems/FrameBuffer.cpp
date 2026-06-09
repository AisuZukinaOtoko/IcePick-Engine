#include "FrameBuffer.h"
#include "Texture.h"
#include "../LogSystem.h"
#include "../Utilities/Assert.h"
#include <entt/entt.h>
#include <vector>

namespace IcePickRenderer {
	FrameBuffer::FrameBuffer() {
		m_ID = 0;
		m_Width = 0;
		m_Height = 0;
	}

	bool FrameBuffer::InitWithTargets(Texture colourAttachments[], unsigned int attachmentCount, const Texture& depthAttachment, bool transferOwnership) {
		if (m_Initialised)
			return m_Initialised;

		IP_ASSERT(attachmentCount <= IcePickRenderer::FrameBufferMaxColourAttachmentsCount, "Passed in too many colour attachments for frame buffer.");

		glGenFramebuffers(1, &m_ID);
   		glBindFramebuffer(GL_FRAMEBUFFER, m_ID);

		for (unsigned int i = 0; i < attachmentCount; i++) {
			Texture& texture = colourAttachments[i];
			unsigned int textureId = texture.GetID();
			texture.GetTextureSize(&m_Width, &m_Height);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textureId, 0);
			m_ColourAttachmentEnums[i] = GL_COLOR_ATTACHMENT0 + i;
			m_ColourTextureAttachments[i] = texture;
		}

		m_ColourAttachmentCount = attachmentCount;
		m_DepthTextureAttachment = depthAttachment;
		unsigned int depthTextureId = m_DepthTextureAttachment.GetID();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,	GL_TEXTURE_2D, depthTextureId, 0); // TODO: attachment type can be depth or depth_stencil

		glDrawBuffers(attachmentCount, m_ColourAttachmentEnums);
		m_Initialised = true;

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			IP_LOG("Framebuffer is not complete!", IP_ERROR_LOG);
		}

		m_TextureOwnership = transferOwnership;
		return m_Initialised;
	}

	void FrameBuffer::Bind() {
		glViewport(0, 0, m_Width, m_Height);
		glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
	}

	void FrameBuffer::UnBind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::EnableRenderAttachments(unsigned int attachmentCount) {
		IP_ASSERT(attachmentCount <= m_ColourAttachmentCount, "Invalid attachment count.");
		glDrawBuffers(attachmentCount, m_ColourAttachmentEnums);
	}

	void FrameBuffer::ClearColourTarget() {
		GLfloat colourTargetClearColour[4] = { 0.6f, 0.8f, 1.0f, 1.0f };
		glClearBufferfv(GL_COLOR, COLOUR_TEXTURE, colourTargetClearColour);
	}

	void FrameBuffer::ClearDebugTarget() {
		GLuint deubgTargetClearColour[4] = { (unsigned int)entt::null, 0, 0, 0 };
		glClearBufferuiv(GL_COLOR, DEBUG_TEXTURE, deubgTargetClearColour);
	}

	void FrameBuffer::ClearDepthTarget() {
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	unsigned int FrameBuffer::GetID() const {
		return m_ID;
	}

	unsigned int FrameBuffer::GetAttachmentID(ATTACHMENT attachment) const {
		IP_ASSERT(attachment <= m_ColourAttachmentCount, "Invalid attachment.");

		if (attachment >= ATTACHMENT::COLOUR_ATTACHMENT_COUNT)
			return 0;

		return m_ColourTextureAttachments[attachment].GetID();
	}

	unsigned int FrameBuffer::GetDepthTextureID() const {
		return m_DepthTextureAttachment.GetID();
	}

	void FrameBuffer::GetEntMatPixelData(int x, int y, void* pixelData) {
		Bind();
		glReadBuffer(GL_COLOR_ATTACHMENT0 + DEBUG_TEXTURE);
		glReadPixels(x, y, 1, 1, GL_RG_INTEGER, GL_UNSIGNED_INT, pixelData);
		UnBind();
	}

	FrameBuffer::~FrameBuffer() {
		glDeleteFramebuffers(1, &m_ID);

		if (!m_TextureOwnership)
			return;
		
		for (unsigned int i = 0; i < m_ColourAttachmentCount; i++) {
			m_ColourTextureAttachments[i].Destroy();
		}
		m_DepthTextureAttachment.Destroy();
	}

}


