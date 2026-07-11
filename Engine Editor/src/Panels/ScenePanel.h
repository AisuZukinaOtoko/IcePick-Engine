#pragma once
#include "PanelCommon.h"
#include "../Viewport/SelectionContext.h"
#include <entt/entt.h>
#include <functional>

namespace IcePick {
	struct SkeletonNodeHierarchy;
	class EngineAPI;
}

class ScenePanel {
public:
	ScenePanel();
	~ScenePanel();
	void SetSelectionContextChangeCallback(std::function<void(SelectionContext)> callback);
	void SetSelectionContext(SelectionContext selectionContext);
	void OnUpdate(DeltaTime dt);
	void ShowSceneHierarchy(IcePick::EngineAPI& engineAPI);

	entt::entity GetDraggedEntity();
private:
	void RenderSkeletonHierarchyRecursive(const IcePick::SkeletonNodeHierarchy& parentNode, IcePick::Skeleton& skeleton, entt::entity entityId);
	std::function<void(SelectionContext)> SelectionContextChangeCallback;
	const char* m_Title;
	bool m_IsDraggingItem = false;

	entt::entity m_DraggedEntity = entt::null;

	bool m_SelectionContextChanged = false;
	SelectionContext m_SelectionContext;
};