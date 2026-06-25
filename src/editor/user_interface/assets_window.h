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

class AssetWindow: public EditorWindowBase
{
private:
	AssetRepository *assetRepository;

public:
	std::filesystem::path *drishPath = nullptr;

	AssetWindow(AssetRepository *assetRepository)
	{
		this->assetRepository = assetRepository;
	}

	template <typename T>
	void drawList(std::string name, std::string payloadName, std::vector<AssetSlot<T>*> slots,
		std::function<void(int)> deleteFunction, std::function<void()> createFunction) {


		if (ImGui::TreeNodeEx(name.c_str()))
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

	void draw() override
	{
		if (open)
		{
			ImGui::Begin("Assets", &open);
	
			drawList<Texture>(
				"Textures",
				"TEXTURE",
				assetRepository->textures.slots,
				[this](int index) {
					assetRepository->textures.remove(index);
				},
				[this]() {
					{
						std::filesystem::path texturePath = drishengine::openImageOpenFileDialog();
						if (!texturePath.empty())
						{
							std::filesystem::path filename = texturePath.filename();

							if (std::filesystem::exists(drishPath->parent_path() / "textures" / filename)) {
								logInfo("[ASSETS WINDOW] file already exists");
							}
							else {
								Texture* texture = new Texture();

								texture->name = filename.string();
								texture->path = std::filesystem::path("textures") / filename;

								ImageLoader::loadImage(texturePath, texture);
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
									std::filesystem::copy_file(texturePath, drishPath->parent_path() / "textures" / filename);
								}
								else
								{
									logError("[ASSETS WINDOW] drishPath is null");
								}
							}
						}
					}
				}
			);


			drawList<Vertices>(
				"Vertices",
				"VERTICES",
				assetRepository->vertices.slots,
				[this](int index) {
					assetRepository->vertices.remove(index);
				},
				[this]() {
					std::filesystem::path objFilePath = drishengine::openModelOpenFileDialog();

					if (!objFilePath.empty())
					{
						std::filesystem::path filename = objFilePath.filename();
						filename.replace_extension("model");

						Vertices* vertices = new Vertices();
						vertices->name = objFilePath.stem().string();

						vertices->path = std::filesystem::path("models") / filename;
						drishengine::loadObj(objFilePath, vertices);

						assetRepository->vertices.add(vertices);

						if (drishPath != nullptr)
						{
							if (std::filesystem::exists(drishPath->parent_path() / "models"))
							{
								logDebug("[ASSETS WINDOW] models exists");
							}
							else
							{
								std::filesystem::create_directory(drishPath->parent_path() / "models");
							}

							drishengine::writeModelData(drishPath->parent_path() / "models" / filename,
								vertices->data);
							vertices->createBuffers();
						}
						else
						{
							logError("[ASSETS WINDOW] drishPath is null");
						}
						}
					}
				);


			drawList<Material>(
				"Materials",
				"MATERIAL",
				assetRepository->materials.slots,
				[this](int index) {
					assetRepository->materials.remove(index);
				},
				[this]() {
					Material* mat = new Material();
					mat->name = "New material";
					mat->shader = &assetRepository->defaultShader;
					assetRepository->materials.add(mat);
				}
			);

			drawList<Script>(
				"Scripts",
				"SCRIPT",
				assetRepository->scripts.slots,
				[this](int index) {
					assetRepository->scripts.remove(index);
				},
				[this]() {
					Script* script = new Script();
					script->name = "New script";
					assetRepository->scripts.add(script);
				}
				);

			drawList<Sound>(
				"Sounds",
				"SOUND",
				assetRepository->sounds.slots,
				[this](int index) {
					assetRepository->sounds.remove(index);
				},
				[this]() {
					Sound* sound = new Sound();
					sound->name = "New sound";
					assetRepository->sounds.add(sound);
				}
				);

	
			ImGui::End();
		}
	}
};