#pragma once

#include <string>
#include <filesystem>
#include <typeinfo>

#include "imgui_docking/imgui.h"

#include "../../engine/asset_repository.h"

#include "../../engine/file_manager.h"

#include "../../engine/logger.h"

#include "editor_window_base.h"


#include <map>


class AssetWindow: public EditorWindowBase
{
private:
	AssetRepository *assetRepository = nullptr;
	FileManager * fileManager = nullptr;

public:
	AssetWindow(AssetRepository *assetRepository, FileManager* fileManager)
	{
		this->assetRepository = assetRepository;
		this->fileManager = fileManager;

	}

	void draw() override
	{
		if (open)
		{
			ImGui::Begin("Assets", &open);


			if (ImGui::Button("Reload")) {
				fileManager->reload();
			}
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(1.0, 1.0));
			if (ImGui::BeginTable("Table", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit)) {
				ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 32.0f);
				ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableHeadersRow();


				int n = 0;
				for (auto const& pair : fileManager->projectFiles) {



					ImGui::TableNextRow();

					
					
					ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));
					ImGui::TableNextColumn();
					static bool selected = false;
					ImGui::PushID(n);
					ImGui::Selectable("##row", &selected, ImGuiSelectableFlags_SpanAllColumns |
						ImGuiSelectableFlags_AllowOverlap, ImVec2(0, 32));
					if (pair.second->assetHandle && ImGui::BeginDragDropSource()) {
						int index = pair.second->assetHandle->index;

						std::string payloadType = "SCRIPT";

						if (pair.second->type == FileType::IMAGE) payloadType = "TEXTURE";
						if (pair.second->type == FileType::SCRIPT) payloadType = "SCRIPT";
						if (pair.second->type == FileType::MODEL) payloadType = "VERTICES";
						if (pair.second->type == FileType::MATERIAL) payloadType = "MATERIAL";

						ImGui::SetDragDropPayload(payloadType.c_str(), &index, sizeof(int));

						ImGui::Text(pair.first.c_str());

						if (pair.second->type == FileType::IMAGE) {
							
							ImGui::Image(pair.second->icon, ImVec2(128, 128));
						}
						

						ImGui::EndDragDropSource();
					}
					ImGui::PopID();
					ImGui::SameLine(0, 0);
					ImGui::SetNextItemAllowOverlap();
					if (pair.second->icon != -1) {
						
						ImGui::Image(pair.second->icon, ImVec2(32, 32));
						if (ImGui::IsItemHovered())
						{
							ImGui::BeginTooltip();
							ImGui::Image(pair.second->icon, ImVec2(256, 256));
							ImGui::EndTooltip();
						}
					}

					ImGui::PopStyleVar();



					ImGui::TableNextColumn();
					ImGui::Text(pair.first.c_str());
					
					ImGui::TableNextColumn();
					if (pair.second->assetHandle != nullptr) {
						ImGui::Text(std::to_string(pair.second->assetHandle->index).c_str());
					}
					n++;
				}


				ImGui::EndTable();
			}

			ImGui::PopStyleVar();


			if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			{
				ImGui::OpenPopup("menu");
			}


			static bool createPopupOpened = false;
			static FileType popupType = FileType::NONE;

			if (ImGui::BeginPopup("menu"))
			{
				if (ImGui::MenuItem("Create script")) {
					createPopupOpened = true;
					popupType = FileType::SCRIPT;
				}
				if (ImGui::MenuItem("Create material")) {
					createPopupOpened = true;
					popupType = FileType::MATERIAL;
				}


				ImGui::EndPopup();
			}
			
			

			if (createPopupOpened) {
				ImGui::OpenPopup("Popup", 0);
			}

			if (ImGui::BeginPopupModal("Popup")) {

				static bool buttonDisabled = false;

				std::string ext = "";
				static std::string name = "";

				if (popupType == FileType::MATERIAL) {
					ext = ".mat";
					ImGui::Text("Create material");
				}
				if (popupType == FileType::SCRIPT) {
					ext = ".lua";
					ImGui::Text("Create script");
				}

				if (ImGui::InputText(ext.c_str(), &name, ImGuiInputTextFlags_ElideLeft |
														ImGuiInputTextFlags_CallbackCharFilter | 
														ImGuiInputTextFlags_EnterReturnsTrue
					, [](ImGuiInputTextCallbackData* data) {
					const ImWchar c = data->EventChar;

					/*if (!(c >= 'A' && c <= 'Z')) {
						return 1;
					}*/

					return 0; })) {
					
				}

				if (ImGui::IsItemEdited()) {
					buttonDisabled = false;
					if (this->fileManager->projectFiles.contains(name + ext)) {
						buttonDisabled = true;
					}
				}



				if (ImGui::Button("Cancel")) {
					ImGui::CloseCurrentPopup();
					createPopupOpened = false;
					name = "";
				}
				ImGui::SameLine();

				

				if (name.empty()) {
					buttonDisabled = true;
				}

				if (buttonDisabled) ImGui::BeginDisabled();

				if (ImGui::Button("Add")) {

					std::string filename = name + ext;

					std::filesystem::path pathTo = fileManager->projectFilesPath / filename;


					AssetHandle* assHandle = nullptr;


					
					std::ofstream stream(pathTo);
					if (stream.is_open()) {
						File* file = new File();
						file->name = name;

						if (popupType == FileType::MATERIAL) {
							Material* material = new Material();
							material->shader = &assetRepository->defaultShader;
							material->name = filename;
							assHandle = assetRepository->materials.add(material);
							nlohmann::json j = *material;
							stream << std::setw(4) << j << std::endl;

						}
						if (popupType == FileType::SCRIPT) {
							Script* script = new Script();
							script->name = filename;
							assHandle = assetRepository->scripts.add(script);
						}

						file->assetHandle = assHandle;
						this->fileManager->projectFiles[filename] = file;

					}
					else {
						logError("Stream is closed");
					}

					stream.close();



					ImGui::CloseCurrentPopup();
					createPopupOpened = false;

					name = "";
				};

				if (buttonDisabled) ImGui::EndDisabled();

				ImGui::EndPopup();
			}

	
			ImGui::End();
		}
	}
};