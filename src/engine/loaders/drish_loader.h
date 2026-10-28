#pragma once

#define JSON_DIAGNOSTICS 1

#include <filesystem>

#include "../../version.h"
#include "../asset_repository.h"
#include "../game_config.h"
#include "../node_creator.h"
#include "../game_object_types.h"
#include "../../consts.h"
#include "../file_manager.h"

class Node;


//Loads data from .drish file;
class DrishLoader {
public:
	static int load(std::filesystem::path drishPath,
		Node* root, AssetRepository* assetRepository,
		GameConfig* gameConfig, NodeRepository* nodeRepository,
		FileManager* fileManager,
		const char* drishData,
		uint64_t dataLength) {
		logDebug("[DRISH LOADER] ", "Load ", drishPath.string());

		try {
			nlohmann::json json = nlohmann::json::parse(drishData, drishData + dataLength);

			logDebug("[DRISH LOADER] ", ".drish version: ", json[JSON_VERSION_MAJOR_KEY_NAME], ".", json[JSON_VERSION_MINOR_KEY_NAME]);

			if (json[JSON_VERSION_MAJOR_KEY_NAME] != DRISH_ENGINE_VERSION_MAJOR ||
				json[JSON_VERSION_MINOR_KEY_NAME] != DRISH_ENGINE_VERSION_MINOR) {
				logWarning("[DRISH LOADER] Versions are different. Drish file version is ",
					json[JSON_VERSION_MAJOR_KEY_NAME], ".", json[JSON_VERSION_MINOR_KEY_NAME], " and engine version is ",
					DRISH_ENGINE_VERSION_MAJOR, ".", DRISH_ENGINE_VERSION_MINOR);

			}

			gameConfig->width = json[JSON_GAME_CONFIG_KEY_NAME]["width"];
			gameConfig->height = json[JSON_GAME_CONFIG_KEY_NAME]["height"];
			gameConfig->title = json[JSON_GAME_CONFIG_KEY_NAME]["title"];
			gameConfig->useFullscreen = json[JSON_GAME_CONFIG_KEY_NAME]["useFullscreen"];

			std::map<long long, Node*> nodes;
			nodes[0] = root;
		
			nextNodeId = json[JSON_NEXT_NODE_ID_KEY_NAME];

			for (const auto& item : json["nodes"].items()) {
				long long id = item.value()["id"];
				Node* node = NodeCreator::createNode(item.value()["type"], item.value()["Name"], nodeRepository, id);

				for (Field field : node->getFields()) {
					if (item.value().contains(field.name)) {
						switch (field.type)
						{
						case FieldType::String:
							*static_cast<std::string*>(field.ptr) = item.value()[field.name];
							break;
						case FieldType::Int:
							*static_cast<int*>(field.ptr) = item.value()[field.name];
							break;
						case FieldType::Float:
							*static_cast<float*>(field.ptr) = item.value()[field.name];
							break;
						case FieldType::Boolean:
							*static_cast<bool*>(field.ptr) = item.value()[field.name];
							break;
						case FieldType::Color3:
						case FieldType::FloatVec3:
						{
							glm::vec3 vec;
							vec[0] = item.value()[field.name][0];
							vec[1] = item.value()[field.name][1];
							vec[2] = item.value()[field.name][2];
							*static_cast<glm::vec3*>(field.ptr) = vec;
							break;
						}
						case FieldType::MaterialHandle:
						case FieldType::TextureHandle:
						case FieldType::VerticesHandle:
						case FieldType::SoundHandle:
						case FieldType::ScriptHandle:
						{
							//AssetHandle* handle = static_cast<AssetHandle*> (field.ptr);
							logDebug(SOURCE_LOCATION, "Field name", field.name);
							logDebug(SOURCE_LOCATION, "Getting file name", item.value()[field.name]);
							auto file = fileManager->getFile(item.value()[field.name]);
							if (file.has_value()) {
								logInfo(SOURCE_LOCATION, "File handle name", file.value()->name);
								
								*static_cast<AssetHandle*>(field.ptr) = *file.value()->assetHandle;
								//*handle = *(file.value()->assetHandle);
							}
							else {
								logError(SOURCE_LOCATION, "File manager cannot get file named ",
									item.value()[field.name]);
							}
							
							break;
						}


						default:
							break;
						}
					}

					
				}
				if (node->type == Type::CAMERA) {
					Camera* camera = static_cast<Camera*>(node);
					camera->setWindowAspectRatio(gameConfig->width, gameConfig->height);
				}

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
			}

		}
		catch (const nlohmann::json::parse_error& ex) {
			logError("[DRISH LOADER] Drish file loading failed");
			logError("[DRISH LOADER] ", ex.what(), " ", ex.byte);
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
};