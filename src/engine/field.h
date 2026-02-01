#pragma once

#include <string>


enum FieldType {
	Float,
	FloatVec3,
	String,
	Color3,
	Boolean,
	TextureID,
	ScriptClass,
	TextureClass,
	MaterialClass, 
	VerticesClass,};

class Field {
public:
	std::string name = "";
	FieldType type;
	int count = 1;
	void* ptr;

	Field(std::string name, FieldType type, int count, void* ptr) {
		this->name = name;
		this->type = type;
		this->count = count;
		this->ptr = ptr;
	}
};