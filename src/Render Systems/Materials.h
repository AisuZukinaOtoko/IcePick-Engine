#pragma once
#include "glm/glm.hpp"

namespace IcePick {
	struct Material {
		enum SampleBits {
			ALBEDO = 0b1, NORMAL = 0b1 << 1,
			ROUGHNESS = 0b1 << 2, METTALIC = 0b1 << 3,
			EMISSIVE = 0b1 << 4
		};
		unsigned int ShaderID;

		// Maps. Opengl ID of texture.
		unsigned int AlbedoMap;
		unsigned int NormalMap;
		unsigned int RoughnessMap;
		unsigned int MetallicMap;
		unsigned int EmissiveMap;

		// Shader control bitmask
		unsigned int SampleBitmask = 0;

		//glm::vec3 AmbientColour;
		glm::vec3 Albedo;
		glm::vec3 SpecularColour;
		float RoughnessValue = 0.0f;
		float MetalValue = 0.0f;
		float EmissiveValue = 0.0f;
	};

	//returns material index
	unsigned int NewMaterial();
	Material& GetMaterial(unsigned int index);
}