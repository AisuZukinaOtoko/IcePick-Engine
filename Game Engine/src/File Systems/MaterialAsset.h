#pragma once
#include "../Scene Systems/UUID.h"
#include <glm/glm.hpp>
#include <vector>

namespace IcePick {
	class MaterialAsset {
	public:
		enum SampleBits {
			ALBEDO = 0b1, NORMAL = 0b1 << 1,
			ROUGHNESS = 0b1 << 2, METTALIC = 0b1 << 3,
			EMISSIVE = 0b1 << 4
		};

		UUID AlbedoTexture = UUID::Unitialised();
		UUID NormalTexture = UUID::Unitialised();
		UUID RoughnessTexture = UUID::Unitialised();
		UUID MetallicTexture = UUID::Unitialised();
		UUID EmissiveTexture = UUID::Unitialised();

		UUID ShaderID = UUID::Unitialised();
		unsigned int SampleBitmask = 0;
		std::vector<std::pair<std::string, IcePick::UUID>> MaterialTextures;

		glm::vec4 AlbedoColour;
		glm::vec3 SpecularColour;
		float RoughnessValue = 0.0f;
		float MetalValue = 0.0f;
		float EmissiveValue = 0.0f;
	private:

	};
}