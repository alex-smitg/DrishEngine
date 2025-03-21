#pragma once

#include <vector>
#include <string>

#define SOL_ALL_SAFETIES_ON 1
#include <sol\sol.hpp>

#include "transform.h"
#include "types.h"
#include "properties.h"



class BaseObject {
public:
	Transform transform;
	Transform global_transform;

	std::string name;
	std::string script = "";

	glm::vec3 global_position = glm::vec3(0.0f, 0.0f, 0.0f);

	std::vector<Property> properties;
	std::vector<BaseObject*> children;

	Type type = Type::NO;

	BaseObject* parent;


	sol::state lua;

	BaseObject() {
		lua.open_libraries(sol::lib::base, sol::lib::math);

	}

	virtual void draw() {

	}

	virtual void update() {

		if (!script.empty()) {
			/*lua["pos_x"] = model[3].x;
			lua["pos_y"] = model[3].y;
			lua["pos_z"] = model[3].z;

			lua["rot_x"] = rotation.x;
			lua["rot_y"] = rotation.y;
			lua["rot_z"] = rotation.z;*/


			try {
				lua.script(script);
			}
			catch (const std::exception& ex) {
				script = "";
			}

			//model[3].x = lua["pos_x"];
			//model[3].y = lua["pos_y"];
			//model[3].z = lua["pos_z"];


			//rotation.x = lua["rot_x"];
			//rotation.y = lua["rot_y"];
			//rotation.z = lua["rot_z"];
		}




		/*for (BaseObject* baseObject: children) {
			baseObject->transform.position = start_position;
			baseObject->global_transform = baseObject->transform + global_transform;
		}*/




	}

	void add_child(BaseObject* object) {
		for (BaseObject* child : children) {
			if (object->name == child->name) {


				int number = 1;



				std::string n = object->name;

				std::string number_string = "";
				std::string name_def = "";

				int stop_pos = n.size();

				for (int i = object->name.length() - 1; i > 0; i--) {
					if (n[i] != '0' && n[i] != '1' && n[i] != '2' && n[i] != '3' && n[i] != '4' && n[i] != '5' && n[i] != '6' && n[i] != '7' && n[i] != '8' && n[i] != '9') {
						stop_pos = i + 1;

						break;
					}
				}

				for (int i = 0; i < stop_pos; i++) {
					name_def += n[i];
				}

				for (int i = stop_pos; i < n.size(); i++) {
					number_string.push_back(n[i]);
				}

				if (number_string != "") {
					number = std::stoi(number_string) + 1;
				}
				else {
					number = 1;
				}


				object->name = name_def + std::to_string(number);


			}
		}


		object->parent = this;
		children.push_back(object);
	}

	void remove() {
		parent->children.erase(std::remove(parent->children.begin(), parent->children.end(), this), parent->children.end());
	}


	virtual ~BaseObject() {
		std::cout << "delete";

		for (BaseObject* child : children) {
			delete child;
		}
	}

};