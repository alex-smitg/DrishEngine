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

	unsigned int textureID = -1; // -1 = no texture
	bool useTexture = true;
	unsigned int color = 0xFF00FF;

	virtual bool isDirectory() {
		return false;
	};


};

class File : public FsItem
{
public:
	bool isDirectory() override {
		return false;
	}

	File(std::string name) {
		this->name = name;
	}
};


class Folder: public FsItem
{
public:
	std::map<std::string, FsItem*> content;

	

	Folder(std::string name) {
		this->name = name;
	}

	bool isDirectory() override {
		return true;
	}

	File* addFile(std::string name) {
		File* file = new File(name);
		file->parent = this;

		content[name] = file;

		return file;
	}

	Folder* addFolder(std::string name) {
		Folder* folder = new Folder(name);
		folder->parent = this;
		
		content[name] = folder;
		
		return folder;
	}
};

class AssetWindow: public EditorWindowBase
{
private:
	AssetRepository *assetRepository;

public:
	std::filesystem::path *drishPath = nullptr;


	
	

	Folder fs = Folder("project");
	std::vector<Folder*> currentFolderPath;
	Folder* selectedFolder = &fs;

	AssetWindow(AssetRepository *assetRepository, std::filesystem::path* drishPath)
	{
		this->assetRepository = assetRepository;
		this->drishPath = drishPath;
	}

