#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexLayout.h"
#include "Renderer.h"
#include <iostream>

namespace IcePickRenderer {

	VertexArray::VertexArray(void* vertexData, size_t vertexDataSize, unsigned int* indexBufferData, size_t indexCount, VertexLayout vertexLayout, VertexType vertexType) :
		m_VertexBuffer(vertexData, vertexDataSize, vertexType),
		m_IndexBuffer(indexBufferData, indexCount)
	{
		m_ID = 0;
		Init();
		if (vertexData && vertexDataSize) {
			Bind();
			m_VertexBuffer.InitializeAndUploadData();
			m_VertexBuffer.Bind();
			m_IndexBuffer.Bind();
			AddBuffer(m_VertexBuffer, vertexLayout);
		}		

		Unbind();
		m_IndexBuffer.Unbind();
		m_VertexBuffer.Unbind();
		IndexCount = indexCount;
	}

	void VertexArray::Init() {
		glGenVertexArrays(1, &m_ID);
	}

	VertexArray::VertexArray(VertexArray&& other) noexcept :
		m_VertexBuffer(std::move(other.m_VertexBuffer)),
		m_IndexBuffer(std::move(other.m_IndexBuffer))
	{
		m_ID = other.m_ID;
		IndexCount = other.IndexCount;

		other.m_ID = 0;
		other.IndexCount = 0;
	}

	void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexLayout& layout) {
		Bind();
		vb.Bind();
		const auto& elements = layout.GetElements(); // std::vector<VertexBufferElement>&

		// loop through elements that make up a vertex
		// Each vertex element has an index
		// Specify the attribute for each vertex element
		for (unsigned int i = 0; i < elements.size(); i++) {
			const IcePickRenderer::VertexBufferElement& element = elements[i];
			glEnableVertexAttribArray(i);
			if (element.type == GL_UNSIGNED_INT) { // Integer attributes
				glVertexAttribIPointer(i, element.count, element.type, layout.GetStride(), (const void*)element.offset);
			}
			else {
				glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (const void*)element.offset);
			}
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

	void VertexArray::Destroy() {
		std::cout << "Delete Vertex Array: " << m_ID << std::endl;
		glDeleteVertexArrays(1, &m_ID);
		m_VertexBuffer.Destroy();
		m_IndexBuffer.Destroy();
	}

	VertexArray::~VertexArray() {
	
	}
}
