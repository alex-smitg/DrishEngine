#include "lua_runner.h"

#include <GLFW/glfw3.h>

#include "window.h"
#include "transform.h"
#include "node_creator.h"

LuaRunner::LuaRunner() {
	lua.open_libraries(sol::lib::base, sol::lib::math);

	lua.new_usertype<glm::vec3>("Vector3",
		sol::constructors<glm::vec3(), glm::vec3(float, float, float)>(),
		"x", &glm::vec3::x,
		"y", &glm::vec3::y,
		"z", &glm::vec3::z
		);

	lua.new_usertype<Transform>("Transform",
		"position", &Transform::position,
		"rotation", &Transform::rotation,
		"scale", &Transform::scale
		);

	lua.new_enum("NODE",
		"BASE", Type::BASE,
		"MODEL", Type::MODEL);

	lua.new_usertype<Node>("Node",
		sol::no_constructor,
		"transform", &Node::transform,
		"name", &Node::name,
		"appendChild", &Node::appendChild,
		"findChild", &Node::findChild,
		"destroy", &Node::destroy,
		"getChildren", &Node::getChildren);

	

	/*lua.new_usertype<Model>("Model",
		sol::base_classes, sol::bases<Node>(),
		"vertices", &Model::vertices,
		"material", &Model::material);*/

	


	lua.set_function("print", [this](sol::variadic_args va) {
		std::string st;
		for (auto v : va) {
			st += lua["tostring"](v);
			st += " ";
		}
		logInfo(st);
	});
}


void LuaRunner::updateNodesScriptEnvironment(Node* node, AssetRepository* assetRepository) {
	this->lastError = "";

	for (Node* n : node->children) {
		updateNodesScriptEnvironment(n, assetRepository);
	}

	std::optional<Script*> scr = assetRepository->scripts.get(&node->scriptHandle);

	if (scr.has_value()) {
		Script* script = scr.value();

		node->env = sol::environment(lua, sol::create, lua.globals());

		node->env.set_function("destroy", [node]() {
			node->destroy();
			});

		node->env.set_function("appendChild", [node](Node* n) {
			node->appendChild(n);
			});

		node->env["name"] = &node->name;
		node->env["position"] = &node->transform.position;
		node->env["rotation"] = &node->transform.rotation;
		node->env["scale"] = &node->transform.scale;
		lua.safe_script(script->source, node->env,
			[this](lua_State*, sol::protected_function_result pfr) {
				// pfr will contain things that went wrong, for either loading or executing the script
				// the user can do whatever they like here, including throw. Otherwise...
				sol::error err = pfr;
				this->lastError = err.what();
				logError(err.what());
				// ... they need to return the protected_function_result
				return pfr;
			});
	}
}

void LuaRunner::addAssetRepositoryFunctions(AssetRepository* assetRepository) {
	lua["AssetRepository"] = lua.create_table();

	/*lua.set_function("getVertices", [assetRepository](long long id) {
		return assetRepository->getVertices(id);
		}
	);
	lua.set_function("getTexture", [assetRepository](long long id) {
		return assetRepository->getTexture(id);
		}
	);
	lua.set_function("getMaterial", [assetRepository](long long id) {
		return assetRepository->getMaterial(id);
		}
	);*/

}

void LuaRunner::addCreateNodesFunctions(NodeRepository* nodeRepository) {
	lua.set_function("createNode", [nodeRepository]() {
			return NodeCreator::createNode(Type::BASE, "Empty", nodeRepository);
		}
	);
	lua.set_function("createModel", [nodeRepository]() {
		return static_cast<Model*>(NodeCreator::createNode(Type::MODEL, "Model", nodeRepository));
	}
	);
}

