#pragma once

#include <string>

#include "imgui_docking/imgui.h"
#include "imgui_docking/imgui_impl_glfw.h"
#include "imgui_docking/imgui_impl_opengl3.h"

#include "../../engine/game_objects/node.h"
#include "../../engine/asset_repository.h"
#include "../../engine/lua_runner.h"
#include "../../engine/game_objects/model.h"

class PropertiesWindow: public EditorWindowBase {
private:
	AssetRepository* assetRepository;
	LuaRunner* luaRunner;
	Node** selectedPtr;
public:
	PropertiesWindow(AssetRepository* assetRepository, LuaRunner* luaRunner,
		Node** selectedPtr) {

		this->selectedPtr = selectedPtr;
		this->assetRepository = assetRepository;
		this->luaRunner = luaRunner;
	}

	template <typename T>
	void Selector(Field* field, std::function<std::optional<T*>(AssetHandle*)> assetGetter,
		std::string payload_name,
		std::string ass_name) {

		AssetHandle* assHolder = static_cast<AssetHandle*>(field->ptr);
		ImGui::BeginGroup();
		ImGui::Button("..");
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(payload_name.c_str())) {
				int payload_index = *(int*)payload->Data;
				assHolder->index = payload_index;
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::SameLine();

		std::optional<T*> asset = assetGetter(assHolder);
		if (asset.has_value()) {
			ImGui::Text(asset.value()->name.c_str());
			ImGui::SameLine();
			if (ImGui::Button("-")) {
				assHolder->index = INVALID_INDEX;
			}
		}
		else {
			ImGui::Text("_");
		}
		ImGui::EndGroup();
		if (asset.has_value()) {
			drawFields(asset.value()->getFields(), ass_name);
		}
	}

	void drawFields(const std::vector<Field>& fields, std::string name) {
		Node* selectedNode = *selectedPtr;

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg,
			ImGui::GetColorU32(ImVec4(0.0f, 0.1f, 0.2f, 1.0f)));
		ImGui::TextColored(ImVec4(0.1, 0.7, 0.8, 1.0), name.c_str());
		ImGui::TableNextColumn();
		ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg,
			ImGui::GetColorU32(ImVec4(0.0f, 0.1f, 0.2f, 1.0f))); 


		for (int i = 0; i < fields.size(); i++) {
			Field field = fields.at(i);
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted(field.name.c_str());
			ImGui::TableNextColumn();

			ImGui::PushItemWidth(-FLT_MIN);
			ImGui::PushID(field.name.c_str());
			switch (field.type)
			{
			case FieldType::Float:
			{
				ImGui::DragScalarN("", ImGuiDataType_Float, field.ptr, 1, 0.1);
				break;
			}
			case FieldType::FloatVec3:
			{
				ImGui::BeginGroup();
				glm::vec3* vec = static_cast<glm::vec3*>(field.ptr);
				ImGui::DragFloat3("##x", &(vec->x), 0.1f, 0.0f, 0.0f, "%.2f");
				ImGui::EndGroup();
				break;
			}
			case FieldType::String:
			{
				ImGui::InputText("", static_cast<std::string*>(field.ptr));
				break;
			}
			case FieldType::Color3:
			{
				glm::vec3* color = static_cast<glm::vec3*>(field.ptr);
				float col[3] = { color->r, color->g, color->b };
				ImGui::ColorEdit3("", col, ImGuiColorEditFlags_DisplayHex);
				color->r = col[0];
				color->g = col[1];
				color->b = col[2];
				break;
			}
			case FieldType::MaterialHandle:
			{
				auto f = std::bind(&AssetsContainer<Material>::get, assetRepository->materials, std::placeholders::_1);
				Selector<Material>(&field, f, "MATERIAL", "Material");
				break;
			}
			case FieldType::VerticesHandle:
			{
				auto f = std::bind(&AssetsContainer<Vertices>::get, assetRepository->vertices, std::placeholders::_1);
				Selector<Vertices>(&field, f, "VERTICES", "Vertices");
				break;
			}
			case FieldType::TextureHandle:
			{
				auto f = std::bind(&AssetsContainer<Texture>::get, assetRepository->textures, std::placeholders::_1);
				Selector<Texture>(&field, f, "TEXTURE", "Texture");
				break;
			}
			case FieldType::ScriptHandle:
			{
				auto f = std::bind(&AssetsContainer<Script>::get, assetRepository->scripts, std::placeholders::_1);
				Selector<Script>(&field, f, "SCRIPT", "Script");
				break;
			}
			case FieldType::SoundHandle:
			{
				auto f = std::bind(&AssetsContainer<Sound>::get, assetRepository->sounds, std::placeholders::_1);
				Selector<Sound>(&field, f, "SOUND", "Sound");
				break;
			}
			case FieldType::TextureID:
			{
				unsigned int* id = *(unsigned int**)(field.ptr);
				ImGui::Image(id, ImVec2(64, 64));
				break;
			}
			case FieldType::Boolean:
			{
				bool* val = (bool*)(field.ptr);
				ImGui::Checkbox("", val);
			}
			}
			ImGui::PopID();
			ImGui::PopItemWidth();
		}		
	}



	void draw() override {
		Node* selectedNode = *selectedPtr;

		if (open) {
			ImGui::Begin("Properties", &open);

			if (selectedNode != nullptr) {
				if (selectedNode->name != "World") {
					if (ImGui::BeginTable("##properties", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY | ImGuiTableFlags_BordersInnerH)) {
						ImGui::PushID(selectedNode->id);
						ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
						ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, 2.0f);
						drawFields(selectedNode->getFields() , "Node");
						ImGui::PopID();
						ImGui::EndTable();
					}
				}
			}
			ImGui::End();
		}
	} 
};