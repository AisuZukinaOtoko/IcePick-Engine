#pragma once
#include <string>
#include <functional>
#include "PanelCommon.h"
#include "Popups/MeshImportPopup.h"
#include "../EditorRenderer.h"
#include "../Viewport/SelectionContext.h"
#include <glm/glm.hpp>
#include <entt/entt.h>

class PropertiesPanel {
public:
	PropertiesPanel(IcePick::EngineAPI engineAPI);
	~PropertiesPanel() = default;
	void SelectedProperties(const Styles& styles);
	void SetColumnWidth(float newWidth);
	void SetSelectionContext(SelectionContext selectionContext);

	void SetDropEntity(entt::entity entity);
	void SetDropAssetPath(std::string filePath);
private:
	void PanelSetup();
	void EntityProperties(const Styles& styles);
	void Vec3Control(const char* label, glm::vec3& values, const float dragSpeed);
	void QuaternionEulerControls(const char* label, glm::quat& value, const float dragSpeed);
	void TextProperty(const char* label, const char* property);
	void InputTextProperty(const char* label, std::string& text);
	void FloatSlider(const char* label, float* value, float min, float max);
	void CheckBox(const char* label, bool* value);
	void ColourPicker(const char* label, glm::vec3& rgb);
	void MaterialInstanceParameters(IcePick::MaterialBase& materialBase, IcePick::MaterialInstance& materialInstance);
	void EntityDropTargetProperty(const char* label, entt::entity& entityProperty);

	// Component Properties
	void CameraDetails();
	void MeshRendererDetails(const Styles& styles);
	void ScriptComponentDetails(const Styles& styles);
	void RigidBodyComponentDetails(const Styles& styles);
	void CameraControllerDetails();

	const char* m_ID = "Properties";
	float m_ColumnWidth = 50.0f;
	float m_ValueColumnWidth = 100.0f;

	entt::entity m_DroppedEntity = entt::null;
	SelectionContext m_SelectionContext;

	std::string m_DropAssetPath;
	IcePick::EngineAPI m_EngineAPI;
	MeshImportPopup m_MeshImportPopup;
	EditorRenderer m_PreviewRenderer;
};
