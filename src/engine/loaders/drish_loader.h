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
			if (json["ver_b"] != DRISH_ENGINE_VERSION_BIG || json["ver_s"] != DRISH_ENGINE_VERSION_SMALL
				|| json["ver_nbns"] != DRISH_ENGINE_VERSION_NOTBIGNOTSMALL) {
				logWarning("[LOADER] Versions are different. Drish file version is ",
					json["ver_b"], ".", json["ver_nbns"], ".", json["ver_s"], " and engine version is ",
					DRISH_ENGINE_VERSION_BIG, ".", DRISH_ENGINE_VERSION_NOTBIGNOTSMALL, ".",
					DRISH_ENGINE_VERSION_SMALL);

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
				case Type::CAMERA:
				{
					Camera* camera = static_cast<Camera*>(node);
					camera->fov = item.value()["fov"];
					camera->f_near = item.value()["f_near"];
					camera->f_far = item.value()["f_far"];
					camera->active = item.value()["active"];
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
				if (item.value().contains("script_index")) {
					if (!item.value()["script_index"].is_null()) {
						node->scriptHandle.index = item.value()["script_index"];
					}
				}
				

				if (node->type == Type::MODEL) {
					Model* model = static_cast<Model*>(node);
					if (item.value().contains("material_index")) {
						if (!item.value()["material_index"].is_null()) {	
							model->materialHandle.index = item.value()["material_index"];
						}
					}
					else {
						logError("[DRISH LOADER] Node type is model but no material_index");
					}

					if (!item.value()["vertices_index"].is_null()) {
						model->verticesHandle.index = item.value()["vertices_index"];
					}
				}
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
		logInfo("[DRISH_LOADER] load assets");

		if (json.contains("scripts")) {
			for (const auto& item : json["scripts"].items())
			{
				Script* script = new Script();
				script->name = item.value()["name"];
				script->source = item.value()["source"];
				assetRepository->scripts.appendNewSlot(script);
			}
		}


		if (json.contains("vertices")) {
			for (const auto& item : json["vertices"].items())
			{
				Vertices* vertices = new Vertices();
				vertices->name = item.value()["name"];
				std::string strpath = item.value()["path"];
				vertices->path = std::filesystem::path(strpath);
				drishengine::loadModelData(projectPath / vertices->path, &vertices->data);
				vertices->createBuffers();

				assetRepository->vertices.appendNewSlot(vertices);
			}
		}


		if (json.contains("textures")) {
			for (const auto& item : json["textures"].items())
			{
				Texture* texture = new Texture();
				texture->name = item.value()["name"];
				std::string strpath = item.value()["path"];
				texture->path = std::filesystem::path(strpath);
				ImageLoader::loadImage(projectPath / texture->path, texture);

				assetRepository->textures.appendNewSlot(texture);
			}
		}


		if (json.contains("materials")) {
			for (const auto& item : json["materials"].items())
			{
				Material* material = new Material();
				material->name = item.value()["name"];
				material->color.r = item.value()["color"]["r"];
				material->color.g = item.value()["color"]["g"];
				material->color.b = item.value()["color"]["b"];
				material->shader = &assetRepository->defaultShader;

				assetRepository->materials.appendNewSlot(material);
					
				
				if (!item.value()["texture_index"].is_null()) {
					material->textureHandle.index = item.value()["texture_index"];
				}
			}
		}
	}
};