#pragma once

#include <vector>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "base_object.h"
#include "shader.h"
#include "line.h"

class Bezier : public BaseObject {
public:
	Shader* shader;

	std::vector<Line*> lines;

	glm::vec3 p1 = glm::vec3(1.0, 0.0, 1.0);
	glm::vec3 p2 = glm::vec3(-1.0, 0.0, -1.0);
	glm::vec3 p3 = glm::vec3(1.0, 0.0, 0.0);

	Line* up_line1;
	Line* up_line2;
	Line* up_line3;

	float step = 0.1f;

	Bezier(Shader* shader) {
		float i = 0.0f;

		this->type = Type::NO;

		name = "Curve";

		this->shader = shader;


		up_line1 = new Line(shader, p1, glm::vec3(p1.x, p1.y + 0.2f, p1.z), glm::vec3(0.0f, 1.0f, 0.0f));
		up_line2 = new Line(shader, p2, glm::vec3(p2.x, p2.y + 0.2f, p2.z), glm::vec3(0.0f, 1.0f, 0.0f));
		up_line3 = new Line(shader, p3, glm::vec3(p3.x, p3.y + 0.2f, p3.z), glm::vec3(0.0f, 1.0f, 0.0f));



		for (i; i < 1.0f; i += step) {
			glm::vec3 point = GetPoint(i);
			glm::vec3 point2 = GetPoint(i + step);

			Line* line = new Line(shader, point, point2, glm::vec3(1.0, 0.0, 0.0));
			lines.push_back(line);

		}

	}

	void draw() override {
		float i = 0.0;
		for (Line* line : lines) {
			glm::vec3 point = GetPoint(i);
			glm::vec3 point2 = GetPoint(i + step);

			line->verts[0] = point.x;
			line->verts[1] = point.y;
			line->verts[2] = point.z;
			line->verts[3] = point2.x;
			line->verts[4] = point2.y;
			line->verts[5] = point2.z;
			line->update_vertex();



			i += step;
			line->draw();
		}

		up_line1->verts[0] = p1.x;
		up_line1->verts[1] = p1.y;
		up_line1->verts[2] = p1.z;
		up_line1->verts[3] = p1.x;
		up_line1->verts[4] = p1.y + 0.2;
		up_line1->verts[5] = p1.z;
		up_line1->update_vertex();
		up_line1->draw();

		up_line2->verts[0] = p2.x;
		up_line2->verts[1] = p2.y;
		up_line2->verts[2] = p2.z;
		up_line2->verts[3] = p2.x;
		up_line2->verts[4] = p2.y + 0.2;
		up_line2->verts[5] = p2.z;
		up_line2->update_vertex();
		up_line2->draw();

		up_line3->verts[0] = p3.x;
		up_line3->verts[1] = p3.y;
		up_line3->verts[2] = p3.z;
		up_line3->verts[3] = p3.x;
		up_line3->verts[4] = p3.y + 0.2;
		up_line3->verts[5] = p3.z;
		up_line3->update_vertex();
		up_line3->draw();


	}

	glm::vec3 GetPoint(float i) {
		//glm::vec2 a = p2 + (1.0f - i) * (p1 - p2);
		//glm::vec2 b = p2 + i * (p3 - p2);

		//glm::vec2 f = a + i * (b - a);


		glm::vec3 f = glm::vec3(i * i * (p3 - p1) - i * (p2 - p1) + p2);

		return f;
	}
};