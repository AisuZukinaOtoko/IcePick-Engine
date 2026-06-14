#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cstddef>
#include <stdexcept>

namespace IcePickRenderer {
	struct VertexBufferElement {
		VertexBufferElement(unsigned int type, unsigned int count, bool normalized, size_t offset) 
			: type(type), count(count), normalized(normalized), offset(offset) {};
		unsigned int type;
		unsigned int count;
		unsigned char normalized;
		size_t offset;
	};

	class VertexLayout {
	public:
		VertexLayout() = default;
		VertexLayout(unsigned int stride)
			: m_Stride(stride) {
		};

		template<typename T>
		void Push(unsigned int count, size_t offset) {
			std::runtime_error(false);
		}

		template<>
		void Push<float>(unsigned int count, size_t offset) {
			m_Elements.push_back({ GL_FLOAT, count, GL_FALSE, offset });
		}

		template<>
		void Push<unsigned int>(unsigned int count, size_t offset) {
			m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE, offset });
		}

		template<>
		void Push<unsigned char>(unsigned int count, size_t offset) {
			m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE, offset});
		}

		inline const std::vector<VertexBufferElement>& GetElements() const { return m_Elements; };
		inline unsigned int GetStride() const { return m_Stride; };
	private:
		unsigned int m_Stride;
		std::vector<VertexBufferElement> m_Elements;
	};
}