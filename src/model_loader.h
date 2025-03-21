#pragma once

#include<vector>
#include <GL/glew.h>
#include<string>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "mat.h"

int open_obj(std::string path, std::vector<std::vector<GLfloat>>* vertices, std::vector<std::string>* mat_n);
int open_mtl(std::string path, std::map<std::string, Mat>* materials);

