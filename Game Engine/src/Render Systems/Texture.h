#pragma once
#include "Renderer.h"
#include <string>

class Texture{
public:
	// Raw texture data
	Texture(unsigned char* data, int m_width, int m_height, int m_BPP);

	// External texture
	Texture(const std::string& path);

	// Compressed texture data
	Texture(unsigned char* data, int width);
	~Texture();

	void Bind(unsigned int slot /*= 0*/) const;
	void Unbind();
	bool IsValid() const;
	void Destroy();

	// ID of the opengl texture
	unsigned int GetID() const;
	
private:
	void CreateTextureContext();
	void SetTextureFormats();
	void UploadTextureData();
	unsigned int m_ID = 0;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer = nullptr; // points to invalid memory after texture is created
	int m_Width = 0;
	int m_Height = 0;
	int m_NumChannels = 0; // bytes per pixel
	GLuint m_InternalFormat = GL_RGBA8;
	GLuint m_LocalFormat = GL_RGBA;
	bool m_TextureValid = false;
};