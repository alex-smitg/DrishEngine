#pragma once

#include <string>


enum FieldType {
	Int,
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
	SoundHandle
};

class Field {
public:
	std::string name = "";
	FieldType type;
	void* ptr;
	bool serializable = true;
	bool showInInspector = true;

	Field(std::string name, FieldType type, void* ptr, bool serializable = true,
		bool showInInspector = true) {
		this->name = name;
		this->type = type;
		this->ptr = ptr;
		this->serializable = serializable;
		this->showInInspector = showInInspector;
	}
};