#pragma once

#define JSON_DIAGNOSTICS 1

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
				/*if (item.value().contains("script_assId")) {
					node->script = assetRepository->getScript(item.value()["script_assId"]);
					logDebug("[DRISH LOADER] Script ASS ID: ", item.value()["script_assId"]);
				}*/
				

				//will cause problems in future if other nodes will use material_index
				if (item.value().contains("material_index")) {
					Model* model = static_cast<Model*>(node);
					if (!item.value()["material_index"].is_null()) {
						model->materialHandle.index = item.value()["material_index"];
					}
					
				}
				/*
				if (item.value().contains("verticesAssId")) {
					Model* model = static_cast<Model*>(node);
					if (!assetRepository->verticesMap.contains(item.value()["verticesAssId"])) {
						logError("[DRISH LOADER] getVertices: ", item.value()["verticesAssId"], " DO NOT EXISTS");
					}
					else {
						model->vertices = assetRepository->getVertices(item.value()["verticesAssId"]);
						logDebug("[DRISH LOADER] Vertices ASS ID: ", item.value()["verticesAssId"]);
					}
				}*/

			}
		}
		catch (const std::exception& ex) {
			logError("[DRISH LOADER] Drish file loading failed");
			logError("[DRISH LOADER] ", ex.what());
		}
		catch (...) {
			logError("[DRISH LOADER] Drish file loading failed");
		}
		return 1;
	}

private:
	static void loadAssets(nlohmann::json& json, std::filesystem::path projectPath, AssetRepository* assetRepository) {
		//for (const auto& item : json["textures"].items())
		//{
		//	Texture* texture = new Texture(item.value()["assId"]);
		//	texture->name = item.value()["name"];
		//	std::string strpath = item.value()["path"];
		//	texture->path = std::filesystem::path(strpath);
		//	ImageLoader::loadImage(projectPath / texture->path, texture);

		//	//assetRepository->addTexture(texture);
		//}

		//for (const auto& item : json["scripts"].items())
		//{
		//	Script* script = new Script(item.value()["assId"]);
		//	script->name = item.value()["name"];
		//	script->source = item.value()["source"];

		//	//assetRepository->addScript(script);
		//}
		//for (const auto& item : json["vertices"].items())
		//{
		//	Vertices* vertices = new Vertices(item.value()["assId"]);
		//	vertices->name = item.value()["name"];
		//	std::string strpath = item.value()["path"];
		//	vertices->path = std::filesystem::path(strpath);
		//	if (!std::filesystem::exists(projectPath / vertices->path)) {
		//		logError("[DRISH LOADER] ", vertices->path, " path does not exist");
		//	}
		//	drishengine::loadModelData(projectPath / vertices->path, &vertices->data);
		//	vertices->createBuffers();

		//	//assetRepository->addVertices(vertices);
		//}

		if (json.contains("materials")) {
			for (const auto& item : json["materials"].items())
			{
				if (item.value().empty()) {
					AssetSlot<Material>* con = new AssetSlot<Material>();
					con->index = assetRepository->materials.size();
					assetRepository->materials.slots.push_back(con);
				}
				else {
					Material* material = new Material();
					material->name = item.value()["name"];
					material->color.r = item.value()["color"]["r"];
					material->color.g = item.value()["color"]["g"];
					material->color.b = item.value()["color"]["b"];
					material->shader = &assetRepository->defaultShader;

					AssetSlot<Material>* con = new AssetSlot<Material>();
					con->index = assetRepository->materials.size();
					assetRepository->materials.slots.push_back(con);
					con->asset = material;
					con->is_valid = true;
					
				}
				//if (item.value().contains("textureAssId")) {
					//material->texture = assetRepository->getTexture(item.value()["textureAssId"]);
				//}

				//assetRepository->addMaterial(material);
			}
		}
	}
};