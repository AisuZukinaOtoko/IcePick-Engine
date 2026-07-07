#pragma once
#include <vector>

namespace IcePickRenderer {
	class SSBO {
	public:
		SSBO();

		void Init(size_t bufferSize);

		template<typename T>
		void UploadData(std::vector<T>& dataVector) {
			UploadDataInternal(dataVector.data(), dataVector.size() * sizeof(T));
		}

		void Bind(unsigned int location);

		void UnBind();

	private:
		unsigned int m_ID = 0;
		void UploadDataInternal(void* data, size_t dataSize);
	};
}