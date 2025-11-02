#include "NodeGraphManager.h"
#include "Nodes/Nodes.h"
#include "../LogSystem.h"

NodeGraphManager::NodeGraphManager() {
    m_DefaultGraph.push_back(std::make_shared<BSDFNode>());
}

Graph NodeGraphManager::GetGraphById(IcePick::UUID graphId) {
    auto iterator = m_NodeGraphs.find(graphId);

    if (iterator == m_NodeGraphs.end()) {
        IP_LOG("Unable to find node graph. Using defualt node graph instead.", IP_WARN_LOG);
        return m_DefaultGraph;
    }

    return iterator->second;
}

NodeGraphManager::~NodeGraphManager() {

}
