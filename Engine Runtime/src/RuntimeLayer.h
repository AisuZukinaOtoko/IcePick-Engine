#pragma once
#include "Public/Layer.h"
#include "Public/EngineAPI.h"

class RuntimeLayer : public IcePick::Layer {
public:
	RuntimeLayer(IcePick::EngineAPI);
	virtual void OnAttach() override;
	virtual void OnUpdate(DeltaTime dt) override;
	virtual void OnDetach() override;

	virtual void OnRender(IcePick::RenderPayload& payload);
private:
	IcePick::EngineAPI m_EngineAPI;
};