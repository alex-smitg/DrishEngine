#pragma once

#include <map>

#include "asset_repository.h"
#include "file_manager.h"
#include "loaders/model_loader.h"
#include "audio_manager.h"

#include "miniaudio.h"

class Importer
{
public:
	static void importVerticesFile(File* file, AssetRepository* assetRepository) {
		if (file->imported == false) {
			logDebug(SOURCE_LOCATION, "importing vertices", file->name);

			auto ver = assetRepository->vertices.get(file->assetHandle);

			Vertices* vertices = nullptr;

			if (ver.has_value()) {
				vertices = ver.value();
			}
			else {
				vertices = new Vertices();
				vertices->name = file->name;
				file->assetHandle = assetRepository->vertices.add(vertices);
				drishengine::loadObj(file->data.data(), file->data.size(), vertices);
				vertices->createBuffers();
			}

			file->type = FileType::MODEL;
			file->imported = true;
			


		}
	}


	static void importScriptFile(File *file, AssetRepository *assetRepository)
	{
		if (file->imported == false)
		{
			logDebug(SOURCE_LOCATION, "importing script", file->name);

			Script *script = nullptr;

			if (file->assetHandle != nullptr)
			{
				auto scr = assetRepository->scripts.get(file->assetHandle);
				if (scr.has_value())
				{
					script = scr.value();
					script->source = std::string(file->data.data(), file->data.size());
				}
			}

			if (!script)
			{

				script = new Script();
				script->name = file->name;
				script->source = std::string(file->data.data(), file->data.size());

				file->assetHandle = assetRepository->scripts.add(script);
			}

			if (script)
			{
				file->type = FileType::SCRIPT;
				file->imported = true;
			}
		}
	}

	static void importTextureFile(File *file,
		AssetRepository *assetRepository,
		std::map<std::string, AssetHandle*> &textures)
	{
		if (file->imported == false)
		{
			logDebug(SOURCE_LOCATION, "importing texture, ", file->name);

			Texture *texture = nullptr;

			if (file->assetHandle != nullptr)
			{
				auto tex = assetRepository->textures.get(file->assetHandle);
				if (tex.has_value())
				{
					texture = tex.value();

					logDebug("[IMPORTER] Freeing previous texture");
					glDeleteTextures(1, &texture->glid);
				}
			}

			if (!texture)
			{
				texture = new Texture();
				texture->name = file->name;
			}

			ImageLoaderError err = ImageLoader::loadImage((unsigned char *)file->data.data(), file->data.size(), texture);
			if (err != ImageLoaderError::OK)
			{
				delete texture;
				logError(SOURCE_LOCATION, "Error loading: ", file->name);
			}
			else
			{
				if (file->assetHandle == nullptr)
				{
					file->assetHandle = assetRepository->textures.add(texture);

					textures[file->name] = file->assetHandle;


				}
				// textures[texture->name] = file->assetHandle;

				file->type = FileType::IMAGE;
				file->icon = texture->glid;
				file->imported = true;
			}
		}
	}

	static void importAll(FileManager *fileManager, AssetRepository *assetRepository, AudioManager* audioManager)
	{

		std::map<std::string, AssetHandle*> loadedTextures;

		//Loading textures first for later usage for materials;
		for (auto const& pair : fileManager->getProjectFiles())
		{
			File* file = pair.second;

			if (file->imported == false)
			{
				if (file->extension == ".png" ||
					file->extension == ".jpg" ||
					file->extension == ".tga" ||
					file->extension == "jpeg")
				{
					Importer::importTextureFile(file, assetRepository, loadedTextures);
				}
			}
		}



		logInfo(SOURCE_LOCATION, "import all");
		for (auto const &pair : fileManager->getProjectFiles())
		{
			File *file = pair.second;




			if (file->imported == false)
			{
				if (file->extension == ".mat") {
					Material* material = nullptr;

					if (file->assetHandle)
					{
						auto mat = assetRepository->materials.get(file->assetHandle);
						if (mat.has_value())
						{
							material = mat.value();
						}
					}

					if (material == nullptr)
					{
						logDebug(SOURCE_LOCATION, "material is nullptr, creating new");
						material = new Material();
						material->name = file->name;
						material->shader = &assetRepository->defaultShader;
						file->assetHandle = assetRepository->materials.add(material);
					}

					if (material)
					{
						nlohmann::json json = nlohmann::json::parse(file->data.data(), file->data.data() +
							file->data.size());

						material->color.r = json["color"]["r"];
						material->color.g = json["color"]["g"];
						material->color.b = json["color"]["b"];
						material->shine = json["shine"];
						material->useLight = json["useLight"];
						if (json.contains("texture")) {
							std::string textureName = json["texture"];

							if (auto it = loadedTextures.find(textureName); it != loadedTextures.end()) {
								material->textureHandle = *it->second;
							}
							else {
								logError(SOURCE_LOCATION, "texture", textureName, "not found");
							}
							logDebug(SOURCE_LOCATION, material->name, "require texture", textureName);
						}

						file->imported = true;
						file->type = FileType::MATERIAL;
					}
				}

				if (file->extension == ".lua")
				{
					Importer::importScriptFile(file, assetRepository);
				}

				if (file->extension == ".obj") {
					Importer::importVerticesFile(file, assetRepository);
				
				}

				if (file->extension == ".mp3" ||
					file->extension == ".wav") {

					Sound* sound = new Sound();

					sound->name = file->name;

					ma_decoder_init_memory(file->data.data(), file->data.size(), NULL, &sound->decoder);
					ma_sound_init_from_data_source(&audioManager->engine, &sound->decoder, NULL, NULL, &sound->sound);
				
					//ma_sound_start(&sound->sound);

					file->imported = true;
					file->type = FileType::SOUND;

					file->assetHandle = assetRepository->sounds.add(sound);
				}
			}
		}
	}
};