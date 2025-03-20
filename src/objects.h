#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "model_loader.h"
#include <string>
#include "shader.h"
#include <string>
#include "cube.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>
#include "light_model.h"

#define SOL_ALL_SAFETIES_ON 1
#include <sol\sol.hpp>
#include <Windows.h>

int load_image(std::string path, GLuint *texture_id);


//types : 0 == mesh

enum Property {MESH, LIGHT, CURVE, TRANSFORM};
enum Type {NO, MESH_HOLDER, POINT_LIGHT, DIRECTIONAL_LIGHT
};




std::string increment_string(std::string str) {

	return "1";




}

class Resource {
public:
	std::string name;
};


class Texture : public Resource {
public:
	GLuint id;
	std::string path;


	//~Texture() {
		//glDeleteTextures(1, &id);
	//}
};

class Material : public Resource {
public:
	float normalmap_value = 1.0f;
	float specular_value = 1.0f;
	float reflection_value = 0.0f;
	float diffuse_value = 1.0f;
	float refract_value = 0.256f;
	float shine_value = 18.0f;

	glm::vec2 uv_scale = { 1.0f, 1.0f };

	bool use_normalmap = false;
	bool use_specular = false;
	bool use_diffuse = false;
	bool use_blinn = false;
	bool emit = false;

	glm::vec3 diffuse_color = glm::vec3(1.0f, 1.0f, 1.0f);


	Texture* diffuse_texture;
	Texture* specular_texture;
	Texture* normalmap_texture;

};





class Transform {
public:
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);


	glm::mat4 get_matrix() {
		glm::mat4 matrix(1.0f);
		matrix = glm::translate(matrix, position);

		matrix = glm::rotate(matrix, rotation.x, glm::vec3(1, 0, 0));
		matrix = glm::rotate(matrix, rotation.y, glm::vec3(0, 1, 0));
		matrix = glm::rotate(matrix, rotation.z, glm::vec3(0, 0, 1));

		matrix = glm::scale(matrix, scale);

		return matrix;
	}
};





class BaseObject {
public:
	Transform transform;
	Transform global_transform;

	std::string name;
	std::string script = "";

	glm::vec3 global_position = glm::vec3(0.0f, 0.0f, 0.0f);

	std::vector<Property> properties;
	std::vector<BaseObject*> children;

	Type type = Type::NO;

	BaseObject* parent;
	

	sol::state lua;

	BaseObject() {
		lua.open_libraries(sol::lib::base, sol::lib::math);

	}

	virtual void draw() {

	}

	virtual void update() {
		
		if (!script.empty()) {
			/*lua["pos_x"] = model[3].x;
			lua["pos_y"] = model[3].y;
			lua["pos_z"] = model[3].z;

			lua["rot_x"] = rotation.x;
			lua["rot_y"] = rotation.y;
			lua["rot_z"] = rotation.z;*/


			try {
				lua.script(script);
			}
			catch (const std::exception& ex) {
				std::cout << ex.what();
				script = "";
			}

			//model[3].x = lua["pos_x"];
			//model[3].y = lua["pos_y"];
			//model[3].z = lua["pos_z"];


			//rotation.x = lua["rot_x"];
			//rotation.y = lua["rot_y"];
			//rotation.z = lua["rot_z"];
		}




		/*for (BaseObject* baseObject: children) {
			baseObject->transform.position = start_position;
			baseObject->global_transform = baseObject->transform + global_transform;
		}*/
		

	
		
	}

