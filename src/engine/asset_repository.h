#pragma once

#include <vector>
#include <string>
#include <map>

#include "assets/asset_handle.h"

#include "assets/script.h"
#include "assets/material.h"
#include "assets/texture.h"
#include "assets/vertices.h"
#include "assets/sound.h"


#include "loaders/image_loader.h"
#include "loaders/model_loader.h"
#include "logger.h"

template <typename T>
class AssetSlot {
public:
	T* asset = nullptr;
	int index = -1;
	bool is_valid = false;
};


template <typename T>
class AssetsContainer {
public:
	std::vector<AssetSlot<T>*> slots;

	void remove(int index) {
		slots[index]->asset = nullptr;
		slots[index]->is_valid = false;
	}

	int size() {
		return slots.size();
	}

	void add(T* asset){
		AssetSlot<T>* freeCon = nullptr;

		for (AssetSlot<T>* assCon : slots) {
			if (assCon->is_valid == false) {
				freeCon = assCon;

				break;
			}
		}
		if (!freeCon) {
			freeCon = new AssetSlot<T>();
			freeCon->index = slots.size();
			slots.push_back(freeCon);
		}

		freeCon->asset = asset;
		freeCon->is_valid = true;
	}

	std::optional<T*> get(AssetHandle* assHolder) {
		if (assHolder->index == INVALID_INDEX) {
			return {};
		}
		AssetSlot<T>* container = slots[assHolder->index];


		if (container->is_valid) {
			return container->asset;
		}
		else {
			return {};
		}
	}
};



class AssetRepository
{
public:
	AssetsContainer<Script> scripts;
	AssetsContainer<Material> materials;
	AssetsContainer<Texture> textures;
	AssetsContainer<Vertices> vertices;
	AssetsContainer<Sound> sounds;

	Shader defaultShader;
};
