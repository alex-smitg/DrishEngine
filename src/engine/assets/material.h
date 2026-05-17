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
	AssetHandle textureHandle;

	float shine = 32.0;
	bool useLight = true;

	Material()
	{
		type = AssetType::MATERIAL;
		addField(Field("Color", FieldType::Color3, &color));
		addField(Field("Texture", FieldType::TextureHandle, &textureHandle));
		addField(Field("Use Light", FieldType::Boolean, &useLight));
		addField(Field("Shine", FieldType::Float, &shine));
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
	//if (material.texture != nullptr)
	//{
	//	//j["textureAssId"] = material.texture->assId;
	//}
}
