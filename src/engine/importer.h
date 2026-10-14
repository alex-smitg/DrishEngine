#pragma once

#include <map>

#include "asset_repository.h"
#include "file_manager.h"

class Importer {
public:

	static void importTextureFile(File* file, AssetRepository* assetRepository) {
		if (file->imported == false) {
			Texture* texture = new Texture();
			texture->name = file->name;

			ImageLoaderError err = ImageLoader::loadImage((unsigned char*)file->data, file->dataSize, texture);
			if (err != ImageLoaderError::OK) {
				delete texture;
				logError("Error loading: ", file->name);
			}
			else {
				file->assetHandle = assetRepository->textures.add(texture);
				//textures[texture->name] = file->assetHandle;

				file->type = FileType::IMAGE;
				file->icon = texture->glid;
				file->imported = true;
			}
		}
	}

	static void importAll(FileManager* fileManager, AssetRepository* assetRepository) {
		logInfo("[IMPORTER] import all");
		for (auto const& pair : fileManager->getProjectFiles()) {
			File* file = pair.second;

			if (file->imported == false) {
				if (file->extension == ".mat") {
					Material* material = nullptr;

					if (file->assetHandle) { 
						auto mat = assetRepository->materials.get(file->assetHandle);
						if (mat.has_value()) {
							material = mat.value();
						}
					}
					
					if (material == nullptr) { 
						logDebug("[IMPORTER] material is nullptr, creating new");
						material = new Material();
						material->name = file->name;
						material->shader = &assetRepository->defaultShader;
						file->assetHandle = assetRepository->materials.add(material);
					}

					if (material) {
						nlohmann::json json = nlohmann::json::parse(pair.second->data, pair.second->data +
							pair.second->dataSize);

						material->color.r = json["color"]["r"];
						material->color.g = json["color"]["g"];
						material->color.b = json["color"]["b"];
						material->shine = json["shine"];
						material->useLight = json["useLight"];

						file->imported = true;
						file->type = FileType::MATERIAL;
				

					}
				}
				if (file->extension == ".png" || 
					file->extension == ".jpg" ||
					file->extension == ".tga" ||
					file->extension == "jpeg") {
					Importer::importTextureFile(file, assetRepository);
				}

			}

			

		}

	}

};