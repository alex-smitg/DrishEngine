#ifndef ObjectsH
#define ObjectsH

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

GLuint load_texture(std::string path);


//types : 0 == mesh

enum Type {NO_TYPE, MESH, POINT_LIGHT, CURVE};
enum Attribute {POSITION, COLOR, TEXTURE, CURVE_VERT, LIGHT};


class BaseObject {
public:
	glm::mat4 model;
	std::string name;

	std::string script;

	glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);

	Type type = NO_TYPE;
	std::vector<Attribute> attributes;

	std::vector<BaseObject*> children;

	sol::state lua;

	

	BaseObject() {

		lua.open_libraries(sol::lib::base, sol::lib::math);
		
	
		
	}

	virtual void draw(){
		;
	}

	virtual void update() {
		if (!script.empty()) {
			lua["pos_x"] = model[3].x;
			lua["pos_y"] = model[3].y;
			lua["pos_z"] = model[3].z;

			lua["rot_x"] = rotation.x;
			lua["rot_y"] = rotation.y;
			lua["rot_z"] = rotation.z;


			try {
				lua.script(script);
			}
			catch (const std::exception& ex) {
				std::cout << ex.what();
				script = "";
			}

			model[3].x = lua["pos_x"];
			model[3].y = lua["pos_y"];
			model[3].z = lua["pos_z"];


			rotation.x = lua["rot_x"];
			rotation.y = lua["rot_y"];
			rotation.z = lua["rot_z"];
		}


		glm::mat4 rot_mat = glm::mat4(1.0);

		rot_mat = glm::translate(rot_mat, glm::vec3(model[3].x, model[3].y, model[3].z)); //non scale 

		rot_mat = glm::rotate(rot_mat, rotation.x, glm::vec3(1, 0, 0));
		rot_mat = glm::rotate(rot_mat, rotation.y, glm::vec3(0, 1, 0));
		rot_mat = glm::rotate(rot_mat, rotation.z, glm::vec3(0, 0, 1));

		model = rot_mat;

	
		
	}

	virtual ~BaseObject() {
		std::cout << "delete";
		
		for (BaseObject* child : children) {
			delete child;
		}
	}

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

class Space {
public:
	glm::vec3 color;
	GLfloat strength;
	Shader* shader;
	unsigned int buffer, VAO, VBO;
	glm::mat4 model;
	GLuint MBO;
	bool visible = true;

	unsigned int amount = 100000;
	float linear = 3.0;
	glm::mat4* modelMatrices;

	Space(Shader* shader) {
		this->strength = strength;
		this->model = glm::mat4(1.0f);
		this->color = color;
		this->shader = shader;
		

		

		modelMatrices = new glm::mat4[amount];

		for (unsigned int i = 0; i < amount; i++)
		{

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(std::rand()/ 32767.0f * 10.0 - 5, std::rand() / 32767.0f * 10.0 - 5, std::rand() / 32767.0f * 10.0 - 5));
			float sc = std::rand() / 32767.0f / 20.0f;
			model = glm::scale(model, glm::vec3(sc, sc, sc));
			modelMatrices[i] = model;

		}

		init();
	};


	void draw() {
		shader->Use();


		glBindVertexArray(VAO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, cube.size(), amount);
		glBindVertexArray(0);
		

		
	}

private:
	void init() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &buffer);
		glGenBuffers(1, &VBO);



		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, cube.size() * sizeof(GLfloat), &cube[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		
		glBindVertexArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

		glBindVertexArray(VAO);

		std::size_t vec4Size = sizeof(glm::vec4);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
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
		this->model = glm::mat4(1.0f);
		this->model = glm::scale(this->model, glm::vec3(0.2f, 0.2f, 0.2f));
		this->color = color;
		this->shader = shader;
		this->name = name;

		//test

		//sol::usertype<PointLight> player_type = lua.new_usertype<PointLight>(name,
		//sol::constructors<PointLight(Shader*, std::string, glm::vec3, GLfloat)>());
		attributes.push_back(Attribute::POSITION);
		attributes.push_back(Attribute::LIGHT);

		type = POINT_LIGHT;
		init();
	};


	void draw() override {
		shader->Use();
		shader->setVec3("color", color);
		shader->setMat4("model", model);
		shader->setFloat("strenght", strength);

		if (visible) {
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, low_poly_sphere.size()/3);
		}


	}

	void update() override {
		if (!script.empty()) {
			lua["pos_x"] = model[3].x;
			lua["pos_y"] = model[3].y;
			lua["pos_z"] = model[3].z;
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

			model[3].x = lua["pos_x"];
			model[3].y = lua["pos_y"];
			model[3].z = lua["pos_z"];
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
		
		this->type = Type::CURVE;

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



class Model{
public:
	Shader* shader;
	unsigned int VBO, VAO;
	std::vector<GLfloat> verts;
	glm::mat4* model;
	struct TexturesPath {
		std::string diffuse_path;
		std::string specular_path;
		std::string normal_path;
	} texturesPath;

	struct Textures {
		GLuint diffuse;
		GLuint specular;
		GLuint normal;
	} textures;

	struct Material {
		float normal_bump = 0.0;
		float specular = 1.0;
		float reflection = 0.0;
		float diffuse = 1.0f;
		float ref_ratio = 0.256f;
		float shine = 18.0;
		float uv_scale = 1.0;

		bool use_normal = false;
		bool use_specular = false;
		bool use_diffuse = true;
		bool use_blinn = false;
		bool emit = false;

		glm::vec3 diffuse_color = glm::vec3(1.0f, 1.0f, 1.0f);

	} material;



	Model(std::vector<GLfloat> data, Shader* shader) {
		this->verts = data;
		this->shader = shader;
		init();
	}

	~Model() {	
		clean_up();
	}

	void clean_up() {
		glDeleteTextures(1, &textures.diffuse);
		glDeleteTextures(1, &textures.specular);
		glDeleteTextures(1, &textures.normal);

		glDeleteBuffers(1, &VBO);
		glDeleteVertexArrays(1, &VAO);
	}


	void draw(){
		shader->setMat4("model", *model);
		shader->setFloat("normal_bump", material.normal_bump);
		shader->setFloat("spec_factor", material.specular);
		shader->setFloat("reflection", material.reflection);
		shader->setFloat("ref_ratio", material.ref_ratio);
		shader->setBool("use_normal", material.use_normal);
		shader->setBool("use_specular", material.use_specular);
		shader->setFloat("shine", material.shine);
		shader->setFloat("dif", material.diffuse);
		shader->setVec3("diffuse_color", material.diffuse_color);
		shader->setBool("use_diffuse", material.use_diffuse);
		shader->setBool("use_blinn", material.use_blinn);
		shader->setFloat("uv_scale", material.uv_scale);
		shader->setInt("emit", material.emit);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures.diffuse);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textures.specular);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, textures.normal);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, verts.size() / 14);
	}

	GLint load_tex(std::string path, int type) {
		std::filesystem::path filepath = std::filesystem::path(path);

		wchar_t szPath[MAX_PATH];
		GetModuleFileNameW(NULL, szPath, MAX_PATH);

		std::string rel_path;

		if (filepath == "images/no_texture.png") {
			rel_path = "images/no_texture.png";
		}
		else {
			rel_path = std::filesystem::relative(filepath, std::filesystem::path(szPath).remove_filename()).generic_u8string();
		}

		


		if (type == 0) { //diffuse
			texturesPath.diffuse_path = rel_path;
		}
		if (type == 1) { //diffuse
			texturesPath.specular_path = rel_path;
		}
		if (type == 2) { //diffuse
			texturesPath.normal_path = rel_path;
		}

		return load_texture(path);
	}


	

