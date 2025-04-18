#pragma once


#include <vector>
#include <string>

#define SOL_ALL_SAFETIES_ON 1
#include <sol\sol.hpp>

#include "transform.h"
#include "types.h"
#include "properties.h"

#include "imgui_docking/imgui.h"


class BaseObject {
public:
	Transform transform; 
	
	std::string name; 
	std::string script = 
		R"(function start()

end
function update()

end)";

	std::vector<Property> properties;
	std::vector<BaseObject*> children;

	Type type = Type::NO;


	BaseObject* parent;

	sol::state lua;

	BaseObject();

	std::string set_script(std::string script);

	virtual void draw();
	virtual void update();
	void add_child(BaseObject* object);
	void remove();
	virtual ~BaseObject();

	

};