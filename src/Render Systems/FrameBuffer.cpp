#include "FrameBuffer.h"
#include "Renderer.h"
#include "../LogSystem.h"

using namespace IcePick;

FrameBuffer::FrameBuffer() {
	m_Type = Type::COLOUR;
	m_ID = 0;
}

FrameBuffer::~FrameBuffer() {
	glDeleteFramebuffers(1, &m_ID);
}

bool FrameBuffer::Init() {
	glGenFramebuffers(1, &m_ID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
	glm::ivec2 windowSize = IcePickRenderer::GetRendererWindowSize();

	// Attachments
	switch (m_Type) {
	case Type::COLOUR:
		glGenTextures(1, &m_ColourTexID);
		glBindTexture(GL_TEXTURE_2D, m_ColourTexID);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowSize.x, windowSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColourTexID, 0);

		// Depth render buffer attachment. Memory optimizations. Can't be sampled.
		glGenRenderbuffers(1, &m_DepthTexID);
		glBindRenderbuffer(GL_RENDERBUFFER, m_DepthTexID);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowSize.x, windowSize.y);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_DepthTexID);

		// create standard depth texture. Can be sampled
		/*glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, Renderer::WindowWidth, Renderer::WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_TextureID, 0);
		glBindTexture(GL_TEXTURE_2D, 0);*/
		break;
	case Type::DEPTH:
		//GLuint depthTex;
		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, windowSize.x, windowSize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_TextureID, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		break;
	case Type::STENCIL:
		break;
	default:
		break;
	}
	// Create frame buffer textures
	/*glGenTextures(1, &m_TextureID);
	glBindTexture(GL_TEXTURE_2D, m_TextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sceneWidth, sceneHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);*/

	//GLuint colorTex;
	/*glGenTextures(1, &m_TextureID);
	glBindTexture(GL_TEXTURE_2D, m_TextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Renderer::WindowWidth, Renderer::WindowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureID, 0);*/


	

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		IP_LOG("Framebuffer is not complete!", IP_ERROR_LOG);
		//return false;
	}

	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS); // default depth test is GL_LESS
	//m_DepthTestEnabled = true;


	

	// Set viewport to match framebuffer size
	glViewport(0, 0, windowSize.x, windowSize.y);

	// Render your scene here...

	// Bind the default framebuffer when done
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

void FrameBuffer::Bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
}

void FrameBuffer::UnBind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int FrameBuffer::GetID() const {
	return m_ID;
}

unsigned int FrameBuffer::GetColourTextureID() const {
	return m_ColourTexID;
}

