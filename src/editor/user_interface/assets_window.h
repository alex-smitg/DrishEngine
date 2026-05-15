#pragma once

#include <string>
#include <filesystem>

#include "imgui_docking/imgui.h"
#include "imgui_docking/imgui_impl_glfw.h"
#include "imgui_docking/imgui_impl_opengl3.h"

#include "../../engine/asset_repository.h"
#include "../../engine/loaders/image_loader.h"

class AssetWindow
{
private:
	AssetRepository *assetRepository;

public:
	bool open = true;

	std::filesystem::path *drishPath = nullptr;

	AssetWindow(AssetRepository *assetRepository)
	{
		this->assetRepository = assetRepository;
	}

	void draw()
	{
		if (open)
		{
			ImGui::Begin("Assets", &open);
			if (ImGui::TreeNodeEx("Textures"))
			{
			//	for (auto const &pair : assetRepository->texturesMap)
			//	{
			//		std::shared_ptr<Texture> texture = pair.second;
			//		ImGui::Text("assID: %d", texture->assId);
			//		ImGui::SameLine();
			//		ImGui::Spacing();
			//		ImGui::SameLine();
			//		ImGui::Text(texture->name.c_str());
			//		ImGui::PushID(texture->assId);
			//		ImGui::ImageButton("Texture", texture->glid, ImVec2(64, 64));
			//		if (ImGui::BeginDragDropSource())
			//		{
			//			ImGui::SetDragDropPayload("TEXTURE", &texture->assId, sizeof(texture->assId));
			//			ImGui::Text(texture->name.c_str());
			//			ImGui::EndDragDropSource();
			//		}
			//		ImGui::PopID();
			//	}

			//	if (ImGui::Button("+", ImVec2(64, 64)))
			//	{
			//		/*std::filesystem::path texturePath = drishengine::openImageOpenFileDialog();
			//		if (!texturePath.empty())
			//		{
			//			std::filesystem::path filename = texturePath.filename();
			//			Texture* texture = new Texture(-1);
			//			texture->name = filename.string();
			//			ImageLoader::loadImage(texturePath, texture);
			//			assetRepository->addTexture(texture);
			//		}*/
			//	}

				ImGui::TreePop();
			}

			if (ImGui::TreeNodeEx("Vertices"))
			{
				//for (auto const &pair : assetRepository->verticesMap)
				//{
				//	std::shared_ptr<Vertices> vertices = pair.second;
				//	ImGui::Text("assID: %d", vertices->assId);
				//	ImGui::SameLine();
				//	ImGui::Spacing();
				//	ImGui::SameLine();
				//	ImGui::Text(vertices->name.c_str());
				//	ImGui::SameLine();
				//	ImGui::PushID(vertices->assId);
				//	ImGui::Button("Drag");
				//	if (ImGui::BeginDragDropSource())
				//	{
				//		ImGui::SetDragDropPayload("VERTICES", &vertices->assId, sizeof(vertices->assId));
				//		ImGui::Text(vertices->name.c_str());
				//		ImGui::EndDragDropSource();
				//	}
				//	ImGui::PopID();
				//}
				////if (ImGui::Button("+", ImVec2(64, 64)))
				////{
				////	std::filesystem::path objFilePath = drishengine::openModelOpenFileDialog();

				////	if (!objFilePath.empty())
				////	{
				////		std::filesystem::path filename = objFilePath.filename();
				////		filename.replace_extension("model");

				////		Vertices *vertices = new Vertices(-1);
				////		vertices->name = objFilePath.stem().string();

				////		vertices->path = std::filesystem::path("models") / filename;
				////		drishengine::loadObj(objFilePath, vertices);
				////		//assetRepository->addVertices(vertices);

				////		if (drishPath != nullptr)
				////		{
				////			if (std::filesystem::exists(drishPath->parent_path() / "models"))
				////			{
				////				logDebug("[ASSETS WINDOW] models exists");
				////			}
				////			else
				////			{
				////				std::filesystem::create_directory(drishPath->parent_path() / "models");
				////			}

				////			drishengine::writeModelData(drishPath->parent_path() / "models" / filename,
				////										vertices->data);
				////			vertices->createBuffers();
				////		}
				////		else
				////		{
				////			logError("[ASSETS WINDOW] drishPath is null");
				////		}
				////	}
				////}

				ImGui::TreePop();
			}
			if (ImGui::TreeNodeEx("Materials"))
			{
				
				for (const AssetSlot<Material> * assetSlot : assetRepository->materials.slots)
				{
					if (assetSlot->is_valid) {
						Material* material = assetSlot->asset;


						ImGui::Text("%d", assetSlot->index);
						ImGui::SameLine();
						ImGui::Text(material->name.c_str());
						ImGui::SameLine();
						ImGui::PushID(material->assId);
						ImGui::Button("Drag");
						if (ImGui::BeginDragDropSource())
						{
							ImGui::SetDragDropPayload("MATERIAL", &assetSlot->index, sizeof(assetSlot->index));
							ImGui::Text("%d. %s", assetSlot->index, material->name.c_str());
							ImGui::EndDragDropSource();
						}
						ImGui::SameLine();
						if (ImGui::Button("Delete")) {
							assetRepository->materials.remove(assetSlot->index);
						}
						ImGui::PopID();
					}
				}

				if (ImGui::Button("Create new"))
				{
					Material* mat = new Material(AUTO_INCREMENT_ID);
					mat->name = "New material";
					mat->shader = &assetRepository->defaultShader;
					assetRepository->materials.add(mat);
					
				}

				ImGui::TreePop();
			}

			if (ImGui::TreeNodeEx("Scripts"))
			{
				//for (auto const &pair : assetRepository->scriptsMap)
				//{
				//	std::shared_ptr<Script> script = pair.second;
				//	ImGui::Text("assID: %d", script->assId);
				//	ImGui::SameLine();
				//	ImGui::Spacing();
				//	ImGui::SameLine();
				//	ImGui::Text(script->name.c_str());
				//	ImGui::SameLine();
				//	ImGui::PushID(script->assId);
				//	ImGui::Button("Drag");
				//	if (ImGui::BeginDragDropSource())
				//	{
				//		ImGui::SetDragDropPayload("SCRIPT", &script->assId, sizeof(script->assId));
				//		ImGui::Text(script->name.c_str());
				//		ImGui::EndDragDropSource();
				//	}
				//	ImGui::PopID();
				//}

				///*if (ImGui::Button("+", ImVec2(64, 64)))
				//{
				//	Script* script = new Script(AUTO_INCREMENT_ID);
				//	script->name = "Script";
				//	assetRepository->addScript(script);
				//}*/

				ImGui::TreePop();
			}

			if (ImGui::TreeNodeEx("Sounds"))
			{
				/*for (auto const &pair : assetRepository->soundsMap)
				{
					std::shared_ptr<Sound> sound = pair.second;
					ImGui::Text("assID: %d", sound->assId);
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Text(sound->name.c_str());
					ImGui::SameLine();
					ImGui::PushID(sound->assId);
					ImGui::Button("Drag");
					if (ImGui::BeginDragDropSource())
					{
						ImGui::SetDragDropPayload("SOUND", &sound->assId, sizeof(sound->assId));
						ImGui::Text(sound->name.c_str());
						ImGui::EndDragDropSource();
					}
					ImGui::PopID();
				}*/

				/*if (ImGui::Button("+", ImVec2(64, 64)))
				{
					Sound *sound = new Sound(AUTO_INCREMENT_ID);
					assetRepository->addSound(sound);
				}*/

				ImGui::TreePop();
			}

			ImGui::End();
		}
	}
};