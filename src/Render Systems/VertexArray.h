#pragma once

class VertexBuffer;
class VertexBufferLayout;

class VertexArray {
public:
	VertexArray();
	VertexArray(const VertexArray& other);
	~VertexArray();

	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);
	void Bind() const;
	void Unbind() const;
	unsigned int IndexCount = 0;
private:
	unsigned int m_ID;
};