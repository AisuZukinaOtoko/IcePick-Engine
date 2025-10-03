#pragma once
#include <string>
#include <functional>
#include "PanelCommon.h"
#include "../../src/Vendor/glm/glm.hpp"
#include "../../src/Vendor/entt/entt.h"

class PropertiesPanel {
public:
	PropertiesPanel();
	~PropertiesPanel() = default;
	void SelectedProperties(const Styles& styles);
	void SetEditMaterialCallback(std::function<void(IcePick::UUID)> callback);
	void SetColumnWidth(float newWidth);
	void SetSelectedEntity(entt::entity entity);

	void SetDropAssetPath(std::string filePath);
private:
	void PanelSetup();
	void EntityProperties(const Styles& styles);
	void Vec3Control(const char* label, glm::vec3& values, const float dragSpeed);
	void TextProperty(const char* label, const char* property);
	void FloatSlider(const char* label, float* value, float min, float max);
	void CheckBox(const char* label, bool* value);
	void ColourPicker(const char* label, glm::vec3& rgb);
	std::function<void(IcePick::UUID)> EditMaterialCallback;
	const char* m_ID = "Properties";
	float m_ColumnWidth = 50.0f;
	entt::entity m_SelectedEntity = entt::null;
	std::string m_DropAssetPath;
};
