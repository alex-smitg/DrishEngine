#pragma once

#include <sol/sol.hpp>
#include <glm/glm.hpp>

#include "window.h"

class Node;

class LuaRunner {
public:
	std::string lastError = "";
	sol::state lua;

	LuaRunner();
	void setGetKey(drishengine::Window* window);
	void updateNodesScriptEnvironment(Node* node);
};  