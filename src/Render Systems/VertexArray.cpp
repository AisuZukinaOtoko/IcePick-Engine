#include "VertexArray.h"
#include "VertexBuffer.h"
#include "Renderer.h"
#include "Texture.h"

VertexArray::VertexArray() {
	glGenVertexArrays(1, &m_ID);
}

VertexArray::VertexArray(const VertexArray& other) {
	m_ID = other.m_ID;
	this->IndexCount = other.IndexCount;
}

VertexArray::~VertexArray() {
	std::cout << "Delete Vertex Array: " << m_ID << std::endl;
	glDeleteVertexArrays(1, &m_ID);
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const IcePickRenderer::VertexLayout& layout) {
	Bind();
	vb.Bind();
	const auto& elements = layout.GetElements(); // std::vector<VertexBufferElement>&
	unsigned int offset = 0;

	// loop through elements that make up a vertex
	// Each vertex element has an index
	// Specify the attribute for each vertex element
	for (unsigned int i = 0; i < elements.size(); i++) {
		const IcePickRenderer::VertexBufferElement& element = elements[i];
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (const void*)offset);
		offset += element.count * IcePickRenderer::VertexBufferElement::GetSizeOfType(element.type);
	}
}

void VertexArray::Bind() const {
	glBindVertexArray(m_ID);
}

void VertexArray::Unbind() const {
	glBindVertexArray(0);
}

unsigned int VertexArray::GetID() {
	return m_ID;
}

