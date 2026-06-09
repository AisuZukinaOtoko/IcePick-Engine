#include <iostream>
#include <STB/stb_image.h>
#include "Texture.h"
#include "../LogSystem.h"

namespace IcePickRenderer {
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
		glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_LocalFormat, m_LocalDataType, m_LocalBuffer);
		m_TextureValid = true;
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

	Texture::Texture(const std::string& path) {
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
		m_LocalBuffer = nullptr;
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
		m_LocalBuffer = nullptr;
	}

	Texture::Texture(const TextureSettings& settings) {
		m_Width = settings.Width;
		m_Height = settings.Height;
		m_LocalBuffer = NULL;

		m_LocalFormat = GL_RGBA; // Not important
		switch (settings.Format) {
		case TextureSettings::TextureFormat::RGBA8:
			m_InternalFormat = GL_RGBA8;
			break;
		case TextureSettings::TextureFormat::RGBA16:
			m_InternalFormat = GL_RGBA16;
			break;
		case TextureSettings::TextureFormat::RGBA16F:
			m_InternalFormat = GL_RGBA16F;
			break;
		case TextureSettings::TextureFormat::RG32UI:
			m_InternalFormat = GL_RG32UI;
			m_LocalFormat = GL_RG_INTEGER;
			m_LocalDataType = GL_UNSIGNED_INT;
			break;
		case TextureSettings::TextureFormat::DEPTH_TEXTURE:
			m_InternalFormat = GL_DEPTH_COMPONENT24;
			m_LocalFormat = GL_DEPTH_COMPONENT;
			m_LocalDataType = GL_FLOAT;
			break;
		case TextureSettings::TextureFormat::DEPTH_STENCIL_TEXTURE:
			m_InternalFormat = GL_DEPTH24_STENCIL8;
			m_LocalFormat = GL_DEPTH_STENCIL;
			m_LocalDataType = GL_UNSIGNED_INT_24_8;
			break;
		}

		CreateTextureContext();
		UploadTextureData();
	}

	void Texture::Bind(unsigned int slot = 0) const {
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_ID);
	}

	void Texture::Bind(unsigned int textureId, unsigned int textureSlot) {
		glActiveTexture(GL_TEXTURE0 + textureSlot);
		glBindTexture(GL_TEXTURE_2D, textureId);
	}

	void Texture::Unbind() {
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	bool Texture::IsValid() const {
		return m_TextureValid;
	}

	unsigned int Texture::GetID() const {
		return m_ID;
	}

	void Texture::GetTextureSize(unsigned int* width, unsigned int* height) {
		if (width)
			*width = m_Width;

		if (height)
			*height = m_Height;
	}

	void Texture::Destroy() {
		if (!m_TextureValid)
			return;

		IP_LOG("Deleting texture.", IP_WARN_LOG);
		glDeleteTextures(1, &m_ID);
		m_TextureValid = false;
	}

	Texture::~Texture() {
		if (m_TextureValid) {
			IP_LOG("Texture object destroyed without cleaning up resources.", IP_WARN_LOG);
		}
	}
}
