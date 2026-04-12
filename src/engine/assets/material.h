#pragma once

#include <string>
#include <vector>

#include <glm/gtc/type_ptr.hpp>

#include "../shader.h"
#include "asset.h"
#include "texture.h"
#include "../field.h"

class Material : public Asset
{
public:
	Shader *shader = nullptr;

	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	Texture *texture = nullptr;

	float shine = 32.0;
	bool useLight = true;

	bool isDeleted = false;

	Material(long long id) : Asset(id)
	{
		addField(Field("Color", FieldType::Color3, 1, &color));
		addField(Field("Texture", FieldType::TextureClass, 1, &texture));
		addField(Field("Use Light", FieldType::Boolean, 1, &useLight));
		addField(Field("Shine", FieldType::Float, 1, &shine));
	}
};


inline void to_json(nlohmann::json &j, const Material &material)
{
	nlohmann::to_json(j, static_cast<Asset>(material));
	j["color"]["r"] = material.color.r;
	j["color"]["g"] = material.color.g;
	j["color"]["b"] = material.color.b;
	j["useLight"] = material.useLight;
	j["shine"] = material.shine;
	if (material.texture != nullptr)
	{
		j["textureAssId"] = material.texture->assId;
	}
}
