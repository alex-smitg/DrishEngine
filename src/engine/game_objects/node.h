#pragma once


#include <string>
#include <vector>

#include "sol/sol.hpp"

#include "../logger.h"
#include "../game_object_types.h"
#include "../assets/script.h"
#include "../transform.h"
#include "../has_fields.h"

class AssetRepository;
class Shader;


class Node : public HasFields {
public:
	Type type = Type::BASE;

	Transform transform = Transform();
	std::string name = "";
	long long id = -1;

	std::vector<Node*> children;
	Node* parent = nullptr;

	Script* script = nullptr;
	
	sol::environment env;
	
	Node() {
		addField(Field("Name", FieldType::String, 1, &name));
		addField(Field("Position", FieldType::FloatVec3, 1, &transform.position));
		addField(Field("Rotation", FieldType::FloatVec3, 1, &transform.rotation));
		addField(Field("Scale", FieldType::FloatVec3, 1, &transform.scale));
		addField(Field("Script", FieldType::ScriptClass, 1, &script));
	}
	


	virtual void drawInEditor(Shader* shader) {
		for (Node* node : children) {
			node->drawInEditor(shader);
		}
	}

	virtual void update(double delta) {
		if (env.valid()) {
			if (env["update"].valid()) {
				env["update"](delta);
			}
		}

		for (Node* node : children) {
			node->update(delta);
		}
	};
	
	void appendChild(Node* node);

	void getAllChildNodes(Node* parent, std::vector<Node*>* vec);

	Node* findChild(std::string name);
	void destroy();

	std::vector<Node*> getChildren() {
		return children;
	}
};

inline void to_json(nlohmann::json& j, const Node& node) {
	j["name"] = node.name;
	j["transform"] = node.transform;
	j["type"] = node.type;
	j["parent"] = node.parent->id;
	j["id"] = node.id;
	
	if (node.script != nullptr) {
		j["script_assId"] = node.script->assId;
	}


	for (int i = 0; i < node.children.size(); i++) {
		j["children"].push_back(node.children[i]->id);
	}
}