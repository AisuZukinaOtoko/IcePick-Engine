#include "SplitVectorNode.h"
#include "../Utils.h"

static int GetNumberOfComponents(Pin::PinType type) {
	switch (type) {
	case Pin::PinType::VEC2:
		return 2;
	case Pin::PinType::VEC3:
		return 3;
	case Pin::PinType::VEC4:
		return 4;
	}
	return 0;
}

SplitVectorNode::SplitVectorNode() {
	InputPins.emplace_back(Pin::ANY, "Vector");
	OutputPins.emplace_back(Pin::FLOAT32, "X", ".x");
	OutputPins.emplace_back(Pin::FLOAT32, "Y", ".y");
	OutputPins.emplace_back(Pin::FLOAT32, "Z", ".z");
	OutputPins.emplace_back(Pin::FLOAT32, "W", ".w");
	m_NodeType = "splitVector";
	NodeName = "Split Vector";
	nodeCanBeParamterized = false;
}

void SplitVectorNode::Initialise(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) {
	if (m_Initialised)
		return;
	
	const int numberOfVectorComponents = GetNumberOfComponents(InputPins[0].ConnectedPinType);
	for (int i = 0; i < OutputPins.size(); i++) {
		if ((i + 1) <= numberOfVectorComponents)
			OutputPins[i].Type = Pin::PinType::FLOAT32;
		else
			OutputPins[i].Type = Pin::PinType::ANY;
	}
	
	//m_Identifier = "node_" + std::to_string(Id);
	//std::string& sx = InputPins[0].ShaderIdentifier;
	//std::string& sy = InputPins[1].ShaderIdentifier;
	//std::string& sz = InputPins[2].ShaderIdentifier;
	//std::string& sw = InputPins[3].ShaderIdentifier;

	//std::string type = GetPinTypeString(InputPins[0].ConnectedPinType);
	//ss << type << " " << m_Identifier << " = " << type << "(";
	//for (int i = 0; i < numberOfVectorComponents; i++) {
	//	ss << OutputPins[i]
	//}
	////<< sx << " + " << sy;
	//ss << ");\n";
	m_Initialised = true;
}

void SplitVectorNode::ParseNodeLogic(std::stringstream& ss) {

}

bool SplitVectorNode::NodeStateValid() {
	Pin::PinType& inputPinType = InputPins[0].ConnectedPinType;
	bool inputTypeValid = (inputPinType == Pin::PinType::VEC2) || (inputPinType == Pin::PinType::VEC3) || (inputPinType == Pin::PinType::VEC4);
	return inputTypeValid;
}

std::string SplitVectorNode::GetPinOutput(unsigned int outputPinIndex) {
	return InputPins[0].ShaderIdentifier + OutputPins[outputPinIndex].ShaderAccessor;
}
