#pragma once
class VertexBuffer;

namespace IcePickRenderer {
	class VertexLayout;
}

class VertexArray {
public:
	VertexArray();
	VertexArray(const VertexArray& other);
	~VertexArray();

	void Destroy();
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