	template <typename T>
	void drawList(std::string name, std::string payloadName, std::vector<AssetSlot<T>*> slots,
		std::function<void(int)> deleteFunction, std::function<void()> createFunction) {


		if (ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_DrawLinesToNodes | ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (true) {
			
			//commented code is crashing if asset_window is moved outside main window
			//if (ImGui::BeginChild("##child", ImVec2(-FLT_MIN, ImGui::GetTextLineHeightWithSpacing() * 8), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeY)) {
				if (ImGui::Button("+"))
				{
					createFunction();
				}
				if (ImGui::BeginTable(name.c_str(),
					2,
					 ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg, ImVec2(0, 32*6))) {

					ImGui::TableSetupColumn("##image", ImGuiTableColumnFlags_WidthFixed, 32.0f);
					ImGui::TableSetupColumn("##text", 0.0f);

					for (const AssetSlot<T>* assetSlot : slots)
					{
						ImGui::PushID(assetSlot->index);
						if (assetSlot->is_valid) {
							T* asset = assetSlot->asset;

							ImGui::TableNextRow();
							ImGui::TableSetColumnIndex(0);
							if (typeid(T) == typeid(Texture)) {
								Asset* ass = static_cast<Asset*>(asset);
								Texture* tex = static_cast<Texture*>(ass);
								ImGui::Image(tex->glid, ImVec2(32, 32));
							}
							ImGui::TableSetColumnIndex(1);
							ImGui::Selectable(asset->name.c_str(), false, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap, ImVec2(0, 32));
							if (ImGui::BeginPopupContextItem())
							{
								ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.0, 0.0, 1.0));
								if (ImGui::MenuItem("Delete")) {
									deleteFunction(assetSlot->index);
								}
								ImGui::PopStyleColor();
								ImGui::EndPopup();
							}

							if (ImGui::BeginDragDropSource())
							{
								ImGui::SetDragDropPayload(payloadName.c_str(), &assetSlot->index, sizeof(assetSlot->index));
								ImGui::Text(asset->name.c_str());
								ImGui::EndDragDropSource();
							}
						}
						ImGui::PopID();
					}
					ImGui::EndTable();
				}
				//ImGui::EndChild();	
			}
			ImGui::TreePop();
		}
	}

	void drawFolderContents(Folder* folder) {
		ImGuiTreeNodeFlags flags;
		flags = ImGuiTreeNodeFlags_DrawLinesToNodes | ImGuiTreeNodeFlags_DefaultOpen;

		if (folder->content.empty()) {
			flags |= ImGuiTreeNodeFlags_Leaf;
		}

		

		if (selectedFolder == folder) {
			flags |= ImGuiTreeNodeFlags_Selected;
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0, 1.0, 0.9, 1.0));
		}


		ImGui::SetNextItemOpen(false, ImGuiCond_Once);
		bool treeOpened = ImGui::TreeNodeEx(folder->name.c_str(), flags);

		if (selectedFolder == folder) {
			ImGui::PopStyleColor();
		}

		if (ImGui::IsItemClicked()) {
			changeCurrentFolder(folder);
		}


		if (treeOpened) {

			for (auto a : folder->content) {
				FsItem* item = a.second;
				if (item->isDirectory()) {
					drawFolderContents((Folder*)item);
				}
			}
			ImGui::TreePop();
		}

		
	}


	Texture* importTexture() {
		std::filesystem::path texturePath = drishengine::openImageOpenFileDialog();
		if (!texturePath.empty())
		{
			std::filesystem::path filename = texturePath.filename();

			Texture* texture = new Texture();
			texture->name = filename.string();
			texture->path = std::filesystem::path("textures") / filename;

			ImageLoaderError err = ImageLoader::loadImage(texturePath, texture);
			if (err != ImageLoaderError::OK) {
				delete texture;
				return nullptr;
			}
			
			assetRepository->textures.add(texture);

			if (drishPath != nullptr)
			{
				if (std::filesystem::exists(drishPath->parent_path() / "textures"))
				{
					logDebug("[ASSETS WINDOW] textures");
				}
				else
				{
					std::filesystem::create_directory(drishPath->parent_path() / "textures");
				}


				if (std::filesystem::exists(drishPath->parent_path() / "textures" / filename)) {
					logInfo("[ASSETS WINDOW] file already exist, no need to copy");
				}
				else {
					std::filesystem::copy_file(texturePath, drishPath->parent_path() / "textures" / filename);
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

	void changeCurrentFolder(Folder* folder) {
		selectedFolder = folder;
		currentFolderPath.clear();
		Folder* f = selectedFolder;

		while (f != nullptr) {
			currentFolderPath.push_back(f);
			f = static_cast<Folder*>(f->parent);
		}

	}


	void refresh() {
		std::map<std::string, FsItem*> index;

		index[(drishPath->parent_path() / "project").string()] = &fs;

		for (const std::filesystem::directory_entry& entry :
			std::filesystem::recursive_directory_iterator(drishPath->parent_path() / "project")) {
			std::cout << entry << "\n";
			
			FsItem* parent = index[entry.path().parent_path().string()];
			if (parent->isDirectory()) {
				Folder* folder = static_cast<Folder*>(parent);

				if (entry.is_directory()) {
					index[entry.path().string()] = folder->addFolder(entry.path().filename().string());
				}
				else {
					File* file = folder->addFile(entry.path().filename().string());
					index[entry.path().string()] = file;
					file->useTexture = false;

					std::string ext = entry.path().extension().string();

					if (ext == ".txt" || ext == ".TXT") {
						file->color = 0xFF3A3A3A;
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
	

			

			/*ImGui::BeginChild("Tree", { 200, 0 }, ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX, ImGuiWindowFlags_HorizontalScrollbar);
			drawFolderContents(&fs);
			ImGui::EndChild();*/
			ImGui::SameLine();

			if (ImGui::Button("Refresh")) {
				refresh();
			}


			bool endDisabled = false;
			if (selectedFolder->parent == nullptr) {
				ImGui::BeginDisabled();
				endDisabled = true;
			}

			
			if (ImGui::Button("<")) {
				if (selectedFolder->parent != nullptr) {
					changeCurrentFolder(static_cast<Folder*>(selectedFolder->parent));
				}
			}

			if (endDisabled) {
				ImGui::EndDisabled();
			}

			ImGui::SameLine();

			for (int i = currentFolderPath.size() - 1; i > 0; i--) {
				if (ImGui::TextLink(currentFolderPath[i]->name.c_str())) {
					changeCurrentFolder(currentFolderPath[i]);
					break;
				}
				ImGui::SameLine();
				ImGui::Text("/");
				ImGui::SameLine();
			}
			ImGui::Text(selectedFolder->name.c_str());
			//ImGui::Text(selectedFolderPath.c_str());
			ImGui::BeginChild("Content", { 0, 0 }, ImGuiChildFlags_Borders);


			static bool isHoveringOverIcon = false;

			if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			{
				ImGui::OpenPopup("r");
				isHoveringOverIcon = false;
			}

			

			//ImVec2 childPos = ImGui::GetWindowPos();
			int width = ImGui::GetWindowWidth();
			const int ASSET_SIZE = 48;
			int r = width / ASSET_SIZE;

			if (r == 0) { r = 1; }


			ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign,
				ImVec2(0.0f, 0.0f));

			

			static FsItem* rightClickedItem = nullptr;

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0, 0 });
			if (selectedFolder != nullptr) {
				int i = 0;
				for (auto a : selectedFolder->content) {
					if (a.second->isDirectory() == true) {
						ImGui::PushID(i);
						if (i % r != 0) {
							ImGui::SameLine();
						}

						if (ImGui::Button(a.second->name.c_str(), { ASSET_SIZE, ASSET_SIZE })) {
							changeCurrentFolder(static_cast<Folder*>(a.second));
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
						changeCurrentFolder(&fs);
					}

				}
				else {
					if (ImGui::BeginMenu("Import")) {
						if (ImGui::MenuItem("Textures")) {
							Texture* texture = importTexture();
							if (texture != nullptr) {
								File* file = selectedFolder->addFile(texture->name);
								file->textureID = texture->glid;
							}
						}
						ImGui::MenuItem("Sounds");
						//ImGui::MenuItem("Materials");
						ImGui::MenuItem("Models");
						ImGui::MenuItem("Scripts");
						ImGui::EndMenu();
					}
					if (ImGui::MenuItem("New folder")) {
						logInfo("Path ", drishPath->parent_path() / "New folder");
						try
						{
							if (std::filesystem::create_directory(drishPath->parent_path() / "New folder")) {
								Folder* f = selectedFolder->addFolder("new folder");
							}
						}

						catch (std::filesystem::filesystem_error const& ex)
						{
							logError(ex.what());
						}
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
							Folder* folder = static_cast<Folder*>(rightClickedItem->parent);
							auto nh = folder->content.extract(rightClickedItem->name);
							nh.key() = str;
							folder->content.insert(std::move(nh));

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