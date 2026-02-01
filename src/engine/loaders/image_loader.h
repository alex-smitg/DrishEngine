#pragma once

#include <filesystem>



#include "stb_image.h"


#include "assets/texture.h"

#include "logger.h"

class ImageLoader { 
public:
	ImageLoader() {
		stbi_set_flip_vertically_on_load(true);
	}

	static void loadImage(std::filesystem::path path, Texture* texture) {
		logDebug("[IMAGE LOADER] load image: ", path.generic_string().c_str());
		if (!std::filesystem::exists(path)) {
			logError("[IMAGE LOADER] Path does not exists");
		}
		unsigned char* data = stbi_load(path.string().c_str(), &texture->width, &texture->height, &texture->channels, 0);


		glGenTextures(1, &texture->glid);
		glBindTexture(GL_TEXTURE_2D, texture->glid);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture->glid);
	}

	static void loadImage(unsigned char* imgData, int size, Texture* texture) {
		unsigned char* data = stbi_load_from_memory(imgData, size,
			&texture->width,
			&texture->height,
			&texture->channels, 0);

		glGenTextures(1, &texture->glid);
		glBindTexture(GL_TEXTURE_2D, texture->glid);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture->glid);
	}
};