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

	virtual bool isDirectory() {
		return false;
	};
};

class File : public FsItem
{
	bool isDirectory() override {
		return false;
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


	Folder* selectedFolder = nullptr;
	

	Folder fs = Folder("project");

	AssetWindow(AssetRepository *assetRepository)
	{
		this->assetRepository = assetRepository;

		fs.addFolder("test");
		fs.addFolder("test2");
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


		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("New folder")) {
				Folder* f = folder->addFolder("new folder");
				selectedFolder = f;
			}
			if (ImGui::BeginMenu("Import")) {
				ImGui::MenuItem("Textures (.png, .jpg...)");
				ImGui::MenuItem("Sounds (.wav)");
				ImGui::MenuItem("Materials (.mat)");
				ImGui::MenuItem("Models (.obj)");
				ImGui::MenuItem("Scripts");
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Rename")) {
			}
			if (ImGui::MenuItem("Delete")) {
				selectedFolder = nullptr;
			}


			ImGui::EndPopup();
		}

		if (ImGui::IsItemClicked()) {
			selectedFolder = folder;
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

	void draw() override
	{
		if (open)
		{
			ImGui::Begin("Assets", &open);
	

			

			ImGui::BeginChild("Tree", { 200, 0 }, ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX, ImGuiWindowFlags_HorizontalScrollbar);
			drawFolderContents(&fs);
			ImGui::EndChild();
			ImGui::SameLine();
			ImGui::BeginChild("Content", { 0, 0 }, ImGuiChildFlags_Borders);

			//ImVec2 childPos = ImGui::GetWindowPos();
			int width = ImGui::GetWindowWidth();
			const int ASSET_SIZE = 32;
			int r = width / ASSET_SIZE;

			if (r == 0) { r = 1; }

			for (int i = 1; i < 100; i++) {
				ImGui::PushID(i);
				ImGui::Button("test", { ASSET_SIZE, ASSET_SIZE });
				if (i % r != 0) {
					ImGui::SameLine();
				}
				ImGui::PopID();
			}

			/*if (selectedFolder != nullptr) {
				for (auto a : selectedFolder->content) {
					if (a.second->isDirectory() == true) {
						if (ImGui::Button(a.first.c_str(), { 64, 64 })) {
							selectedFolder = static_cast<Folder*>(a.second);
						}
					}
				}
			}*/
			
			ImGui::EndChild();
	
			ImGui::End();
		}
	}
};