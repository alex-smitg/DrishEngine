#pragma once

#include <vector>
#include <string>
#include <map>

#include "field.h"


class HasFields {
public:
	void addField(const Field& f) {
		fields.push_back(f);
	}

	const std::vector<Field>& getFields() {
		return fields;
	}

	const Field& getField(const std::string& name) {
		auto it = this->index.find(name);
		if (it != this->index.end()) {
			return this->fields[it->second];
		}
		else {
			static void* nothing = 0;
			static Field field = Field("INVALID", FieldType::Int, nothing, false, true);
			return field;
		}
	}

private:
	std::map<std::string, int> index;
	std::vector<Field> fields;
};