	void add_child(BaseObject* object) {
		for (BaseObject* child : children) {
			if (object->name == child->name) {


				int number = 1;

				

				std::string n = object->name;

				std::string number_string = "";
				std::string name_def = "";

				int stop_pos = n.size();

				for (int i = object->name.length() - 1; i > 0; i--) {
					if (n[i] != '0' && n[i] != '1' && n[i] != '2' && n[i] != '3' && n[i] != '4' && n[i] != '5' && n[i] != '6' && n[i] != '7' && n[i] != '8' && n[i] != '9') {
						stop_pos = i + 1;
						
						break;
					}
				}

				for (int i = 0; i < stop_pos; i++) {
					name_def += n[i];
				}

				for (int i = stop_pos; i < n.size(); i++) {
						number_string.push_back(n[i]);
				}

				if (number_string != "") {
					number = std::stoi(number_string) + 1;
				}
				else {
					number = 1;
				}
				

				object->name = name_def + std::to_string(number);


			}
		}

		
		object->parent = this;
		children.push_back(object);
	}

	void remove() {
		parent->children.erase(std::remove(parent->children.begin(), parent->children.end(), this), parent->children.end());
	}


	virtual ~BaseObject() {
		std::cout << "delete";
		
		for (BaseObject* child : children) {
			delete child;
		}
	}

};



class RigidBody : public BaseObject {
public:
	btCollisionShape* collisionShape;
	btRigidBody* rigidBody;

	RigidBody(btDiscreteDynamicsWorld* dynamicsWorld, btCollisionShape* collisionShape, Transform transform, int mass) {
		properties.push_back(Property::TRANSFORM);

		this->transform = transform;

		this->collisionShape = collisionShape;
		btDefaultMotionState* motionstate = new btDefaultMotionState(btTransform(
			btQuaternion(0.0, 0.0, 0.0, 1.0),
			btVector3(transform.position.x, transform.position.y, transform.position.z)
		));


		btRigidBody::btRigidBodyConstructionInfo rigidbodyci(
			mass,                  // mass, in kg. 0 -> static object, will never move.
			motionstate,
			collisionShape,  // collision shape of body
			btVector3(0, 0, 0)    // local inertia
		);
		rigidBody = new btRigidBody(rigidbodyci);


		dynamicsWorld->addRigidBody(rigidBody);
	}

	void update() { 
		BaseObject::update();

		

		btTransform trans;
		rigidBody->getMotionState()->getWorldTransform(trans);

		//rigidBody->applyCentralForce(btVector3(0.01, 0.0, 0.0));
		/*for (BaseObject *ch : children) {
			ch->transform = transform;
		}*/

		this->transform.position.x = trans.getOrigin().getX();
		this->transform.position.y = trans.getOrigin().getY();
		this->transform.position.z = trans.getOrigin().getZ();

		//obj2->model[3][0] = trans.getOrigin().getX();
		//obj2->model[3][1] = trans.getOrigin().getY();
		//obj2->model[3][2] = trans.getOrigin().getZ();

		transform.rotation.x = trans.getRotation().getX();
		transform.rotation.y = trans.getRotation().getY();
		transform.rotation.z = trans.getRotation().getZ();

	}
};


class PhysicsColission {
	btCollisionShape* collision_shape;




};



class Camera {
public:
	glm::mat4 view;
	glm::vec3 position;

	

	Camera() {
		view = glm::mat4(1.0);
		position = glm::vec3(0.0);
	}
};



class DirectionalLight : public BaseObject {
public:
	glm::vec3 color;
	GLfloat strength;
	Shader* shader;
	unsigned int VBO, VAO;

	bool visible = true;

	glm::vec3 direction = glm::vec3(0.0, 0.0, 0.0);

	DirectionalLight(Shader* shader, std::string name, glm::vec3 color = glm::vec3(1.0, 1.0, 1.0), GLfloat strength = 1.0) : BaseObject() {
		this->strength = strength;
		this->transform.scale = glm::vec3(0.2f, 0.2f, 0.2f);
		this->color = color;
		this->shader = shader;
		this->name = name;

		//sol::usertype<PointLight> player_type = lua.new_usertype<PointLight>(name,
		//sol::constructors<PointLight(Shader*, std::string, glm::vec3, GLfloat)>());
		properties.push_back(Property::TRANSFORM);
		//properties.push_back(Property::LIGHT);

		type = DIRECTIONAL_LIGHT;
		init();
	};


