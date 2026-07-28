#pragma once
#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace IcePickRenderer {
	class VertexLayout;
	class VertexLayout;
	
	class VertexArray {
	public:
		VertexArray() = delete;
		VertexArray(void* vertexData, size_t vertexDataSize, unsigned int* indexBufferData, size_t indexCount, VertexLayout vertexLayout, VertexType vertexType);
		VertexArray(const VertexArray& other) = delete;
		VertexArray(VertexArray&& other) noexcept;
		~VertexArray();

		void Destroy();
		void AddBuffer(const VertexBuffer& vb, const VertexLayout& layout);
		void Bind() const;
		void Unbind() const;
		unsigned int GetID();
		size_t IndexCount = 0;
	private:
		void Init();
		unsigned int m_ID = 0;
		VertexBuffer m_VertexBuffer;
		IndexBuffer m_IndexBuffer;
	};

	struct NewVertexArrayData {
		VertexArray& VA;
		unsigned int VA_Index;
	};
}
