#pragma once
#include <string>
#include <vector>
#include <sstream>
#include "../Scene Systems/UUID.h"
#include "imgui-docking/imgui.h"

namespace Pin {
	enum PinType {
		FLOAT32, VEC3, VEC4,
		UINT8, UVEC3, UVEC4,
		PIN_TYPE_COUNT
	};

	void InitPins();
	std::string GetPinDefault(PinType type);
}

struct InputPin {
	InputPin() = delete;
	InputPin(Pin::PinType type, std::string label) : Type(type), Label(label) {}
	void DeleteConnection();

	Pin::PinType Type;
	std::string Label;
	std::string ShaderIdentifier;
	IcePick::UUID ConnectedNodeId = IcePick::UUID::Unitialised();
	unsigned int ConnectedPinIndex = 0;
};

struct OutputPin {
	OutputPin() = delete;
	OutputPin(Pin::PinType type, std::string label, std::string shaderAccessor) : Type(type), Label(label), ShaderAccessor(shaderAccessor) {}
	void DeleteConnection(IcePick::UUID connectedNode, unsigned int pinIndex);

	Pin::PinType Type;
	std::string Label;
	std::string ShaderAccessor;
	std::vector<IcePick::UUID> ConnectedNodeIds;
	std::vector<unsigned int> ConnectedPinIndices;
};

class Node {
public:
	IcePick::UUID Id;
	std::vector<InputPin> InputPins;
	std::vector<OutputPin> OutputPins;
	virtual void Initialise(std::stringstream& ss) {}
	virtual void ParseNodeLogic(std::stringstream& ss) {}
	virtual std::string GetPinOutput(unsigned int outputPinIndex) { return ""; }
	void Unitialise();
	ImVec2 CanvasPosition;
protected:
	bool m_Initialised = false;
	std::string m_Identifier;
};

class TextureNode : public Node {
public:
	TextureNode(IcePick::UUID textureId);

	void Initialise(std::stringstream& ss) override;
	void ParseNodeLogic(std::stringstream& ss) override;
	std::string GetPinOutput(unsigned int outputPinIndex) override;
private:
	IcePick::UUID m_TextureId;
};

class BSDFNode : public Node {
public:
	BSDFNode();
};

class Vector3Node : public Node {
public:
	Vector3Node();

	//void Initialise(std::stringstream& ss) override;
	void ParseNodeLogic(std::stringstream& ss) override;
	//std::string GetPinOutput(unsigned int outputPinIndex) override;
};