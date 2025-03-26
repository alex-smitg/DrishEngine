#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "model_loader.h"
#include <string>
#include "shader.h"
#include <string>
#include "cube.h"

#include <filesystem>
#include "light_model.h"


#include <Windows.h>

#include "base_object.h"
#include "texture.h"
#include "material.h"
#include "transform.h"
#include "rigidbody.h"
#include "vertex_group.h"
#include "mesh.h"
#include "directional_light.h"
#include "point_light.h"
#include "line.h"
#include "mesh_holder.h"
#include "bezier.h"


int load_image(std::string path, GLuint *texture_id) { 
	int width, height, nrChannels;


	unsigned char* img = stbi_load(path.c_str(), &width, &height, &nrChannels, 4); //diff

	
	GLuint white = 0xFFFFFFFF;
	GLuint black = 0xFF000000;
	GLfloat pixels[12] = { 1.0f, 1.0f, 1.0f, 0.0f,  0.0f,  0.0f, 0.0f,  0.0f,  0.0f, 1.0f, 1.0f, 1.0f, };

	if (!img) {
		std::cout
			<< "ERROR: can't load image at path: " << path << " "
			<< stbi_failure_reason()
			<< "\n";

		stbi_image_free(img);

		return 0;
	}

	GLenum format = GL_RGB;
	//if (nrChannels == 1)
		//format = GL_RED;
	if (nrChannels == 3)
		format = GL_RGB;
	else if (nrChannels == 4)
		format = GL_RGBA;



	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	if (img) {
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
	}
	else {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_FLOAT, pixels);
	}
	
	glGenerateMipmap(GL_TEXTURE_2D);

	if (img) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	stbi_image_free(img);

	*texture_id = texture;

	return 1;
}


