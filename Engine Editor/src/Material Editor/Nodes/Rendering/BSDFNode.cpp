#include "BSDFNode.h"

BSDFNode::BSDFNode() {
	InputPins.emplace_back(Pin::VEC4, "Albedo");
	InputPins.emplace_back(Pin::FLOAT32, "Shininess");
	//InputPins.emplace_back(Pin::VEC3, "Normal");
	//InputPins.emplace_back(Pin::FLOAT32, "Roughness");
	//InputPins.emplace_back(Pin::FLOAT32, "Metallic");
	//InputPins.emplace_back(Pin::FLOAT32, "Emissive");
	NodeName = "Output";
	m_NodeType = "bsdf";
	m_Identifier = "node_" + std::to_string(Id);

	NodeHeaderColour = ImU32(0xFF4B92C2);
}

void BSDFNode::InitialiseNodeState(IcePick::MaterialBase& editMaterialBase) {
	InputPins[1].PinIsUsedByNode = (editMaterialBase.ShadingModel != IcePick::MaterialBase::MaterialShadingModel::UNLIT);
}

void BSDFNode::ParseLogic(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) {
	if (m_NodeVisited)
		return;

	std::string& sAlbedo = InputPins[0].ShaderIdentifier;
	std::string& sShininess = InputPins[1].ShaderIdentifier;

	switch (editMaterialBase.ShadingModel) {
	case IcePick::MaterialBase::MaterialShadingModel::UNLIT:
	{
		ss << "OutColour = " << sAlbedo << ";\n";
	}
	break;

	case IcePick::MaterialBase::MaterialShadingModel::PHONG_SHADING:
	{
		ss <<
			"vec4 albedoColour = " << sAlbedo << ";\n" <<
			"float shininess = " << sShininess << ";\n" <<
		R"(
	vec3 normal = normalize(v_Normal);
    vec3 L = normalize(vec3(2.0f, 3.0f, 4.0f) - vec3(0.0f));
    vec3 R = normalize(reflect(-L, normalize(normal)));
    vec3 V = normalize(CameraPosition - v_Pos);
    vec4 OutputColour = vec4(0.0f);
    if (dot(normal, L)  < 0.0f){
        OutputColour = vec4((vec3(0.525f, 0.565f, 0.565f) * 0.8f) * albedoColour.xyz, albedoColour.a);
    }
    else {
        OutputColour += vec4((vec3(0.525f, 0.565f, 0.565f) * 0.8f) * albedoColour.xyz, albedoColour.a); // ambient
        OutputColour += vec4((albedoColour.rgb * dot(normal, L)) * (vec3(1.0f, 1.0f, 1.0f) * 0.68f), 0.0f); // diffuse
        OutputColour += vec4(vec3(1.0f, 1.0f, 1.0f) * pow(max(dot(V, R), 0.0), 32.0f * shininess), 0.0f) * shininess;
    }
    OutColour = OutputColour;
		)";
	}
	break;
		
	}

	m_NodeVisited = true;
}

bool BSDFNode::NodeStateValid() {
	bool pinIsTypeVec4 = (InputPins[0].ConnectedPinType == Pin::PinType::VEC4);
	bool shininessIsScalar = (InputPins[1].ConnectedPinType == Pin::PinType::FLOAT32) || (InputPins[1].ConnectedPinType == Pin::PinType::ANY);
	return pinIsTypeVec4 && shininessIsScalar;
}

std::string BSDFNode::GetPinOutput(unsigned int outputPinIndex) {
	return "";
}
