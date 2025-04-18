#pragma once

#include "base_object.h"



BaseObject::BaseObject() {
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

	lua["transform"] = &transform;
};

void BaseObject::draw() {};

void BaseObject::update() {

	if (!script.empty()) {
		if (lua["start"].valid()) {
			lua["start"]();
		}

		if (lua["update"].valid()) {
			lua["update"]();
		}
	}
};

void BaseObject::add_child(BaseObject* object) {
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
};


std::string BaseObject::set_script(std::string script) {
	try {
		this->script = script;
		lua.script(script);
		return "";
	}
	catch (const std::exception& ex) {
		return ex.what();
	}
}

void BaseObject::remove() {
	parent->children.erase(std::remove(parent->children.begin(), parent->children.end(), this), parent->children.end());
};


BaseObject::~BaseObject() {
	std::cout << "delete";

	for (BaseObject* child : children) {
		delete child;
	}
};