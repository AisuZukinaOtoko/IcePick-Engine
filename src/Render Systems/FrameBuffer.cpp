#include "FrameBuffer.h"
#include "Renderer.h"
#include "../LogSystem.h"
#include "entt/entt.h"

using namespace IcePick;

FrameBuffer::FrameBuffer() {
	m_ID = 0;
	m_Width = 0;
	m_Height = 0;
	m_Type = FORWARD;
}

FrameBuffer::~FrameBuffer() {
	glDeleteFramebuffers(1, &m_ID);
	glDeleteTextures(m_AttachmentCount, m_AttachmentIDs);
}

bool FrameBuffer::Init(int width, int height, Type type) {
	glGenFramebuffers(1, &m_ID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
	m_Width = width;
	m_Height = height;
	m_Type = type;
	m_AttachmentCount = (m_Type == DEFERRED) ? ATTACHMENT_COUNT : 1;

	glGenTextures(m_AttachmentCount, m_AttachmentIDs);

	// Colour texture
	glBindTexture(GL_TEXTURE_2D, m_AttachmentIDs[COLOUR_TEXTURE]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (unsigned int)COLOUR_TEXTURE, GL_TEXTURE_2D, m_AttachmentIDs[COLOUR_TEXTURE], 0);

	if (m_Type == DEFERRED) {
		// Normal texture
		glBindTexture(GL_TEXTURE_2D, m_AttachmentIDs[NORMAL_TEXTURE]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (unsigned int)NORMAL_TEXTURE, GL_TEXTURE_2D, m_AttachmentIDs[NORMAL_TEXTURE], 0);

		// Entity and material slot texture
		glBindTexture(GL_TEXTURE_2D, m_AttachmentIDs[ENTITY_MAT_TEXTURE]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32UI, m_Width, m_Height, 0, GL_RG_INTEGER, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (unsigned int)ENTITY_MAT_TEXTURE, GL_TEXTURE_2D, m_AttachmentIDs[ENTITY_MAT_TEXTURE], 0);
	}	
	
	// Depth render buffer attachment. Memory optimizations. Can't be sampled.
	glGenRenderbuffers(1, &m_DepthTexID);
	glBindRenderbuffer(GL_RENDERBUFFER, m_DepthTexID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_DepthTexID);

	GLenum attachments[ATTACHMENT_COUNT] = {
		GL_COLOR_ATTACHMENT0 + COLOUR_TEXTURE,
		GL_COLOR_ATTACHMENT0 + NORMAL_TEXTURE,
		GL_COLOR_ATTACHMENT0 + ENTITY_MAT_TEXTURE,
	};
	glDrawBuffers(m_AttachmentCount, attachments);


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		IP_LOG("Framebuffer is not complete!", IP_ERROR_LOG);
	}

	glViewport(0, 0, m_Width, m_Height);

	return true;
}

void FrameBuffer::Bind() {
	glViewport(0, 0, m_Width, m_Height);
	glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
}

void FrameBuffer::UnBind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Clear() {
	Bind();
	glClear(GL_DEPTH_BUFFER_BIT);

	GLfloat colourClearColour[4] = { 0.6f, 0.8f, 1.0f, 1.0f };
	GLfloat normalClearColour[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	GLuint entityMatClearColour[4] = { (unsigned int)entt::null, 0, 0, 0};

	glClearBufferfv(GL_COLOR, COLOUR_TEXTURE, colourClearColour);
	if (m_Type == DEFERRED) {
		glClearBufferfv(GL_COLOR, NORMAL_TEXTURE, normalClearColour);
		glClearBufferuiv(GL_COLOR, ENTITY_MAT_TEXTURE, entityMatClearColour);
	}
	UnBind();
	return;
}

unsigned int FrameBuffer::GetID() const {
	return m_ID;
}

unsigned int FrameBuffer::GetColourTextureID() const {
	return m_AttachmentIDs[COLOUR_TEXTURE];
}

unsigned int FrameBuffer::GetAttachmentID(ATTACHMENT attachment) {
	IP_ASSERT(attachment >= m_AttachmentCount, "Invalid attachment.");
	return m_AttachmentIDs[attachment];
}

unsigned int FrameBuffer::GetDepthTextureID() const {
	return m_DepthTexID;
}

void FrameBuffer::GetEntMatPixelData(int x, int y, void* pixelData) {
	if (m_Type == FORWARD)
		return;

	Bind();
	glReadBuffer(GL_COLOR_ATTACHMENT0 + ENTITY_MAT_TEXTURE);
	glReadPixels(x, y, 1, 1, GL_RG_INTEGER, GL_UNSIGNED_INT, pixelData);
	UnBind();
}