private:
	void init() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), &verts[0], GL_STATIC_DRAW);

		int vertex_stride = 14 * sizeof(GLfloat);
		// pos
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertex_stride, 0);

		// tex
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertex_stride, (void*)(3 * sizeof(GLfloat)));
		//norm
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vertex_stride, (void*)(5 * sizeof(GLfloat)));

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, vertex_stride, (void*)(8 * sizeof(GLfloat)));

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, vertex_stride, (void*)(11 * sizeof(GLfloat)));


		textures.diffuse = load_tex("images/no_texture.png", 0);
		textures.specular = load_tex("images/no_texture.png", 1);
		textures.normal = load_tex("images/no_texture.png", 2);
	}
};



class Mesh : public BaseObject {
public:
	std::vector<std::vector<GLfloat>> models_data;
	std::vector<Model*> models;
	Shader* shader;
	std::vector<std::string> mat_names;
	std::map<std::string, Mat> materials;

	std::string rel_path;
	bool no_model = false;
	bool no_material_data = false;

	Mesh(const std::string path, Shader* shader, std::string name) : BaseObject(){
		this->model = glm::mat4(1.0f);
		this->name = name;
		this->shader = shader;
		type = MESH;

		attributes.push_back(Attribute::POSITION);
		attributes.push_back(Attribute::TEXTURE);
		
		std::filesystem::path filepath = std::filesystem::path(path);

		//rel_path = "models/" + filepath.filename().generic_string();

		wchar_t szPath[MAX_PATH];
		GetModuleFileNameW(NULL, szPath, MAX_PATH);


		rel_path = std::filesystem::relative(filepath, std::filesystem::path(szPath).remove_filename()).generic_u8string();
		

		if (open_obj(path, &models_data, &mat_names)) {//verts = [x, y, z, u, v, nx, ny, nz, tx, ty, tz, bx, by, bz]
			no_model = true;
		}
		else {
			std::filesystem::path obj_path = std::filesystem::path(path);
			std::string obj_path_s = obj_path.parent_path().string() + "/" + obj_path.stem().string() + ".mtl";

			
			if (open_mtl(obj_path_s, &materials)) {
				no_material_data = true;
			};


			int n = 0;
			for (std::vector<GLfloat> model_verts : models_data) {
				Model* mod = new Model(model_verts, shader);
				mod->model = &model;
				models.push_back(mod);
				if (no_material_data == false && materials.size() != 0) {
					mod->textures.diffuse = mod->load_tex(materials[mat_names[n]].dif_path, 0);
					mod->material.diffuse_color = materials[mat_names[n]].d_color;
					if (materials[mat_names[n]].dif_path == "") {
						mod->material.use_diffuse = false;
					}
				}

				n += 1;
			}
		}
	};

	~Mesh() {
		int s = models.size();
		for (int i = 0; i < s; i++) {
			delete models[i];
		}

		models.clear();
	}

	void draw() override {
		shader->Use();

		for (Model* mod : models) {
			mod->draw();
		}
	}


};


GLuint load_texture(std::string path) {
	int width, height, nrChannels;


	unsigned char* img = stbi_load(path.c_str(), &width, &height, &nrChannels, 4); //diff

	
	GLuint white = 0xFFFFFFFF;
	GLuint black = 0xFF000000;
	GLfloat pixels[12] = { 1.0f, 1.0f, 1.0f, 0.0f,  0.0f,  0.0f, 0.0f,  0.0f,  0.0f, 1.0f, 1.0f, 1.0f, };

	if (!img) {
		std::cout
			<< " unable to load image: "
			<< stbi_failure_reason()
			<< "\n";
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

	return texture;
}

#endif


