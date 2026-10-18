#include "TerrainPanel.h"
#include "PropertiesPanel.h"
#include "File Systems/AssetTypes.h"
#include <imgui-docking/imgui.h>
#include "Scene Systems/TerrainSystem.h"
#include "Public/EngineAPI.h";

TerrainPanel::TerrainPanel() {

}

void TerrainPanel::Render(IcePick::EngineAPI engineAPI, const Styles& styles) {
	ImGui::Begin("Terrain");
	bool terrainCreated = engineAPI.IsTerrainCreated();

	if (terrainCreated)
		TerrainCreatedUI(engineAPI, styles);
	else
		NoTerrainCreatedUI(engineAPI);

	ImGui::End();
}

void TerrainPanel::TerrainCreatedUI(IcePick::EngineAPI engineAPI, const Styles& styles) {
	// Height map UI
	if (ImGui::BeginTable("Height Map Texture", 2)) {
		ImGui::TableNextRow(ImGuiTableRowFlags_None);
		ImGui::TableNextColumn();
		const int imageSize = 45;
		unsigned int parameterTextureRenderId = engineAPI.GetTextureRenderId(engineAPI.GetTerrainHeightMapId());
		ImGui::Image((void*)parameterTextureRenderId, ImVec2(imageSize, imageSize), ImVec2(0, 1), ImVec2(1, 0));

		ImGui::TableNextColumn();

		ImGui::Text("Height Map");
		if (ImGui::Button("Clear")) {
			engineAPI.SetTerrainHeightMapId(IcePick::UUID::Unitialised());
		}
		ImGui::EndTable();
	}

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IcePick::GetAssetTypeString(IcePick::AssetTypes::TEXTURE))) {
			std::filesystem::path droppedAssetPath((const char*)payload->Data);
			IcePick::UUID droppedTextureId = engineAPI.LoadTextureFromAsset(droppedAssetPath);
			engineAPI.SetTerrainHeightMapId(droppedTextureId);
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::Separator();


	{
		float& terrainHeight = engineAPI.GetTerrainHeightScale();
		ImGui::DragFloat("Height Scale", &terrainHeight);
	}

	{
		std::vector<IcePick::UUID>& terrainMaterialSlots = engineAPI.GetTerrainMaterialSlots();
		PropertiesPanel::RenderMaterialSlots(terrainMaterialSlots, engineAPI, styles, IcePick::ImportSettings::MeshType::TERRAIN_MESH);
	}

	ImGui::Separator();
	if (ImGui::Button("Delete Terrain")) {
		ImGui::OpenPopup("DELETE_CONFIRMATION");
	}

	if (ImGui::BeginPopup("DELETE_CONFIRMATION")) {
		ImGui::Text("Are you sure you want to delete the terrain?");

		if (ImGui::Button("Delete")) {
			engineAPI.DestroyTerrain();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void TerrainPanel::NoTerrainCreatedUI(IcePick::EngineAPI engineAPI) {
	ImVec2 buttonSize{};
	if (!m_InputtingTerrainSettings && ImGui::Button("Create Terrain")) {
		m_InputtingTerrainSettings = true;
	}

	if (!m_InputtingTerrainSettings)
		return;

	ImGui::InputScalar("Terrain size", ImGuiDataType_U32, &m_TerrainSize);
	ImGui::InputScalar("Units per metre", ImGuiDataType_U32, &m_UnitsPerMetre);

	if (ImGui::Button("Create")) {
		engineAPI.CreateNewTerrain(m_TerrainSize, m_UnitsPerMetre);
		m_InputtingTerrainSettings = false;
	}

	ImGui::SameLine();

	if (ImGui::Button("Cancel")) {
		m_InputtingTerrainSettings = false;
	}
}

TerrainPanel::~TerrainPanel() {

}