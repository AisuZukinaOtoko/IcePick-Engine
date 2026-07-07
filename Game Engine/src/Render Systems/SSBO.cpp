#include "Renderer.h"
#include "SSBO.h"

namespace IcePickRenderer {
	SSBO::SSBO() {

	}

	void SSBO::Init(size_t bufferSize) {
		glCreateBuffers(1, &m_ID);
		glNamedBufferStorage(m_ID, bufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
	}
	
	void SSBO::UploadDataInternal(void* data, size_t dataSize) {
		glNamedBufferSubData(m_ID, 0, dataSize,	data);
	}

	void SSBO::Bind(unsigned int location) {
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, location, m_ID);
	}

	void SSBO::UnBind() {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
}