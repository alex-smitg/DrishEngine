#pragma once

#include <string>
#include <filesystem>
#include <typeinfo>

#include "imgui_docking/imgui.h"

#include "../../engine/asset_repository.h"
#include "../../engine/loaders/image_loader.h"
#include "../../engine/loaders/model_loader.h"

#include "../../engine/logger.h"

#include "editor_window_base.h"


#include <map>

enum class FileType {
	NONE,
	IMAGE,
	SCRIPT,
	MATERIAL,
	MODEL,
	OTHER
};

class File {
public:
	std::string name = "";
	AssetHandle* assetHandle = nullptr;

	unsigned int icon = -1; //opengl texture

	FileType type = FileType::NONE;

};


class AssetWindow: public EditorWindowBase
{
private:
	AssetRepository *assetRepository;

public:
	std::filesystem::path *drishPath = nullptr;

	std::filesystem::path filesPath;

	std::map<std::string, File*> projectFiles;

	AssetWindow(AssetRepository *assetRepository, std::filesystem::path* drishPath)
	{
		this->assetRepository = assetRepository;
		this->drishPath = drishPath;

	}

	void reload() {
		for (const std::filesystem::directory_entry& entry :
			std::filesystem::directory_iterator(filesPath)) {
			


			if (entry.is_directory()) {
			}
			else {
				std::string filename = entry.path().filename().string();
				std::string extension = entry.path().extension().string();

				if (projectFiles.contains(filename)) {

				} else {
					logInfo("First load: ", entry.path());

					File* file = new File();
					file->name = filename;
					projectFiles[filename] = file;

					if (extension == ".mat") {
						Material* material = new Material();
						material->name = filename;
						material->shader = &assetRepository->defaultShader;
						file->assetHandle = assetRepository->materials.add(material);
						file->type = FileType::MATERIAL;
					}

					if (extension == ".lua") {
						Script* script = new Script();
						script->name = filename;
						file->assetHandle = assetRepository->scripts.add(script);
						file->type = FileType::SCRIPT;

						std::ifstream file(filesPath / filename);
						if (file.is_open()) {
							std::stringstream buf;
							buf << file.rdbuf();
							script->source = buf.str();
						}
						else {
							delete script;
							logError("Script import stream is closed");
						}

					}
					if (extension == ".png" || extension == ".jpg" || extension == "jpeg") {
						Texture* texture = new Texture();
						texture->name = filename;

						ImageLoaderError err = ImageLoader::loadImage(entry.path(), texture);
						if (err != ImageLoaderError::OK) {
							delete texture;
							logError("Error loading: ", entry.path().string());
						}
						else {
							file->assetHandle = assetRepository->textures.add(texture);
							file->type = FileType::IMAGE;
							file->icon = texture->glid;
						}
						
					}
					if (extension == ".obj") {
						Vertices* vertices = new Vertices();
						vertices->name = filename;

						drishengine::loadObj(filesPath / filename, vertices);
						file->assetHandle = assetRepository->vertices.add(vertices);
						file->type = FileType::MODEL;
						logInfo("Generating buffers");
						vertices->createBuffers();
						
					}
				}

				
				
			}
			
		}
	}

	void draw() override
	{
		if (open)
		{
			ImGui::Begin("Assets", &open);


			if (ImGui::Button("Reload")) {
				this->reload();
			}
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(1.0, 1.0));
			if (ImGui::BeginTable("Table", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit)) {
				ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 32.0f);
				ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableHeadersRow();


				int n = 0;
				for (auto const& pair : projectFiles) {



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
					if (this->projectFiles.contains(name + ext)) {
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

					std::filesystem::path pathTo = filesPath / filename;


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
						projectFiles[filename] = file;

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