	void draw() override {
		shader->Use();
		shader->setVec3("color", color);

		Transform temp;
		temp.position.x = parent->transform.position.x + transform.position.x;
		temp.position.y = parent->transform.position.y + transform.position.y;
		temp.position.z = parent->transform.position.z + transform.position.z;

		temp.scale = transform.scale;
		temp.rotation = transform.rotation;

		shader->setMat4("model", temp.get_matrix());
		shader->setFloat("strenght", strength);

		if (visible) {
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, low_poly_sphere.size() / 3);
		}


	}

	void update() override {

		if (!script.empty()) {
			/*lua["pos_x"] = model[3].x;
			lua["pos_y"] = model[3].y;
			lua["pos_z"] = model[3].z;*/
			lua["r"] = color.r;
			lua["g"] = color.g;
			lua["b"] = color.b;

			try {
				lua.script(script);
			}
			catch (const std::exception& ex) {
				std::cout << ex.what();
				script = "";
			}

			/*model[3].x = lua["pos_x"];
			model[3].y = lua["pos_y"];
			model[3].z = lua["pos_z"];*/
			color.r = lua["r"];
			color.g = lua["g"];
			color.b = lua["b"];

			color.r = std::clamp(color.r, 0.0f, 1.0f);
			color.g = std::clamp(color.g, 0.0f, 1.0f);
			color.b = std::clamp(color.b, 0.0f, 1.0f);
		}
	}

private:
	void init() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);


		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, low_poly_sphere.size() * sizeof(GLfloat), &low_poly_sphere[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}
};


class PointLight : public BaseObject {
public:
	glm::vec3 color;
	GLfloat strength;
	Shader* shader;
	unsigned int VBO, VAO;

	bool visible = true;

	float radius = 3.0;

	PointLight(Shader* shader, std::string name, glm::vec3 color = glm::vec3(1.0,1.0,1.0), GLfloat strength = 1.0) : BaseObject() {
		this->strength = strength;
		this->transform.scale = glm::vec3(0.2f, 0.2f, 0.2f);
		this->color = color;
		this->shader = shader;
		this->name = name;

		//test

		//sol::usertype<PointLight> player_type = lua.new_usertype<PointLight>(name,
		//sol::constructors<PointLight(Shader*, std::string, glm::vec3, GLfloat)>());
		properties.push_back(Property::TRANSFORM);
		properties.push_back(Property::LIGHT);

		type = POINT_LIGHT;
		init();
	};


	void draw() override {
		shader->Use();
		shader->setVec3("color", color);

		Transform temp;
		temp.position.x = parent->transform.position.x + transform.position.x;
		temp.position.y = parent->transform.position.y + transform.position.y;
		temp.position.z = parent->transform.position.z + transform.position.z;

		temp.scale = transform.scale;
		temp.rotation = transform.rotation;

		shader->setMat4("model", temp.get_matrix());
		shader->setFloat("strenght", strength);

		if (visible) {
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, low_poly_sphere.size()/3);
		}


	}

	void update() override {

		if (!script.empty()) {
			/*lua["pos_x"] = model[3].x;
			lua["pos_y"] = model[3].y;
			lua["pos_z"] = model[3].z;*/
			lua["r"] = color.r;
			lua["g"] = color.g;
			lua["b"] = color.b;

			try {
				lua.script(script);
			}
			catch (const std::exception& ex) {
				std::cout << ex.what();
				script = "";
			}

			/*model[3].x = lua["pos_x"];
			model[3].y = lua["pos_y"];
			model[3].z = lua["pos_z"];*/
			color.r = lua["r"];
			color.g = lua["g"];
			color.b = lua["b"];

			color.r = std::clamp(color.r, 0.0f, 1.0f);
			color.g = std::clamp(color.g, 0.0f, 1.0f);
			color.b = std::clamp(color.b, 0.0f, 1.0f);
		}	
	}

