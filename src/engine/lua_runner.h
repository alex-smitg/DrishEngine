#pragma once

#include <sol/sol.hpp>
#include <glm/glm.hpp>

#include "window.h"

#include "asset_repository.h"

class Node;
class NodeRepository;

class LuaRunner {
public:
	std::string lastError = "";
	sol::state lua;

	LuaRunner();
	void setGetKey(drishengine::Window* window);
	void addCreateNodesFunctions(NodeRepository* nodeRepository);
	void addAssetRepositoryFunctions(AssetRepository* assetRepository);
	void updateNodesScriptEnvironment(Node* node, AssetRepository* assetRepository);
};  