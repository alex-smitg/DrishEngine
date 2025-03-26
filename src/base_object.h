//__ReadAttributes__
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
	Transform transform; //__Attribute__ class Transform
	
	Transform global_transform;

	std::string name; //__Attribute__ textEdit name
	std::string script = ""; 

	glm::vec3 global_position = glm::vec3(0.0f, 0.0f, 0.0f);

	std::vector<Property> properties;
	std::vector<BaseObject*> children;

	Type type = Type::NO;

	BaseObject* parent;


	sol::state lua;

	BaseObject();
	virtual void draw();
	virtual void update();
	void add_child(BaseObject* object);
	void remove();
	virtual ~BaseObject();
};