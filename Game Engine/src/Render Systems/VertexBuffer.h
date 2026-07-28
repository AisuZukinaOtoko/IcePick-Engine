#pragma once
#include "Vertex.h"

namespace IcePickRenderer {
	class VertexBuffer {
	public:
		VertexBuffer(const void* data, size_t size, VertexType vertexType);
		VertexBuffer(VertexBuffer&& other) noexcept;
		VertexBuffer(const VertexBuffer& other) = delete;
		VertexBuffer() = delete;
		~VertexBuffer();
		void Bind() const;
		void Unbind() const;
		void InitializeAndUploadData();
		void GetLocalVertexData(void** data, size_t* size);
		VertexType GetVertexType();
		void Destroy();
	private:
		VertexType m_VertexType = STATIC_MESH_VERTEX;
		unsigned int m_ID = 0;
		void* m_LocalVertexData = nullptr;
		size_t m_LocalVertexDataSize = 0;
	};
}