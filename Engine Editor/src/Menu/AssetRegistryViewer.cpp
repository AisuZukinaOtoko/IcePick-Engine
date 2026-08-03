#include "AssetRegistryViewer.h"
#include "Scene Systems/AssetRegistry.h"
#include <IconsFontAwesome7.h>
#include <imgui-docking/imgui.h>
#include <string>

//static std::string GetAssetRegistryAssetTypeString(IcePick::AssetTypes assetType) {
//	switch (assetType) {
//	case IcePick::AssetTypes::STATIC_MESH:
//		return "Static Mesh";
//	case IcePick::AssetTypes::SKELETAL_MESH:
//		return "Skeletal Mesh";
//	case IcePick::AssetTypes::MATERIAL_BASE:
//		return "Material Base";
//	case IcePick::AssetTypes::MATERIAL_INSTANCE:
//		return "Material Instance";
//	case IcePick::AssetTypes::TEXTURE:
//		return "Texture";
//	case IcePick::AssetTypes::SKELETON:
//		return "Skeleton";
//	default:
//		return "Error";
//	}
//}

void AssetRegistryViewer::Render() {
	if (!m_IsOpen)
		return;


	IcePick::AssetRegistry& assetRegistry = IcePick::GetAssetRegistry();
	const std::vector<IcePick::AssetRegistryEntry>& registeredAssets = assetRegistry.GetRegisteredAssets();

	IcePick::UUID deleteAssetId = IcePick::UUID::Unitialised();
	ImGui::Begin("Asset Registry", &m_IsOpen);

	ImGuiTableFlags tableFlags = ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY;
	if (ImGui::BeginTable("Registered Assets", 4, tableFlags)) {
		ImGui::TableSetupColumn("Asset ID");
		ImGui::TableSetupColumn("File Path");
		ImGui::TableSetupColumn("Asset Type");
		ImGui::TableHeadersRow();

		for (size_t assetIndex = 0; assetIndex < registeredAssets.size(); assetIndex++) {
			const IcePick::AssetRegistryEntry& assetReference = registeredAssets[assetIndex];

			ImGui::PushID(assetIndex);
			ImGui::TableNextRow(ImGuiTableRowFlags_None);
			ImGui::TableNextColumn();

			ImGui::Text(std::to_string(assetReference.AssetId).c_str());

			ImGui::TableNextColumn();
			ImGui::Text(assetReference.AssetRelativePath.string().c_str());

			ImGui::TableNextColumn();
			ImGui::Text(GetAssetTypeString(assetReference.AssetType));

			ImGui::TableNextColumn();
			if (ImGui::Button(ICON_FA_TRASH, ImVec2(ImGui::GetContentRegionAvail().x, 0.0f))) {
				deleteAssetId = assetReference.AssetId; // Avoid deleting from the registry while iterating
			}

			ImGui::PopID();
		}

		ImGui::EndTable();
	}

	if (deleteAssetId != IcePick::UUID::Unitialised()) {
		assetRegistry.DeleteAsset(deleteAssetId);
	}

	ImGui::End();
}

void AssetRegistryViewer::SetOpen(bool openState) {
	m_IsOpen = openState;
}