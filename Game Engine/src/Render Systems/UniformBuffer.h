#pragma once
#include "Renderer.h"

namespace IcePickRenderer {
	template<typename BufferStruct>
	class UniformBuffer {
	public:
		UniformBuffer();
		void Init();
		void Update(const BufferStruct& data);
		void Bind(unsigned int location);
		void UnBind();
		void Destroy();
		~UniformBuffer();
	private:
		unsigned int m_ID;
	};

	template<typename BufferStruct>
	UniformBuffer<BufferStruct>::UniformBuffer() {
		m_ID = 0;
	}

	template<typename BufferStruct>
	void UniformBuffer<BufferStruct>::Init() {
		glGenBuffers(1, &m_ID);

		glBindBuffer(GL_UNIFORM_BUFFER, m_ID);
		glBufferData(GL_UNIFORM_BUFFER,	sizeof(BufferStruct), nullptr, GL_DYNAMIC_DRAW);

		UnBind();
	}

	template<typename BufferStruct>
	void UniformBuffer<BufferStruct>::Update(const BufferStruct& data) {
		glNamedBufferSubData(m_ID, 0, sizeof(BufferStruct),	&data);
	}

	template<typename BufferStruct>
	void UniformBuffer<BufferStruct>::Bind(unsigned int location) {
		glBindBufferBase(GL_UNIFORM_BUFFER, location, m_ID);
	}

	template<typename BufferStruct>
	void UniformBuffer<BufferStruct>::UnBind() {
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	template<typename BufferStruct>
	void UniformBuffer<BufferStruct>::Destroy() {
		glDeleteBuffers(1, &m_ID);
		m_ID = 0;
	}

	template<typename BufferStruct>
	UniformBuffer<BufferStruct>::~UniformBuffer() {

	}
}