void LuaRunner::setGetKey(drishengine::Window* window) {
	lua.set_function("isKeyPressed", [window](int key) {
		return (bool) glfwGetKey(window->getWindow(), key);
	});

	lua["KEY"] = lua.create_table();

	lua["KEY"]["SPACE"] = GLFW_KEY_SPACE;
	lua["KEY"]["APOSTROPHE"] = GLFW_KEY_APOSTROPHE;
	lua["KEY"]["COMMA"] = GLFW_KEY_COMMA;
	lua["KEY"]["MINUS"] = GLFW_KEY_MINUS;
	lua["KEY"]["PERIOD"] = GLFW_KEY_PERIOD;
	lua["KEY"]["SLASH"] = GLFW_KEY_SLASH;
	lua["KEY"]["0"] = GLFW_KEY_0;
	lua["KEY"]["1"] = GLFW_KEY_1;
	lua["KEY"]["2"] = GLFW_KEY_2;
	lua["KEY"]["3"] = GLFW_KEY_3;
	lua["KEY"]["4"] = GLFW_KEY_4;
	lua["KEY"]["5"] = GLFW_KEY_5;
	lua["KEY"]["6"] = GLFW_KEY_6;
	lua["KEY"]["7"] = GLFW_KEY_7;
	lua["KEY"]["8"] = GLFW_KEY_8;
	lua["KEY"]["9"] = GLFW_KEY_9;
	lua["KEY"]["SEMICOLON"] = GLFW_KEY_SEMICOLON;
	lua["KEY"]["EQUAL"] = GLFW_KEY_EQUAL;
	lua["KEY"]["A"] = GLFW_KEY_A;
	lua["KEY"]["B"] = GLFW_KEY_B;
	lua["KEY"]["C"] = GLFW_KEY_C;
	lua["KEY"]["D"] = GLFW_KEY_D;
	lua["KEY"]["E"] = GLFW_KEY_E;
	lua["KEY"]["F"] = GLFW_KEY_F;
	lua["KEY"]["G"] = GLFW_KEY_G;
	lua["KEY"]["H"] = GLFW_KEY_H;
	lua["KEY"]["I"] = GLFW_KEY_I;
	lua["KEY"]["J"] = GLFW_KEY_J;
	lua["KEY"]["K"] = GLFW_KEY_K;
	lua["KEY"]["L"] = GLFW_KEY_L;
	lua["KEY"]["M"] = GLFW_KEY_M;
	lua["KEY"]["N"] = GLFW_KEY_N;
	lua["KEY"]["O"] = GLFW_KEY_O;
	lua["KEY"]["P"] = GLFW_KEY_P;
	lua["KEY"]["Q"] = GLFW_KEY_Q;
	lua["KEY"]["R"] = GLFW_KEY_R;
	lua["KEY"]["S"] = GLFW_KEY_S;
	lua["KEY"]["T"] = GLFW_KEY_T;
	lua["KEY"]["U"] = GLFW_KEY_U;
	lua["KEY"]["V"] = GLFW_KEY_V;
	lua["KEY"]["W"] = GLFW_KEY_W;
	lua["KEY"]["X"] = GLFW_KEY_X;
	lua["KEY"]["Y"] = GLFW_KEY_Y;
	lua["KEY"]["Z"] = GLFW_KEY_Z;
	lua["KEY"]["LEFT_BRACKET"] = GLFW_KEY_LEFT_BRACKET;
	lua["KEY"]["BACKSLASH"] = GLFW_KEY_BACKSLASH;
	lua["KEY"]["RIGHT_BRACKET"] = GLFW_KEY_RIGHT_BRACKET;
	lua["KEY"]["GRAVE_ACCENT"] = GLFW_KEY_GRAVE_ACCENT;
	lua["KEY"]["WORLD_1"] = GLFW_KEY_WORLD_1;
	lua["KEY"]["WORLD_2"] = GLFW_KEY_WORLD_2;
	lua["KEY"]["ESCAPE"] = GLFW_KEY_ESCAPE;
	lua["KEY"]["ENTER"] = GLFW_KEY_ENTER;
	lua["KEY"]["TAB"] = GLFW_KEY_TAB;
	lua["KEY"]["BACKSPACE"] = GLFW_KEY_BACKSPACE;
	lua["KEY"]["INSERT"] = GLFW_KEY_INSERT;
	lua["KEY"]["DELETE"] = GLFW_KEY_DELETE;
	lua["KEY"]["RIGHT"] = GLFW_KEY_RIGHT;
	lua["KEY"]["LEFT"] = GLFW_KEY_LEFT;
	lua["KEY"]["DOWN"] = GLFW_KEY_DOWN;
	lua["KEY"]["UP"] = GLFW_KEY_UP;
	lua["KEY"]["PAGE_UP"] = GLFW_KEY_PAGE_UP;
	lua["KEY"]["PAGE_DOWN"] = GLFW_KEY_PAGE_DOWN;
	lua["KEY"]["HOME"] = GLFW_KEY_HOME;
	lua["KEY"]["END"] = GLFW_KEY_END;
	lua["KEY"]["CAPS_LOCK"] = GLFW_KEY_CAPS_LOCK;
	lua["KEY"]["SCROLL_LOCK"] = GLFW_KEY_SCROLL_LOCK;
	lua["KEY"]["NUM_LOCK"] = GLFW_KEY_NUM_LOCK;
	lua["KEY"]["PRINT_SCREEN"] = GLFW_KEY_PRINT_SCREEN;
	lua["KEY"]["PAUSE"] = GLFW_KEY_PAUSE;
	lua["KEY"]["F1"] = GLFW_KEY_F1;
	lua["KEY"]["F2"] = GLFW_KEY_F2;
	lua["KEY"]["F3"] = GLFW_KEY_F3;
	lua["KEY"]["F4"] = GLFW_KEY_F4;
	lua["KEY"]["F5"] = GLFW_KEY_F5;
	lua["KEY"]["F6"] = GLFW_KEY_F6;
	lua["KEY"]["F7"] = GLFW_KEY_F7;
	lua["KEY"]["F8"] = GLFW_KEY_F8;
	lua["KEY"]["F9"] = GLFW_KEY_F9;
	lua["KEY"]["F10"] = GLFW_KEY_F10;
	lua["KEY"]["F11"] = GLFW_KEY_F11;
	lua["KEY"]["F12"] = GLFW_KEY_F12;
	lua["KEY"]["F13"] = GLFW_KEY_F13;
	lua["KEY"]["F14"] = GLFW_KEY_F14;
	lua["KEY"]["F15"] = GLFW_KEY_F15;
	lua["KEY"]["F16"] = GLFW_KEY_F16;
	lua["KEY"]["F17"] = GLFW_KEY_F17;
	lua["KEY"]["F18"] = GLFW_KEY_F18;
	lua["KEY"]["F19"] = GLFW_KEY_F19;
	lua["KEY"]["F20"] = GLFW_KEY_F20;
	lua["KEY"]["F21"] = GLFW_KEY_F21;
	lua["KEY"]["F22"] = GLFW_KEY_F22;
	lua["KEY"]["F23"] = GLFW_KEY_F23;
	lua["KEY"]["F24"] = GLFW_KEY_F24;
	lua["KEY"]["F25"] = GLFW_KEY_F25;
	lua["KEY"]["KP_0"] = GLFW_KEY_KP_0;
	lua["KEY"]["KP_1"] = GLFW_KEY_KP_1;
	lua["KEY"]["KP_2"] = GLFW_KEY_KP_2;
	lua["KEY"]["KP_3"] = GLFW_KEY_KP_3;
	lua["KEY"]["KP_4"] = GLFW_KEY_KP_4;
	lua["KEY"]["KP_5"] = GLFW_KEY_KP_5;
	lua["KEY"]["KP_6"] = GLFW_KEY_KP_6;
	lua["KEY"]["KP_7"] = GLFW_KEY_KP_7;
	lua["KEY"]["KP_8"] = GLFW_KEY_KP_8;
	lua["KEY"]["KP_9"] = GLFW_KEY_KP_9;
	lua["KEY"]["KP_DECIMAL"] = GLFW_KEY_KP_DECIMAL;
	lua["KEY"]["KP_DIVIDE"] = GLFW_KEY_KP_DIVIDE;
	lua["KEY"]["KP_MULTIPLY"] = GLFW_KEY_KP_MULTIPLY;
	lua["KEY"]["KP_SUBTRACT"] = GLFW_KEY_KP_SUBTRACT;
	lua["KEY"]["KP_ADD"] = GLFW_KEY_KP_ADD;
	lua["KEY"]["KP_ENTER"] = GLFW_KEY_KP_ENTER;
	lua["KEY"]["KP_EQUAL"] = GLFW_KEY_KP_EQUAL;
	lua["KEY"]["LEFT_SHIFT"] = GLFW_KEY_LEFT_SHIFT;
	lua["KEY"]["LEFT_CONTROL"] = GLFW_KEY_LEFT_CONTROL;
	lua["KEY"]["LEFT_ALT"] = GLFW_KEY_LEFT_ALT;
	lua["KEY"]["LEFT_SUPER"] = GLFW_KEY_LEFT_SUPER;
	lua["KEY"]["RIGHT_SHIFT"] = GLFW_KEY_RIGHT_SHIFT;
	lua["KEY"]["RIGHT_CONTROL"] = GLFW_KEY_RIGHT_CONTROL;
	lua["KEY"]["RIGHT_ALT"] = GLFW_KEY_RIGHT_ALT;
	lua["KEY"]["RIGHT_SUPER"] = GLFW_KEY_RIGHT_SUPER;
	lua["KEY"]["MENU"] = GLFW_KEY_MENU;
}