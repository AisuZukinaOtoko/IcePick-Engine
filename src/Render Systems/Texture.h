#pragma once
#include "Renderer.h"

class Texture{
public:
	Texture(const std::string& path);
	~Texture();

	void Bind(unsigned int slot /*= 0*/) const;
	void Unbind();
private:
	unsigned int m_ID;
	std::string m_filePath;
	unsigned char* m_localBuffer;
	int m_width, m_height, m_BPP;
};