#pragma once

#include <unordered_map>
#include <typeindex>
#include <functional>

#include "base_object.h"

#include <string>

#include "imgui_docking/imgui.h"

class PropertyDrawer {
public:
	template<typename T>
	void registerDrawer(std::function<void(T&)> func) {
		drawers[typeid(T)] = [func](void* obj) {
			func(*static_cast<T*>(obj));
		};
	}

	void draw_properties(const std::type_info *type_inf, BaseObject* obj) {
		auto it = drawers.find(*type_inf);

		if (it != drawers.end()) {
			it->second(obj);
		}
		else {
			ImGui::Text("empty");
		};
	}

	void draw_properties(BaseObject *obj) {
		auto it = drawers.find(typeid(*obj));

		if (it != drawers.end()) {
			it->second(obj);
		}
		else {
			ImGui::Text("empty");
		};
	}


private:
	std::unordered_map<std::type_index, std::function<void(BaseObject*)>> drawers;
};

