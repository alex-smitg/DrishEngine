#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Bullet/LinearMath/btIDebugDraw.h"

class BulletDebugDraw : public btIDebugDraw
{
public:

	GLuint VBO, VAO;

	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {
		GLfloat points[12];

		points[0] = from.x();
		points[1] = from.y();
		points[2] = from.z();
		points[3] = to.x();
		points[4] = to.y();
		points[5] = to.z();



		glDeleteBuffers(1, &VBO);
		glDeleteVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);



		glBindVertexArray(VAO);
		glDrawArrays(GL_LINES, 0, 2);

	}
	virtual void drawTriangle(const btVector3& v0, const btVector3& v1, const btVector3& v2, const btVector3& color, btScalar) {
		drawLine(v0, v1, color);
		drawLine(v1, v2, color);
		drawLine(v2, v0, color);

	}


	virtual void	drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) {

	}

	virtual void	reportErrorWarning(const char* warningString) {

	}

	virtual void	draw3dText(const btVector3& location, const char* textString) {

	}
	virtual void	setDebugMode(int debugMode) {

	}


	virtual int		getDebugMode() const {
		return 1;
	}

};