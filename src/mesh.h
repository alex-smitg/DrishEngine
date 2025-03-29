#pragma once


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "resource.h"
#include "transform.h"
#include "vertex_group.h"

#include "mat.h"

class Mesh : public Resource {
public:
	std::vector<std::vector<GLfloat>> models_data;
	std::vector<VertexGroup*> vertexGroups;

	Transform* transform;

	std::vector<std::string> mat_names;
	std::map<std::string, Mat> materials;

	Transform temp;


	int vertice_number = 0; //

	void add_vertex_group(VertexGroup* vertexGroup) {
		vertexGroups.push_back(vertexGroup);

		vertice_number += vertexGroup->get_vertices_number();
	}

	void draw_properties() {

	}

	Mesh() {

	}


	void draw() {

		for (VertexGroup* vertexGroup : vertexGroups) {
			vertexGroup->temp = temp;
			vertexGroup->transform = transform;
			vertexGroup->draw();
		}
	}

};