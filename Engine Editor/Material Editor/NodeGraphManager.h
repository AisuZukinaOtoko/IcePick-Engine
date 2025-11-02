#pragma once
#include "Nodes/NodeBase.h"
#include <memory>

typedef std::vector<std::shared_ptr<Node>> Graph;

class NodeGraphManager {
public:
	NodeGraphManager();
	~NodeGraphManager();
	Graph GetGraphById(IcePick::UUID graphId);
private:
	Graph m_DefaultGraph;
	std::unordered_map<IcePick::UUID, Graph, UUIDHasher> m_NodeGraphs;
};