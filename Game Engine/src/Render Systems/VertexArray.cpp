#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexLayout.h"
#include "Renderer.h"
#include <iostream>

namespace IcePickRenderer {

	VertexArray::VertexArray() {
		m_ID = 0;
	}

	void VertexArray::Init() {
		glGenVertexArrays(1, &m_ID);
	}

	VertexArray::VertexArray(const VertexArray& other) {
		m_ID = other.m_ID;
		this->IndexCount = other.IndexCount;
	}

	VertexArray::~VertexArray() {
	
	}

	void VertexArray::Destroy() {
		std::cout << "Delete Vertex Array: " << m_ID << std::endl;
		glDeleteVertexArrays(1, &m_ID);
	}

	void VertexArray::AddBuffer(const VertexBuffer& vb, const IcePickRenderer::VertexLayout& layout) {
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

}