private:
	void init() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);


		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, low_poly_sphere.size() * sizeof(GLfloat), &low_poly_sphere[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}
};



class Line {
public:
	Shader* shader;
	unsigned int VBO, VAO;
	glm::vec3 color;
	std::vector<GLfloat> verts;
	glm::mat4 model;

	bool visible = true;


	Line(Shader* shader, glm::vec3 start_pos, glm::vec3 end_pos, glm::vec3 color = glm::vec3(1.0, 1.0, 1.0)) {
		this->model = glm::mat4(1.0f);
		this->color = color;
		this->shader = shader;
		verts.push_back(start_pos.x);
		verts.push_back(start_pos.y);
		verts.push_back(start_pos.z);
		verts.push_back(end_pos.x);
		verts.push_back(end_pos.y);
		verts.push_back(end_pos.z);
		init();
	};

	void draw() {
		shader->Use();
		shader->setVec3("color", color);
		shader->setMat4("model", model);
		shader->setFloat("strenght", 1.0);


		if (visible) {
			glBindVertexArray(VAO);
			glDrawArrays(GL_LINES, 0, 2);
		}
		
	}

	void update_vertex() {
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), &verts[0], GL_DYNAMIC_DRAW);
	}



private:
	void init() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);


		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), &verts[0], GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

};



class Bezier : public BaseObject {
public:
	Shader* shader;

	std::vector<Line*> lines;

	glm::vec3 p1 = glm::vec3(1.0, 0.0, 1.0);
	glm::vec3 p2 = glm::vec3(-1.0, 0.0, -1.0);
	glm::vec3 p3 = glm::vec3(1.0, 0.0,  0.0);

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
			glm::vec3 point2 = GetPoint(i+step);

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


		glm::vec3 f = glm::vec3(i*i*(p3-p1)-i*(p2-p1)+p2);

		return f;
	}
};



class VertexGroup {
public:
	Shader* shader;
	unsigned int VBO, VAO;
	std::vector<GLfloat> vertices;
	Transform* transform;
	Material* material = nullptr;

	Transform temp;


	//change it later;
	int vertex_stride = 14 * sizeof(GLfloat); //vertices = {{x, y, z, u, v, nx, ny, nz, tx, ty, tz, bx, by, bz}, ....} 

	VertexGroup(std::vector<GLfloat> data, Shader* shader) {
		this->vertices = data;
		//this->vertices = {-1,0,1,0,0,0,1,0, 0,0,0,0,0,0, 1,0,1,0,0,0,1,0, 0,0,0,0,0,0, -1,0,-1,0,0,0,1,0, 0,0,0,0,0,0};

		this->shader = shader;
		init();
	}

	~VertexGroup() {
		glDeleteBuffers(1, &VBO);
		glDeleteVertexArrays(1, &VAO);
	}


	int get_vertices_number() {
		return vertices.size() / 14;
	}

	void draw() {
		

		shader->setMat4("model", temp.get_matrix());

		if (material != nullptr) {
			shader->setFloat("normal_bump", material->normalmap_value);
			shader->setFloat("spec_factor", material->specular_value);
			shader->setFloat("reflection", material->reflection_value);
			shader->setFloat("ref_ratio", material->refract_value);
			shader->setBool("use_normal", material->use_normalmap);
			shader->setBool("use_specular", material->use_specular);
			shader->setFloat("shine", material->shine_value);
			shader->setFloat("dif", material->diffuse_value);
			shader->setVec3("diffuse_color", material->diffuse_color);
			shader->setBool("use_diffuse", material->use_diffuse);
			shader->setBool("use_blinn", material->use_blinn);
			shader->setVec2("uv_scale", material->uv_scale);
			shader->setInt("emit", material->emit);

			if (material->diffuse_texture != nullptr) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, material->diffuse_texture->id);
			}

