#pragma once
#include "Renderer.h"

class Texture{
public:
	Texture(unsigned char* data, int m_width, int m_height, int m_BPP);
	Texture(const std::string& path);
	~Texture();

	void Bind(unsigned int slot /*= 0*/) const;
	void Unbind();

	// ID of the texture asset
	int GetTextureID();

	// ID of the opengl texture
	unsigned int GetID();
	
private:
	int m_TextureID = -1;
	unsigned int m_ID;
	std::string m_filePath;
	unsigned char* m_localBuffer;
	int m_width, m_height, m_BPP;
};