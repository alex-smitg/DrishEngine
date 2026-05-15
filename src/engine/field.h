#pragma once

#include <string>


enum FieldType {
	Float,
	FloatVec3,
	String,
	Color3,
	Boolean,
	TextureID,
	ScriptHandle,
	TextureHandle,
	MaterialHandle,
	VerticesHandle,
};

class Field {
public:
	std::string name = "";
	FieldType type;
	void* ptr;

	Field(std::string name, FieldType type, void* ptr) {
		this->name = name;
		this->type = type;
		this->ptr = ptr;
	}
};