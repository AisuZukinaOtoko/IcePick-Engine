#include "Renderer.h"
#include "SSBO.h"
#include "../Utilities/Assert.h"

namespace IcePickRenderer {
	SSBO::SSBO() {

	}

	void SSBO::Init(size_t bufferSize) {
		glCreateBuffers(1, &m_ID);
		glNamedBufferStorage(m_ID, bufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
		m_MaxBufferSize = bufferSize;
	}
	
	void SSBO::UploadDataInternal(void* data, size_t dataSize) {
		IP_ASSERT(m_MaxBufferSize >= dataSize, "Data size excedes allocated size of SSBO.");
		glNamedBufferSubData(m_ID, 0, dataSize,	data);
	}

	void SSBO::Bind(unsigned int location) {
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, location, m_ID);
	}

	void SSBO::UnBind() {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
}