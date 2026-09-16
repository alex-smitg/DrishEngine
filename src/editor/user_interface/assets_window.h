#pragma once

#include <string>
#include <filesystem>
#include <typeinfo>

#include "imgui_docking/imgui.h"
#include "imgui_docking/imgui_impl_glfw.h"
#include "imgui_docking/imgui_impl_opengl3.h"

#include "../../engine/asset_repository.h"
#include "../../engine/loaders/image_loader.h"

#include "../../engine/logger.h"

#include "editor_window_base.h"


#include <map>


class File {
public:
	std::string name = "";
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

	/*Texture* importTexture() {
		std::filesystem::path texturePath = drishengine::openImageOpenFileDialog();
		if (!texturePath.empty())
		{
			std::filesystem::path filename = texturePath.filename();

			Texture* texture = new Texture();
			texture->name = filename.string();

			ImageLoaderError err = ImageLoader::loadImage(texturePath, texture);
			if (err != ImageLoaderError::OK) {
				delete texture;
				return nullptr;
			}
			
			assetRepository->textures.add(texture);

			if (drishPath != nullptr)
			{
				std::filesystem::path copyTo = currentDirectory->absolutePath / filename;
				
				if (std::filesystem::exists(copyTo)) {
					logInfo("[ASSETS WINDOW] file already exist, no need to copy");
				}
				else {
					std::filesystem::copy_file(texturePath, copyTo);
				}
			}	
			else
			{
				logError("[ASSETS WINDOW] drishPath is null");
			}
			return texture;
			
		}
		return nullptr;
	}*/


	void reload() {
		for (const std::filesystem::directory_entry& entry :
			std::filesystem::directory_iterator(filesPath)) {
			std::cout << entry << "\n";


			if (entry.is_directory()) {
			}
			else {
				File* file = new File();
				file->name = entry.path().filename().string();
				projectFiles[file->name] = file;
				
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

			if (ImGui::BeginTable("Table", 3, ImGuiTableFlags_Borders)) {
				ImGui::TableSetupColumn("i");
				ImGui::TableSetupColumn("Name");
				ImGui::TableSetupColumn("Props");
				ImGui::TableHeadersRow();


				int n = 0;
				for (auto const& pair : projectFiles) {



					ImGui::TableNextRow();

					
					
					ImGui::TableNextColumn();
					static bool selected = false;
					ImGui::PushID(n);
					ImGui::Selectable("##row", &selected, ImGuiSelectableFlags_SpanAllColumns);
					if (ImGui::BeginDragDropSource()) {
						ImGui::EndDragDropSource();
					}
					ImGui::PopID();

					ImGui::TableNextColumn();
					ImGui::Text(pair.first.c_str());
					
					ImGui::TableNextColumn();
					n++;
				}


				ImGui::EndTable();
			}




			if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			{
				ImGui::OpenPopup("menu");
			}


			static bool createPopupOpened = false;
			static int popupType = 0;

			if (ImGui::BeginPopup("menu"))
			{
				if (ImGui::MenuItem("Create script")) {
					createPopupOpened = true;
					popupType = 1;
				}
				if (ImGui::MenuItem("Create material")) {
					createPopupOpened = true;
					popupType = 2;
				}


				ImGui::EndPopup();
			}
			
			

			if (createPopupOpened) {
				ImGui::OpenPopup("Popup", 0);
			}

			if (ImGui::BeginPopupModal("Popup")) {

				static bool buttonDisabled = false;
				ImGui::Text((popupType - 1) ? "Create material" : "Create script");
				static std::string name;
				std::string ext = (popupType - 1) ? ".mat" : ".lua";
				if (ImGui::InputText(ext.c_str(), &name, ImGuiInputTextFlags_ElideLeft | ImGuiInputTextFlags_CallbackEdit, [](ImGuiInputTextCallbackData* data) {
					if (data->EventFlag == ImGuiInputTextFlags_CallbackEdit) {
						/*char c = data->Buf[0];
						if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) data->Buf[0] ^= 32;
						data->BufDirty = true;*/


					}

					return 0; })) {
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

					std::string filename = name + ((popupType - 1) ? ".mat" : ".lua");

					std::filesystem::path pathTo = filesPath / filename;

					std::ofstream stream(pathTo);
					if (stream.is_open()) {
						File* file = new File();
						file->name = name;
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