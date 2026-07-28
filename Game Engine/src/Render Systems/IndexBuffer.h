#pragma once

namespace IcePickRenderer {
class IndexBuffer {
	public:
		IndexBuffer() = delete;
		IndexBuffer(const unsigned int* data, size_t count);
		IndexBuffer(IndexBuffer&& other) noexcept;
		~IndexBuffer();
		void Bind() const;
		void Unbind() const;
		void Destroy();
		inline size_t GetCount() const { return m_Count; }
	private:
		unsigned int m_ID; //general object
		size_t m_Count; // indices count
	};
}