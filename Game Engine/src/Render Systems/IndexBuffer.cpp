#include "IndexBuffer.h"
#include "Renderer.h"

namespace IcePickRenderer {
    IndexBuffer::IndexBuffer(const unsigned int* data, size_t count) :m_Count(count) {
        glGenBuffers(1, &m_ID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), data, GL_STATIC_DRAW);
    }

    IndexBuffer::IndexBuffer(IndexBuffer&& other) noexcept {
        m_ID = other.m_ID;
        m_Count = other.m_Count;

        other.m_ID = 0;
        other.m_Count = 0;
    }


    void IndexBuffer::Bind() const {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
    }

    void IndexBuffer::Unbind() const {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void IndexBuffer::Destroy() {
        glDeleteBuffers(1, &m_ID);
        m_ID = 0;
        m_Count = 0;
    }

    IndexBuffer::~IndexBuffer() {
        //std::cout << "Delete Index Buffer" << std::endl;
        //glDeleteBuffers(1, &m_ID);
    }
}