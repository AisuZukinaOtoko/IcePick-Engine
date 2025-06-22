#pragma once
#include "../src/Layers/Layer.h"
#include "Viewport/Viewport.h"
#include "Panels/LogPanel.h"
#include "Panels/PropertiesPanel.h"
#include "Panels/ScenePanel.h"
#include "Panels/AssetBrowser.h"
#include "Menu/Toolbar.h"
#include "../src/Layers/EngineAPI.h"

namespace IcePick {
	class EditorLayer : public Layer {
	public:
		EditorLayer(EngineAPI engineAPI);
		virtual void OnAttach() override;
		virtual void OnUpdate(DeltaTime dt) override;
		virtual void OnDetach() override;

		virtual void OnEvent(Event& event) override;
		virtual void OnRender(RenderPayload& payload) override;
	private:
		EngineAPI m_EngineAPI;
		LogPanel m_LogPanel;
		PropertiesPanel m_PropertiesPanel;
		Viewport m_Viewport;
		ScenePanel m_ScenePanel;
		AssetBrowser m_AssetBrowser;
		Toolbar m_Toolbar;
	};
}