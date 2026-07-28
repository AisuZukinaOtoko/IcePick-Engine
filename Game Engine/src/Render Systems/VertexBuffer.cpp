#include "VertexBuffer.h"
#include "Renderer.h"
#include "../LogSystem.h"

namespace IcePickRenderer {
    VertexBuffer::VertexBuffer(const void* data, size_t size, VertexType vertexType) {
        if (!data) {
            IP_LOG("Vertex buffer initialized with no data.", IP_WARN_LOG);
            return;
        }
        m_LocalVertexData = malloc(size);

        if (!m_LocalVertexData) {
            IP_LOG("Failed to copy vertex buffer memory.", IP_ERROR_LOG);
            return;
        }

        std::memcpy(m_LocalVertexData, data, size);
        m_LocalVertexDataSize = size;
        m_VertexType = vertexType;
    }

    VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept {
        m_ID = other.m_ID;
        m_LocalVertexData = other.m_LocalVertexData;
        m_LocalVertexDataSize = other.m_LocalVertexDataSize;

        other.m_ID = 0;
        other.m_LocalVertexData = nullptr;
        other.m_LocalVertexDataSize = 0;
    }

    void VertexBuffer::InitializeAndUploadData() {
        if ((m_LocalVertexData == nullptr) || (m_LocalVertexDataSize == 0)) {
            IP_LOG("Cannot initialize a vertex buffer with no data.", IP_ERROR_LOG);
            return;
        }

        glGenBuffers(1, &m_ID);
        glBindBuffer(GL_ARRAY_BUFFER, m_ID);
        glBufferData(GL_ARRAY_BUFFER, m_LocalVertexDataSize, m_LocalVertexData, GL_STATIC_DRAW);
    }

    void VertexBuffer::GetLocalVertexData(void** data, size_t* size) {
        if (data)
            *data = m_LocalVertexData;

        if (size)
            *size = m_LocalVertexDataSize;
    }

    VertexType VertexBuffer::GetVertexType() {
        return m_VertexType;
    }

    void VertexBuffer::Bind() const {
        glBindBuffer(GL_ARRAY_BUFFER, m_ID);
    }

    void VertexBuffer::Unbind() const {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void VertexBuffer::Destroy() {
        if (m_LocalVertexData) {
            free(m_LocalVertexData);
            m_LocalVertexData = nullptr;
            m_LocalVertexDataSize = 0;
        }
        glDeleteBuffers(1, &m_ID);
        m_ID = 0;
    }

    VertexBuffer::~VertexBuffer() {

    }
}