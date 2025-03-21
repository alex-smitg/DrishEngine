#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "texture.h"
#include "mesh.h"
#include "material.h"

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


	void create_mesh(std::string path, Shader* shader) {
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