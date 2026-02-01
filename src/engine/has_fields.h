#pragma once

#include <vector>

#include "field.h"

class HasFields {
public:
	void addField(const Field& f) {
		fields.push_back(f);
	}

	const std::vector<Field>& getFields() {
		return fields;
	}

private:
	std::vector<Field> fields;
};