			if (material->specular_texture != nullptr) {
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, material->specular_texture->id);
			}

			if (material->normalmap_texture != nullptr) {
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, material->normalmap_texture->id);
			}

		
		
		}

		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, material->specular_texture->id);
		//glActiveTexture(GL_TEXTURE2);
		//glBindTexture(GL_TEXTURE_2D, material->normalmap_texture->id);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 14);
	}

private:
	void init() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

		vertex_stride = 14 * sizeof(GLfloat);
		//Position x,y,z
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertex_stride, 0);
		//Uv u,v
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertex_stride, (void*)(3 * sizeof(GLfloat)));
		//Normal nx, ny, nz
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vertex_stride, (void*)(5 * sizeof(GLfloat)));
		//Tangent tx, ty, yz
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, vertex_stride, (void*)(8 * sizeof(GLfloat)));
		//Bitangent bx, by, bz (idk if it is bitangent)
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, vertex_stride, (void*)(11 * sizeof(GLfloat)));
	}
};




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


class MeshHolder : public BaseObject {
public:
	Shader* shader;
	Mesh* mesh = nullptr;


	MeshHolder(Shader* shader, std::string name) : BaseObject(){
		this->name = name;
		this->shader = shader;
		type = MESH_HOLDER;

		properties.push_back(Property::TRANSFORM);
		//properties.push_back(Property::TEXTURE);
	}

	~MeshHolder() {
	}




	void draw() override {
		if (mesh != nullptr) {
			shader->Use();

			Transform temp;
			temp.position.x = parent->transform.position.x + transform.position.x;
			temp.position.y = parent->transform.position.y + transform.position.y;
			temp.position.z = parent->transform.position.z + transform.position.z;

			temp.scale = transform.scale;
			temp.rotation = transform.rotation;


			mesh->temp = temp;
			mesh->transform = &transform;
			mesh->draw();
		}

		
	}
	

};


class ResourceManager {
public:
	std::vector<Texture*> textures;
	std::vector<Material*> materials;
	std::vector<Mesh*> meshes;


	void create_texture(std::string path) {
		Texture* texture = new Texture();
		GLuint texture_id;


		
		if (load_image(path, &texture_id)) {
			texture->id = texture_id;
			texture->path = path;
			textures.push_back(texture);
		}
		else {
			std::cout << "ERROR: Texture was not loaded";
		}
		
	}


	void create_mesh(std::string path, Shader *shader) {
		std::filesystem::path filepath = std::filesystem::path(path);


		wchar_t szPath[MAX_PATH];
		GetModuleFileNameW(NULL, szPath, MAX_PATH);


		std::vector<std::vector<GLfloat>> vertexGroups_data;

		std::vector<std::string> mat_names;
		std::map<std::string, Mat> materials;

		if (open_obj(path, &vertexGroups_data, &mat_names)) {
			std::filesystem::path obj_path = std::filesystem::path(path);
			std::string obj_path_s = obj_path.parent_path().string() + "/" + obj_path.stem().string() + ".mtl";


			open_mtl(obj_path_s, &materials);
			
			Mesh* mesh = new Mesh();

			int n = 0;
			for (std::vector<GLfloat> vertices : vertexGroups_data) {
				VertexGroup* vertexGroup = new VertexGroup(vertices, shader);

				vertexGroup->material = new Material();

				mesh->add_vertex_group(vertexGroup);
				
				if (materials.size() != 0) {

					vertexGroup->material->name = mat_names[n];
					/*mod->material->diffuse_texture->id = mod->load_tex(materials[mat_names[n]].dif_path, 0);
					mod->material->diffuse_color = materials[mat_names[n]].d_color;
					if (materials[mat_names[n]].dif_path == "") {
						mod->material->use_diffuse = false;
					}*/
				}

				n += 1;
			}


			
			meshes.push_back(mesh);


		}

	}


	void create_material() {
		//Material* material = new Material();
		//materials.push_back(material);
	}

};


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


