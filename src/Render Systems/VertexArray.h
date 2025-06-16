#pragma once
#include "VertexLayout.h"
class VertexBuffer;

class VertexArray {
public:
	VertexArray();
	VertexArray(const VertexArray& other);
	~VertexArray();

	void AddBuffer(const VertexBuffer& vb, const IcePickRenderer::VertexLayout& layout);
	void Bind() const;
	void Unbind() const;
	unsigned int GetID();
	unsigned int IndexCount = 0;
private:
	unsigned int m_ID;
};

struct NewVertexArrayData {
	VertexArray& VA;
	unsigned int VA_Index;
};