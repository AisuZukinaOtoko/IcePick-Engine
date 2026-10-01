#pragma once
#include "Renderer.h"
#include <string>

namespace IcePickRenderer {
	struct TextureSettings {
		unsigned int Width = 0;
		unsigned int Height = 0;
		enum class TextureFormat {
			RGBA8 = 0, SRGBA, RGBA16, R32F, RGBA16F, RG32UI, 
			DEPTH_TEXTURE, DEPTH_STENCIL_TEXTURE
		} Format = TextureFormat::RGBA8;
	};

	class Texture {
	public:
		Texture() : m_TextureValid(false) { }
		// Raw texture data
		Texture(unsigned char* data, int m_width, int m_height, int m_BPP, bool isNonLinearSpace = false);

		// External texture
		Texture(const std::string& path, bool isNonLinearSpace = false);

		// Compressed texture data
		Texture(unsigned char* data, int width, bool isNonLinearSpace = false);

		// Empty textures. Mainly as render targets
		Texture(const TextureSettings& settings);

		// Raw texture data
		Texture(void* data, const TextureSettings& settings);

		Texture(const Texture& other) = default;
		~Texture();

		void Bind(unsigned int slot /*= 0*/) const;
		static void Bind(unsigned int textureId, unsigned int textureSlot);
		void Unbind();
		bool IsValid() const;
		void Destroy();

		// ID of the opengl texture
		unsigned int GetID() const;

		void GetTextureSize(unsigned int* width, unsigned int* height);

	private:
		void SetTextureSettings(const TextureSettings& settings);
		void CreateTextureContext();
		void SetTextureFormats();
		void UploadTextureData();

		void* m_LocalBuffer = nullptr; // points to invalid memory after texture is created
		int m_Width = 0;
		int m_Height = 0;
		int m_NumChannels = 0; // bytes per pixel

		unsigned int m_ID = 0;
		bool m_IsNonLinearSpace = false;
		GLuint m_InternalFormat = GL_RGBA8;
		GLenum m_LocalFormat = GL_RGBA;
		GLenum m_LocalDataType = GL_UNSIGNED_BYTE;

		bool m_TextureValid = false;
	};
}
