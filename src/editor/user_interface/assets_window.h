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


class FsItem {
public:
	std::string name;

	FsItem* parent = nullptr;

	std::filesystem::path absolutePath;

	unsigned int textureID = -1; // -1 = no texture
	bool useTexture = true;
	unsigned int color = 0xFF00FF;

	virtual bool isDirectory() {
		return false;
	};

	virtual void free() {};
};

class File : public FsItem
{
public:
	bool isDirectory() override {
		return false;
	}

	void free() override {
		
	}

	File(std::string name) {
		this->name = name;
	}
};


class Directory: public FsItem
{
public:
	std::map<std::string, FsItem*> content;

	Directory(std::string name) {
		this->name = name;
	}

	bool isDirectory() override {
		return true;
	}


	void free() override {
		for (auto a : content) {
			a.second->free();
			delete a.second;
		}
		content.clear();
	}

	File* addFile(std::string name) {
		File* file = new File(name);
		file->parent = this;

		content[name] = file;

		return file;
	}

	Directory* addDirectory(std::string name) {
		Directory* childDirectory = new Directory(name);
		childDirectory->parent = this;
		
		content[name] = childDirectory;
		
		return childDirectory;
	}
};

class AssetWindow: public EditorWindowBase
{
private:
	AssetRepository *assetRepository;

public:
	std::filesystem::path *drishPath = nullptr;

	Directory projectRoot = Directory("project");

	std::vector<Directory*> currentDirectoryPath;
	Directory* currentDirectory = &projectRoot; 

	AssetWindow(AssetRepository *assetRepository, std::filesystem::path* drishPath)
	{
		this->assetRepository = assetRepository;
		this->drishPath = drishPath;
	}





	Texture* importTexture() {
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
	}

	void changeCurrentFolder(Directory* directory) {
		currentDirectory = directory;

		currentDirectoryPath.clear();

		Directory* tempDirectory = directory;

		while (tempDirectory != nullptr) {
			currentDirectoryPath.push_back(tempDirectory);
			tempDirectory = static_cast<Directory*>(tempDirectory->parent);
		}

	}


	void refresh() {
		changeCurrentFolder(&projectRoot);
		projectRoot.free();
		

		std::map<std::string, FsItem*> index;

		index[(drishPath->parent_path() / "project").string()] = &projectRoot;

		for (const std::filesystem::directory_entry& entry :
			std::filesystem::recursive_directory_iterator(drishPath->parent_path() / "project")) {
			std::cout << entry << "\n";
			
			FsItem* parent = index[entry.path().parent_path().string()];
			if (parent->isDirectory()) {
				Directory* parentDirectory = static_cast<Directory*>(parent);

				if (entry.is_directory()) {
					Directory* childDirectory = parentDirectory->addDirectory(entry.path().filename().string());
					index[entry.path().string()] = childDirectory;
					childDirectory->absolutePath = entry.path().string();
				}
				else {
					File* childFile = parentDirectory->addFile(entry.path().filename().string());
					index[entry.path().string()] = childFile;
					childFile->absolutePath = entry.path().string();
					childFile->useTexture = false;

					std::string ext = entry.path().extension().string();

					if (ext == ".txt" || ext == ".TXT") {
						childFile->color = 0xFF3A3A3A;
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
	

			ImGui::SameLine();

			if (ImGui::Button("Refresh")) {
				refresh();
			}


			bool endDisabled = false;
			if (currentDirectory->parent == nullptr) {
				ImGui::BeginDisabled();
				endDisabled = true;
			}

			
			if (ImGui::Button("<")) {
				if (currentDirectory->parent != nullptr) {
					changeCurrentFolder(static_cast<Directory*>(currentDirectory->parent));
				}
			}

			if (endDisabled) {
				ImGui::EndDisabled();
			}

			ImGui::SameLine();

			for (int i = currentDirectoryPath.size() - 1; i > 0; i--) {
				if (ImGui::TextLink(currentDirectoryPath[i]->name.c_str())) {
					changeCurrentFolder(currentDirectoryPath[i]);
					break;
				}
				ImGui::SameLine();
				ImGui::Text("/");
				ImGui::SameLine();
			}
			ImGui::Text(currentDirectory->name.c_str());
			ImGui::BeginChild("Content", { 0, 0 }, ImGuiChildFlags_Borders);


			static bool isHoveringOverIcon = false;

			if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			{
				ImGui::OpenPopup("r");
				isHoveringOverIcon = false;
			}

			int width = ImGui::GetWindowWidth();
			const int ASSET_SIZE = 48;
			int r = width / ASSET_SIZE;

			if (r == 0) { r = 1; }

			ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign,
				ImVec2(0.0f, 0.0f));

			

			static FsItem* rightClickedItem = nullptr;

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0, 0 });
			if (currentDirectory != nullptr) {
				int i = 0;
				for (auto a : currentDirectory->content) {
					if (a.second->isDirectory() == true) {
						ImGui::PushID(i);
						if (i % r != 0) {
							ImGui::SameLine();
						}

						if (ImGui::Button(a.second->name.c_str(), { ASSET_SIZE, ASSET_SIZE })) {
							changeCurrentFolder(static_cast<Directory*>(a.second));
						}


						if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
							rightClickedItem = a.second;
						}



						
						isHoveringOverIcon |= ImGui::IsItemHovered();

						ImGui::PopID();
					}
					else {
						ImGui::PushID(i);
						if (i % r != 0) {
							ImGui::SameLine();
						}

						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
						ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
						if (a.second->useTexture) {
							if (ImGui::ImageButton(a.second->name.c_str(), a.second->textureID, { ASSET_SIZE, ASSET_SIZE })) {

							}
						}
						else {
							ImGui::PushStyleColor(ImGuiCol_Button, a.second->color);
							if (ImGui::Button(a.second->name.c_str(), { ASSET_SIZE, ASSET_SIZE }));
							ImGui::PopStyleColor();
						}
						ImGui::PopStyleColor(3);

						if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
						{
							ImGui::SetTooltip(a.second->name.c_str());
						}

						if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
							rightClickedItem = a.second;
						}

						isHoveringOverIcon |= ImGui::IsItemHovered();

						ImGui::PopID();
					}
					i++;
				}
			}
			ImGui::PopStyleVar();
			ImGui::PopStyleVar();

			
			static bool renamePopupOpened = false;

