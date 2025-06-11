#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cstddef>
#include "../Utilities/Assert.h"

namespace IcePickRenderer {
	struct VertexBufferElement {
		VertexBufferElement(unsigned int type, unsigned int count, bool normalized) : type(type), count(count), normalized(normalized) {};
		unsigned int type;
		unsigned int count;
		unsigned char normalized;
		//void* offset;

		static unsigned int GetSizeOfType(unsigned int type) {
			switch (type) {
			case GL_FLOAT:			return 4;
			case GL_UNSIGNED_INT:	return 4;
			case GL_UNSIGNED_BYTE:	return 4;
				IP_ASSERT(false, "Unknown type");
				return 0;
			}
		}
	};

	class VertexLayout {
	public:
		VertexLayout()
			: m_Stride(0) {
		};

		template<typename T>
		void Push(unsigned int count) {
			std::runtime_error(false);
		}

		template<>
		void Push<float>(unsigned int count) {
			m_Elements.push_back({ GL_FLOAT, count, GL_FALSE });
			m_Stride += count * sizeof(GLfloat);
		}

		template<>
		void Push<unsigned int>(unsigned int count) {
			m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
			m_Stride += count * sizeof(GLuint);
		}

		template<>
		void Push<unsigned char>(unsigned int count) {
			m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
			m_Stride += count * sizeof(GLbyte);
		}

		inline const std::vector<VertexBufferElement>& GetElements() const { return m_Elements; };
		inline unsigned int GetStride() const { return m_Stride; };
	private:
		unsigned int m_Stride;
		std::vector<VertexBufferElement> m_Elements;
	};
}