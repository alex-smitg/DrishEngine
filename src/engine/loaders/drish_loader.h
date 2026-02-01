#pragma once


#include <filesystem>

#include "../../version.h"
#include "../asset_repository.h"
#include "../game_config.h"
#include "../node_creator.h"
#include "../game_object_types.h"


class Node;


//Loads data from .drish file;
class DrishLoader {
public:
	static int load(std::filesystem::path drishPath,
		Node* world, AssetRepository* assetRepository,
		GameConfig* gameConfig, NodeRepository* nodeRepository) {
		logDebug("[LOADER] ", "Load ", drishPath.string());

		if (!std::filesystem::exists(drishPath)) {
			logDebug("[LOADER] .drish where?");
			return 0;
		}

	
		std::ifstream js(drishPath);
		try {
			nlohmann::json json = nlohmann::json::parse(js);

			loadAssets(json, drishPath.parent_path(), assetRepository);

			logDebug("[LOADER] ", ".drish version: ", json["version"]);
			if (json["version"] != DRISH_ENGINE_VERSION) {
				logWarning("[LOADER] Versions: ", json["version"], " != ", DRISH_ENGINE_VERSION);
			}

			gameConfig->width = json["gameConfig"]["width"];
			gameConfig->height = json["gameConfig"]["height"];
			gameConfig->title = json["gameConfig"]["title"];
			gameConfig->useFullscreen = json["gameConfig"]["useFullscreen"];

			std::map<long long, Node*> nodes;
			nodes[0] = world;
		
			nextNodeId = json["nextNodeId"];


			for (const auto& item : json["nodes"].items())
			{
				long long id = item.value()["id"];
				Node* node = NodeCreator::createNode(item.value()["type"], item.value()["name"], nodeRepository, id);
				switch (node->type)
				{
				case Type::MODEL:
					static_cast<Model*>(node)->material = assetRepository->defaultMaterial;
					break;
				case Type::POINT_LIGHT:
				{
					PointLight* pointLight = static_cast<PointLight*>(node);
					pointLight->color = glm::vec3(item.value()["color"]["r"],
						item.value()["color"]["g"],
						item.value()["color"]["b"]);
					pointLight->radius = item.value()["radius"];
					pointLight->strength = item.value()["strength"];
					
					break;

				}
				default:
					break;
				}
				node->transform = item.value()["transform"].get<Transform>();


				if (nodes.contains(id)) {
					logError("[LOADER] Same ids");
					throw 1;
				}

				nodes[id] = node;
			}


			for (const auto& item : json["nodes"].items())
			{
				long long id = item.value()["id"];
				Node* node = nodes[id];
				nodes[item.value()["parent"]]->appendChild(node);
				if (item.value().contains("script_assId")) {
					node->script = assetRepository->getScript(item.value()["script_assId"]);
					logDebug("[DRISH LOADER] Script ASS ID: ", item.value()["script_assId"]);
				}

				if (item.value().contains("materialAssId")) {
					Model* model = static_cast<Model*>(node);
					model->material = assetRepository->getMaterial(item.value()["materialAssId"]);
					logDebug("[DRISH LOADER] Material ASS ID: ", item.value()["materialAssId"]);
				}

				if (item.value().contains("verticesAssId")) {
					Model* model = static_cast<Model*>(node);
					if (!assetRepository->verticesMap.contains(item.value()["verticesAssId"])) {
						logError("[DRISH LOADER] getVertices: ", item.value()["verticesAssId"], " DO NOT EXISTS");
					}
					else {
						model->vertices = assetRepository->getVertices(item.value()["verticesAssId"]);
						logDebug("[DRISH LOADER] Vertices ASS ID: ", item.value()["verticesAssId"]);
					}
				}

			}
		}
		catch (...) {
			logError("[DRISH LOADER] Drish file loading failed");
		}
		return 1;
	}

private:
	static void loadAssets(nlohmann::json& json, std::filesystem::path projectPath, AssetRepository* assetRepository) {
		nextAssetId = json["nextAssetId"];
		logDebug("[DRISH LOADER] ", "nextAssetId ", nextAssetId);

		for (const auto& item : json["textures"].items())
		{
			Texture* texture = AssetCreator::createTexture(item.value()["name"], item.value()["assId"]);
			std::string strpath = item.value()["path"];
			texture->path = std::filesystem::path(strpath);
			ImageLoader::loadImage(projectPath / texture->path, texture);

			assetRepository->addTexture(texture);
		}

		for (const auto& item : json["scripts"].items())
		{
			Script* script = AssetCreator::createScript(item.value()["name"], item.value()["assId"]);
			script->source = item.value()["source"];

			assetRepository->addScript(script);
		}
		for (const auto& item : json["vertices"].items())
		{
			Vertices* vertices = AssetCreator::createVertices(item.value()["name"], item.value()["assId"]);
			std::string strpath = item.value()["path"];
			vertices->path = std::filesystem::path(strpath);
			if (!std::filesystem::exists(projectPath / vertices->path)) {
				logError("[DRISH LOADER] ", vertices->path, " path does not exist");
			}
			drishengine::loadModelData(projectPath / vertices->path, &vertices->data);
			vertices->createBuffers();

			assetRepository->addVertices(vertices);
		}

		for (const auto& item : json["materials"].items())
		{
			Material* material = AssetCreator::createMaterial(item.value()["name"], item.value()["assId"]);
			material->color.r = item.value()["color"]["r"];
			material->color.g = item.value()["color"]["g"];
			material->color.b = item.value()["color"]["b"];
			material->shader = &assetRepository->defaultShader;
			if (item.value().contains("textureAssId")) {
				material->texture = assetRepository->getTexture(item.value()["textureAssId"]);
			}

			assetRepository->addMaterial(material);
		}
	}
};