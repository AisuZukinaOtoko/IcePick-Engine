#pragma once
#include <string>
#include "PanelCommon.h"
#include "../../src/Vendor/glm/glm.hpp"
#include "../../src/Vendor/entt/entt.h"

class PropertiesPanel {
public:
	PropertiesPanel();
	~PropertiesPanel() = default;
	void SelectedProperties();
	void SetColumnWidth(float newWidth);
	void SetEntitySelect(entt::entity entity);

	void EntityProperties();
private:
	void PanelSetup();
	void Vec3Control(const char* label, glm::vec3& values);
	void TextProperty(const char* label, const char* property);
	void FloatSlider(const char* label, float* value, float min, float max);
	void CheckBox(const char* label, bool* value);
	void ColourPicker(const char* label, glm::vec3& rgb);
	const char* m_ID = "Properties";
	float m_ColumnWidth = 50.0f;
	entt::entity m_SelectedEntity = entt::null;
};
