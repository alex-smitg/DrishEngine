#pragma once

#include <vector>
#include <string>
#include <map>

#include "assets/script.h"
#include "assets/material.h"
#include "assets/texture.h"
#include "assets/vertices.h"
#include "assets/sound.h"


#include "loaders/image_loader.h"
#include "loaders/model_loader.h"
#include "logger.h"


class AssetRepository
{
public:
	std::map<long long, Script *> scriptsMap;
	std::map<long long, Material *> materialsMap;
	std::map<long long, Texture *> texturesMap;
	std::map<long long, Vertices *> verticesMap;
	std::map<long long, Sound *> soundsMap;

	Shader defaultShader;


	AssetRepository() {}

	void addMaterial(Material *material)
	{
		materialsMap[material->assId] = material;
	}

	void addScript(Script *script)
	{
		scriptsMap[script->assId] = script;
	}

	void addVertices(Vertices *vertices)
	{
		verticesMap[vertices->assId] = vertices;
	}

	void addTexture(Texture *texture)
	{
		texturesMap[texture->assId] = texture;
	}

	void addSound(Sound *sound)
	{
		soundsMap[sound->assId] = sound;
	}

	Script *getScript(long long id)
	{
		if (scriptsMap.contains(id))
		{
			return scriptsMap[id];
		}
		else
		{
			return nullptr;
		}
	}

	Vertices *getVertices(long long id)
	{
		if (verticesMap.contains(id))
		{

			return verticesMap[id];
		}
		else
		{
			return nullptr;
		}
	}

	Texture *getTexture(long long id)
	{
		if (texturesMap.contains(id))
		{
			return texturesMap[id];
		}
		else
		{
			return nullptr;
		}
	}

	Material *getMaterial(long long id)
	{
		if (materialsMap.contains(id))
		{
			return materialsMap[id];
		}
		else
		{
			return nullptr;
		}
	}

	Sound *getSound(long long id)
	{
		if (soundsMap.contains(id))
		{
			return soundsMap[id];
		}
		else
		{
			return nullptr;
		}
	}
};
