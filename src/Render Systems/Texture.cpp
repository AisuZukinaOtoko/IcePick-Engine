#include <iostream>
#include "Texture.h"
#include "../Vendor/stb_image.h"
#include "../LogSystem.h"

void Texture::CreateTextureContext() {
	glGenTextures(1, &m_ID);
	glBindTexture(GL_TEXTURE_2D, m_ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // image minimize resize setting
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // image enlarge resize setting
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Texture::SetTextureFormats() {
	switch (m_NumChannels) {
	case 1:
		m_LocalFormat = GL_RED;
		m_InternalFormat = GL_R8;
		break;
	case 2:
		m_LocalFormat = GL_RG;
		m_InternalFormat = GL_RG8;
		break;
	case 3:
		m_LocalFormat = GL_RGB;
		m_InternalFormat = GL_RGB8;
		break;
	case 4:
		m_LocalFormat = GL_RGBA;
		m_InternalFormat = GL_RGBA8;
		break;
	default:
		IP_LOG("Invalid number of texture channels.", IP_WARN_LOG);
		break;
	}
}

void Texture::UploadTextureData() {
	glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_LocalFormat, GL_UNSIGNED_BYTE, m_LocalBuffer);
}

Texture::Texture(unsigned char* data, int width, int height, int numChannels) {
	if (!data) {
		IP_LOG("Texture received null data.", IP_ERROR_LOG);
		return;
	}

	m_LocalBuffer = data;
	m_Width = width;
	m_Height = height;
	m_NumChannels = numChannels;
	CreateTextureContext();
	SetTextureFormats();

	UploadTextureData();
	Unbind();
}

Texture::Texture(const std::string& path){
	m_ID = 0;
	m_FilePath = path;
	m_LocalBuffer = nullptr;
	m_Width = m_Height = m_NumChannels = 0;

	stbi_set_flip_vertically_on_load(1);
	m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_NumChannels, 0);

	if (!m_LocalBuffer) {
		std::string errorMsg = "Error loading texture with path: " + path;
		IP_LOG(errorMsg.c_str(), IP_ERROR_LOG);
		return;
	}

	CreateTextureContext();
	SetTextureFormats();

	UploadTextureData();
	Unbind();
	
	stbi_image_free(m_LocalBuffer);
}

Texture::Texture(unsigned char* data, int width) {
	if (!data) {
		IP_LOG("Texture received null data.", IP_ERROR_LOG);
		return;
	}

	m_LocalBuffer = stbi_load_from_memory(data, width, &m_Width, &m_Height, &m_NumChannels, 0);

	if (!m_LocalBuffer) {
		IP_LOG("Error uncompressing texture data.", IP_ERROR_LOG);
		return;
	}

	CreateTextureContext();
	SetTextureFormats();

	UploadTextureData();
	Unbind();

	stbi_image_free(m_LocalBuffer);
}

void Texture::Bind(unsigned int slot = 0) const {
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_ID);
}


void Texture::Unbind(){
	glBindTexture(GL_TEXTURE_2D, 0);
}

int Texture::GetTextureID() {
	return m_TextureID;
}

unsigned int Texture::GetID() const {
	return m_ID;
}

void Texture::Destroy() {
	IP_LOG("Deleting texture.", IP_WARN_LOG);
	glDeleteTextures(1, &m_ID);
}

Texture::~Texture() {

}