			if (ImGui::BeginPopup("r"))
			{

				if (isHoveringOverIcon) {
					if (ImGui::MenuItem("Rename")) {
						renamePopupOpened = true;
					}
					if (ImGui::MenuItem("Delete")) {
						changeCurrentFolder(&projectRoot);
					}

				}
				else {
					if (ImGui::BeginMenu("Add")) {
						if (ImGui::MenuItem("New folder")) {
							try
							{
								std::filesystem::path pathTo = currentDirectory->absolutePath / "New folder";
								logInfo("Create new folder ", pathTo);

								if (std::filesystem::create_directory(pathTo)) {
									Directory* directory = currentDirectory->addDirectory("new folder");
								}
							}

							catch (std::filesystem::filesystem_error const& ex)
							{
								logError(ex.what());
							}
						}
						if (ImGui::MenuItem("New material")) {

						}
						if (ImGui::MenuItem("New script")) {

						}
						ImGui::EndMenu();
					}
				}



				ImGui::EndPopup();
			}

			static std::string str = "";

			if (renamePopupOpened) {
				ImGui::OpenPopup("RenamePopup", 0);
				
				if (rightClickedItem != nullptr && str.empty()) {
					str = rightClickedItem->name;
				}
			}
			

			

			if (ImGui::BeginPopupModal("RenamePopup")) {
				
				ImGui::InputText("Text", &str);
				if (ImGui::Button("Ok")) {
					ImGui::CloseCurrentPopup();
					renamePopupOpened = false;
					

					if (rightClickedItem->parent != nullptr) {
						if (rightClickedItem->parent->isDirectory()) {
							Directory* directory = static_cast<Directory*>(rightClickedItem->parent);
							auto nh = directory->content.extract(rightClickedItem->name);
							nh.key() = str;
							directory->content.insert(std::move(nh));

							rightClickedItem->name = str;

						}
					}


					str = "";

					
					
				};
				if (ImGui::Button("Cancel")) {
					ImGui::CloseCurrentPopup();
					renamePopupOpened = false;
					str = "";
				}
				ImGui::EndPopup();
			}
			

			ImGui::EndChild();
	
			ImGui::End();
		}
	}
};