#pragma once
#include <Windows.h>
#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#define STB_IMAGE_IMPLEMENTATION
#define STBIW_WINDOWS_UTF8
#include <stb_image.h>

#define AL_LIBTYPE_STATIC
#include <AL/al.h>
#include <AL/alc.h>


#include <algorithm>
#include <filesystem>

#include "property_drawer.h"
#include "register_properties.h"

#include "transform.h"

#include <nfd.h>
#pragma comment(lib, "opengl32.lib")

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "imgui_docking/imgui.h"
#include "imgui_docking/imgui_impl_glfw.h"
#include "imgui_docking/imgui_impl_opengl3.h"
#include "imgui_docking/imgui_stdlib.h"
#include "imgui_docking/imgui_internal.h"

#include <json.hpp>

#include "bullet/btBulletDynamicsCommon.h"


#include "objects.h"
#include "cube.h"
#include "shader.h"


#include "santa_claus.h"

#include "bulletDebugDraw.h"

#include "resource_manager.h"

#include "camera.h"


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void drop_callback(GLFWwindow* window, int count, const char** paths);

using json = nlohmann::json;

const int SCREEN_WIDTH = 768;
const int SCREEN_HEIGHT = 768;
const char WINDOW_TITLE[] = "DrishEngine";

int window_width = SCREEN_WIDTH;
int window_height = SCREEN_HEIGHT;

float aspectRatio = SCREEN_WIDTH / SCREEN_HEIGHT;


double offs_cur_x, offs_cur_y = 0;

Material* selectedMaterial;

Camera* world_camera;

glm::vec3 ambientCol(0.0f, 0.0f, 0.0f);

float horizontalAngle = 3.14f;
float verticalAngle = 0.0f;


std::vector<BaseObject*> objects;

std::string save_scene_as = "test";

std::string script = "";


bool show_lines = true;
bool show_lights = true;
bool update_physics = false;

std::vector<std::string> sky_faces = {
	"assets/textures/negx.jpg",
	"assets/textures/posx.jpg",
	"assets/textures/posy.jpg",
	"assets/textures/negy.jpg",
	"assets/textures/negz.jpg",
	"assets/textures/posz.jpg"
};

Shader* emission_shader_ptr;
Shader* shader_ptr;

bool draw_depth = true;

ResourceManager resourceManager(&draw_depth);

VertexGroup* selected_vertex_group;

const float PI = 3.14;

bool selected = false;
BaseObject* selected_p = 0;


void error_callback(int error, const char* description)
{
	fprintf(stderr, "ERROR: %s\n", description);
	system("pause");
}





unsigned int loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	int pixels = 0;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			for (int x = 0; x < width; x += 1) {
				for (int y = 0; y < height; y += 1) {
					ambientCol.x += data[x * y * 3];
					ambientCol.y += data[x * y * 3 + 1];
					ambientCol.z += data[x * y * 3 + 2];
					pixels += 1;
				}
			}
			





			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	ambientCol.x = ambientCol.x / pixels / 255;
	ambientCol.y = ambientCol.y / pixels / 255;
	ambientCol.z = ambientCol.z / pixels / 255;

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

std::string open_file_dialog() {
	nfdu8char_t* outPath = NULL;
	nfdfilteritem_t filterItem[1] = {};
	nfdresult_t result = NFD_OpenDialogU8(&outPath, filterItem, 0, NULL);

	if (result == NFD_OKAY) {
		std::filesystem::path new_path = std::filesystem::u8path(outPath);
		free(outPath);
		return new_path.generic_string();
	}
	return "";
}




void jump_into_child(BaseObject* parent, std::vector<BaseObject*>* vector) {
	vector->push_back(parent);


	if (selected_p == parent && selected == true) {
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0, 1.0, 0.0, 1.0));
	}
	else {
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 1.0, 1.0, 1.0));
	}


	bool is_node_shown = ImGui::TreeNodeEx((parent->name).c_str(), ImGuiTreeNodeFlags_Leaf);



	if (true) {
		if (ImGui::IsItemClicked())
		{
			if (selected_p == parent && selected == true) {
				selected = false;
				script = "";
			}
			else {
				selected_p = parent;
				script = selected_p->script;
				selected = true;
				//if (selected_p->type == MESH) {
				//	//mod = Fselected_p)->mesh->models[0];
				//}

			}
		}
	}

	ImGui::PopStyleColor();



	if (parent->children.size() != 0) {
		for (BaseObject* child : parent->children) {
			jump_into_child(child, vector);
		}
	}

	if (is_node_shown) {
		ImGui::TreePop();
	}
}



int main() 
{
	glfwInit();
	NFD_Init();

	//-------------------------------IMGUI THEME---------------------------
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.IniFilename = NULL;

	float size_pixels = 15;
	
	io.Fonts->AddFontFromFileTTF("assets/fonts/microsoftsansserif.ttf", size_pixels);

	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4 accent_color = ImVec4(0.1f, 0.05f, 0.0f, 1.0f);

	style.Alpha = 1.0f;
	style.DockingSeparatorSize = 1.0f;
	style.WindowBorderSize = 1.0f;

	style.FramePadding = ImVec2(15, 2);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.071f, 0.071f, 0.071f, 1.0f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.051f, 0.051f, 0.051f, 1.0f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.071f, 0.071f, 0.071f, 1.0f);
	style.Colors[ImGuiCol_Header] = accent_color;
	style.Colors[ImGuiCol_Border] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_Tab] = accent_color;
	style.Colors[ImGuiCol_TabActive] = accent_color;
	style.Colors[ImGuiCol_TabUnfocused] = accent_color;
	style.Colors[ImGuiCol_TabUnfocusedActive] = accent_color;
	style.Colors[ImGuiCol_TitleBg] = accent_color;
	style.Colors[ImGuiCol_TitleBgActive] = accent_color;
	style.Colors[ImGuiCol_TitleBgCollapsed] = accent_color;

	//ImVec4 hl_col = ImVec4(0.0f, 0.5f, 0.1f, 1.0f);

	//style.Colors[ImGuiCol_Button] = hl_col;
	//style.Colors[ImGuiCol_CheckMark] = hl_col;
	//style.Colors[ImGuiCol_SliderGrab] = hl_col;
	//style.Colors[ImGuiCol_Header] = hl_col;
	//style.Colors[ImGuiCol_TitleBg] = hl_col;
	//--------------------------------------------------------------
		

	BulletDebugDraw* bulletDebugDraw = new BulletDebugDraw();


	std::cout << "Hello" << "\n";

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwSetErrorCallback(error_callback);
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE, nullptr, nullptr);

	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetDropCallback(window, drop_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);

	//---------------------------PHYSICS--------------------------------------
	btBroadphaseInterface* broadphase = new btDbvtBroadphase();
	// Set up the collision configuration and dispatcher
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
	// The actual physics solver
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
	// The world.
	btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, -9.81f, 0));

	dynamicsWorld->setDebugDrawer(bulletDebugDraw);
	//-----------------------------------------------------------------------



	
	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true); // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	Shader shader("standart.vertex", "standart.fragment", true);
	Shader screen_shader("screen.vertex", "screen.fragment", true);
	Shader emission_shader("emission.vertex", "emission.fragment", true);
	Shader depth_shader("depth.vertex", "depth.fragment", true);

	shader_ptr = &shader;
	emission_shader_ptr = &emission_shader;

	BaseObject *world = new BaseObject();
	world->name = "root";
	

	MeshHolder* obj1 = new MeshHolder(&shader, "Cube");
	MeshHolder* obj2 = new MeshHolder(&shader, "Phys");
	DirectionalLight* dl = new DirectionalLight(&emission_shader, "dL", glm::vec3(1.0f, 1.0f, 1.0f), 1.0);
	MeshHolder* sphere = new MeshHolder(&shader, "Sphere");
	PointLight* obj6 = new PointLight(&emission_shader, "PointLight");
	PointLight* obj7 = new PointLight(&emission_shader, "PointLight");
	obj6->transform.position.x = 2.0;
	dl->direction = glm::vec3(0.1, -1.5, 1.0);


	//WARNING: need to be removed to build;
	resourceManager.create_mesh("C:\\Users\\Admin\\Desktop\\GAME\\models\\sphere_smooth.obj", &shader, &depth_shader);
	resourceManager.create_mesh("C:\\Users\\Admin\\Desktop\\GAME\\models\\terrain.obj", &shader, &depth_shader);

	resourceManager.create_texture("C:\\Users\\Admin\\Desktop\\GAME\\images\\diff.tga");
	resourceManager.create_texture("C:\\Users\\Admin\\Desktop\\GAME\\images\\grass.png");
	
	MeshHolder* obj = new MeshHolder(&shader, "ShpereC");
	obj->mesh = resourceManager.meshes[0];
	obj->mesh->vertexGroups[0]->material->diffuse_texture = resourceManager.textures[0];
	obj->mesh->vertexGroups[0]->material->use_diffuse = true;


	
	/*for (int x = 0; x < 25; x++) {
		
		
		Transform transform;
		btCollisionShape* collisionShape = new btSphereShape(1);
		transform.position.y = x*3;
		RigidBody *rigidBody = new RigidBody(dynamicsWorld, collisionShape, transform, 10);

		rigidBody->add_child(obj);
		world->add_child(rigidBody);
	}*/
	
	
	
	obj2->mesh = resourceManager.meshes[0];
	obj1->mesh = resourceManager.meshes[1];
	obj1->mesh->vertexGroups[0]->material->diffuse_texture = resourceManager.textures[1];
	obj1->mesh->vertexGroups[0]->material->use_diffuse = true;

	sphere->mesh = resourceManager.meshes[0];
	
	selected_p = obj1;
	selected = true;


	world->add_child(obj7);
	world->add_child(obj6);
	world->add_child(dl);

	btCollisionShape* bCS = new btSphereShape(1);
	btCollisionShape* bCS2 = new btBoxShape({50, 1, 50});
	btCollisionShape* bCS3 = new btSphereShape(1);
	Transform tr;
	tr.position.y = 5;
	Transform tr2;
	tr2.position.y = -5;
	tr.position.x = 0.5;
	Transform tr3;
	tr3.position.y = 0;
	RigidBody* rgb = new RigidBody(dynamicsWorld, bCS, tr, 1);
	RigidBody* rgb2 = new RigidBody(dynamicsWorld, bCS2, tr2, 0);
	RigidBody* rgb3 = new RigidBody(dynamicsWorld, bCS3, tr3, 5);
	obj1->transform.scale.x = 10;
	obj1->transform.scale.z = 10;
	obj1->transform.scale.y = 10;
	
	rgb->name = "rigidbody";
	rgb->add_child(obj2);
	world->add_child(rgb);
	world->add_child(rgb2);
	rgb3->add_child(sphere);
	world->add_child(rgb3);
	rgb2->name = "rgb2";
	rgb2->add_child(obj1);
	obj7->transform.position.y = 2.0;

	Camera* world_camera = new Camera();


	
	std::vector<PointLight*> point_lights;

	//std::vector<>

	int point_lights_max_size = 64;


	std::vector<Line*> lines;

	Line* line = new Line(&emission_shader, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 3.0f, 0.0f));
	line->color = glm::vec3(0.0f, 1.0f, 0.0f);
	lines.push_back(line);

	line = new Line(&emission_shader, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(3.0f, 0.0f, 0.0f));
	line->color = glm::vec3(1.0f, 0.0f, 0.0f);
	lines.push_back(line);

	line = new Line(&emission_shader, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 3.0f));
	line->color = glm::vec3(0.0f, 0.0f, 1.0f);
	lines.push_back(line);


	Bezier* l = new Bezier(&emission_shader);
	world->add_child(l);

	float test = 0;


	float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};


	//UI reg
	PropertyDrawer drawer;
	registerProperties(&drawer);



	screen_shader.Use();
	screen_shader.setInt("screenTexture", 0);

	// framebuffer configuration
	// -------------------------
	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	// create a color attachment texture
	unsigned int textureColorbuffer;
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//shadows 

	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);

	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);




	


	unsigned int quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	shader.Use();
	glUniform1i(glGetUniformLocation(shader.ID, "oText"), 0);
	glUniform1i(glGetUniformLocation(shader.ID, "sText"), 1);
	glUniform1i(glGetUniformLocation(shader.ID, "nText"), 2);
	glUniform1i(glGetUniformLocation(shader.ID, "skybox"), 3);
	glUniform1i(glGetUniformLocation(shader.ID, "shadowMap"), 4);

	//cubemap
	Shader sky_shader("sky.vertex", "sky.fragment", true);


	unsigned int SKY_VBO, SKY_VAO;
	glGenVertexArrays(1, &SKY_VAO);
	glGenBuffers(1, &SKY_VBO);


	glBindVertexArray(SKY_VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, SKY_VBO);
	glBufferData(GL_ARRAY_BUFFER, cube.size() * sizeof(GLfloat), &cube[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);


	unsigned int cubemapTexture = loadCubemap(sky_faces);


	world_camera->view = glm::translate(world_camera->view, glm::vec3(0.0f, -0.2f, -3.0f));


	float speed = 0.3f;
	float mouseSpeed = 0.005f;

	

	bool show_wireframe_if_selected = true;

	float mouse_delta = 0;
	float mouse_old_delta = 0;

	bool left_mouse_clicked = false;

	float ambient_color[3] = { 0.0f, 0.0f, 0.0f };
	ambient_color[0] = ambientCol.x;
	ambient_color[1] = ambientCol.y;
	ambient_color[2] = ambientCol.z;

	ImGuiID id;
	//test

	bool oneRun = true;

	float directions = 16;
	float size = 0.01;
	float quality = 3.0;

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //bg color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//view controls
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {

			if (left_mouse_clicked == false) {
				left_mouse_clicked = true;
				glfwSetCursorPos(window, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
			}

			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			glfwSetCursorPos(window, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
			horizontalAngle += mouseSpeed * float(SCREEN_WIDTH / 2 - xpos);
			verticalAngle += mouseSpeed * float(SCREEN_HEIGHT / 2 - ypos);
		}
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
			left_mouse_clicked = false;
		}

		if (verticalAngle < -3.14f / 2.0f) verticalAngle = -3.14f / 2.0f;
		if (verticalAngle > 3.14f / 2.0f) verticalAngle = 3.14f / 2.0f;




		


		glm::vec3 a = glm::vec3(l->GetPoint(test).x, l->GetPoint(test).y,  l->GetPoint(test).z);
		glm::vec3 b = glm::vec3(l->GetPoint(test + 0.02).x, l->GetPoint(test + 0.02).y,  l->GetPoint(test + 0.02).z);

		

		float x = b.x - a.x;
		float z = b.z - a.z;
		float angle = atan2(x, z) - 3.14 / 2.0;


		x = b.z - a.z;
		float y = b.y - a.y;
		float angle2 = atan2(x, y) - 3.0 / 2.0;


		glm::vec3 np = glm::normalize((b - a));

		




		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			horizontalAngle = atan2(np.x, np.z);
			verticalAngle = atan2(np.x, np.y) - 3.14 / 2;
			world_camera->position = glm::vec3(l->GetPoint(test).x, l->GetPoint(test).y, l->GetPoint(test).z);
		}

		glm::vec3 direction(
			cos(verticalAngle) * sin(horizontalAngle),
			sin(verticalAngle),
			cos(verticalAngle) * cos(horizontalAngle)
		);

		glm::vec3 right = glm::vec3(
			sin(horizontalAngle - 3.14f / 2.0f),
			0,
			cos(horizontalAngle - 3.14f / 2.0f)
		);

		glm::vec3 up = glm::cross(right, direction);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			world_camera->position += direction * speed;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			world_camera->position -= direction * speed;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			world_camera->position += right * speed;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			world_camera->position -= right * speed;
		}

		world_camera->view = glm::lookAt(
			world_camera->position,           // Camera is here
			world_camera->position + direction, // and looks here : at the same position, plus "direction"
			up                  // Head is up (set to 0,-1,0 to look upside-down)
		);



		//gui start
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//docking 
		
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGuiID idd = ImGui::DockSpaceOverViewport(0, viewport, ImGuiDockNodeFlags_PassthruCentralNode);
		if (oneRun) {
			
			ImVec2 workCenter = ImGui::GetMainViewport()->GetWorkCenter();
			id = ImGui::GetID("DrishEngine"); // The string chosen here is arbitrary (it just gives us something to work with)
			ImGui::DockBuilderRemoveNode(id);             // Clear any preexisting layouts associated with the ID we just chose
			ImGui::DockBuilderAddNode(id);
			ImVec2 nodePos{ workCenter.x - SCREEN_WIDTH * 0.5f, workCenter.y - SCREEN_HEIGHT * 0.5f };
			ImGui::DockBuilderSetNodeSize(id, ImVec2(SCREEN_WIDTH, SCREEN_HEIGHT));
			ImGui::DockBuilderSetNodePos(id, nodePos);
			ImGuiID dock1 = ImGui::DockBuilderSplitNode(idd, ImGuiDir_Right, 0.5f, nullptr, &idd);
			ImGuiID dock2 = ImGui::DockBuilderSplitNode(dock1, ImGuiDir_Down, 0.75f, nullptr, &dock1);
			ImGuiID dock3 = ImGui::DockBuilderSplitNode(idd, ImGuiDir_Left, 0.7f, nullptr, &idd);
			ImGuiID dock4 = ImGui::DockBuilderSplitNode(idd, ImGuiDir_Down, 0.2f, nullptr, &idd);
			ImGuiID dock5 = ImGui::DockBuilderSplitNode(dock3, ImGuiDir_Down, 0.3f, nullptr, &dock3);
			ImGui::DockBuilderDockWindow("Tree", dock1);
			ImGui::DockBuilderDockWindow("Inspector", dock2);
			ImGui::DockBuilderDockWindow("Other", dock3);
			ImGui::DockBuilderDockWindow("Resources", dock4);
			ImGui::DockBuilderFinish(id);
			oneRun = false;
		}
		


		ImGui::Begin("Resources");

		if (ImGui::Button("Add Texture")) {
			std::string texture_path = open_file_dialog();
			if (!texture_path.empty()) {
				resourceManager.create_texture(texture_path);
			}
		}
		ImGui::SameLine();


		if (ImGui::Button("Add 3d model (.obj)")) {
			std::string model_path = open_file_dialog();
			if (!model_path.empty()) {
				resourceManager.create_mesh(model_path, &shader, &depth_shader);
			}

		}
		ImGui::SameLine();
		if (ImGui::Button("Add material")) {

		}

		
		ImGui::BeginChild("Textures", ImVec2(200, 0), ImGuiChildFlags_Border);

		for (int i = 0; i < resourceManager.textures.size(); i++) {
			Texture *texture = resourceManager.textures[i];

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
			ImGui::ImageButton(("id##"+std::to_string(texture->id)).c_str(), texture->id, 
				ImVec2(32, 32), ImVec2(0.0f,0.0f), ImVec2(1.0f, 1.0f), ImVec4(0.0, 0.0, 0.0, 1.0), ImVec4(1.0, 1.0, 1.0, 1.0));
			
			ImGui::PopStyleVar();

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
				ImGui::SetDragDropPayload("TEXTURE", &i, sizeof(int));

				ImGui::Image(texture->id, ImVec2(32, 32));
				ImGui::EndDragDropSource();
			}

			ImGui::SameLine();
			ImGui::Text(texture->path.c_str());

		}
		ImGui::EndChild();

		ImGui::SameLine();



		ImGui::BeginChild("Meshes", ImVec2(200, 0), ImGuiChildFlags_Border);

		for (int i = 0; i < resourceManager.meshes.size(); i++) {
			ImGui::Button(("Mesh" + std::to_string(i) + " vertices: " + std::to_string(resourceManager.meshes[i]->vertice_number)).c_str());

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
				ImGui::SetDragDropPayload("MESH", &i, sizeof(int));
				ImGui::Text(("Mesh" + std::to_string(i) + " vertices: " + std::to_string(resourceManager.meshes[i]->vertice_number)).c_str());
				ImGui::EndDragDropSource();
			}
		}

		ImGui::EndChild();


		/*if (ImGui::BeginListBox("##box")) {
			std::filesystem::path new_path = project_path.parent_path();



			for (const auto& entry : std::filesystem::recursive_directory_iterator(new_path, std::filesystem::directory_options::skip_permission_denied)) {
				std::string path = entry.path().string();
				ImGui::Text(path.c_str());
			}


			ImGui::EndListBox();
		}*/
		ImGui::End();


		
		//tree 
		ImGui::Begin("Tree");
		std::vector<BaseObject*> treeObjects;
		jump_into_child(world, &treeObjects);
		ImGui::End();

		
		

		static float sky_val = 1.0;
		ImGui::Begin("Other", 0, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::SeparatorText("Add");
		if (ImGui::Button("Add point light")) {
			PointLight* ob = new PointLight(&emission_shader, "PointLight");
			
			if (selected) {
				selected_p->add_child(ob);
			}
		}

		if (ImGui::Button("Add directional light")) {
			DirectionalLight* ob = new DirectionalLight(&emission_shader, "DirLight");

			if (selected) {
				selected_p->add_child(ob);
			}
		}

		if (ImGui::Button("Add meshHolder")) {
			MeshHolder* ob = new MeshHolder(&shader, "MeshHolder");

			if (selected) {
				selected_p->add_child(ob);
			}

		}


		ImGui::SliderFloat("Sky Value", &sky_val, 0.0, 1.0);

		ImGui::SeparatorText("Settings");
		ImGui::Text("Skybox Light");
		ImGui::Text("FOV");
		ImGui::SliderFloat("##FOV", &world_camera->fov, 1.0f, 179.0f);
		ImGui::Checkbox("Show wireframe", &show_wireframe_if_selected);
		ImGui::Checkbox("Show lines", &show_lines);
		ImGui::Checkbox("Show lights", &show_lights);
		ImGui::Checkbox("Update Physics", &update_physics);
		static bool debugDraw = false;

		ImGui::Checkbox("Physics Collisions Draw", &debugDraw);


		
		ImGui::End();

		ImGui::Begin("Inspector");
		if (selected) {
			drawer.draw(selected_p);
		}
		ImGui::End();
		
		//proj mat
		glm::mat4 projection(1.0f);

		projection = glm::perspective(world_camera->fov / 180.0f * 3.14f, aspectRatio, 0.1f, 1000.0f);


		


		//ImGui::Begin("rays");

		double xpos = 0;
		double ypos = 0;

		glfwGetCursorPos(window, &xpos, &ypos);



		

		glm::vec3 worldPos = glm::unProject(glm::vec3(xpos, window_height-ypos, 1.0),
			world_camera->view, projection,
			glm::vec4(0, 0, window_width, window_height));
		glm::vec3 rayMouse = glm::normalize(worldPos - world_camera->position);

		//ImGui::Text("world pos %f", worldPos.x);
		//ImGui::Text("ray_mouse %f,   %f,    %f", rayMouse.x, rayMouse.y, rayMouse.z);

		//ImGui::End();

		int distance = 100;

		btVector3 from = btVector3(world_camera->position.x, world_camera->position.y, world_camera->position.z);
		btVector3 to = btVector3(world_camera->position.x + rayMouse.x * distance, world_camera->position.y + rayMouse.y * distance,
			world_camera->position.z + rayMouse.z * distance);


		btCollisionWorld::ClosestRayResultCallback closestResults = btCollisionWorld::ClosestRayResultCallback(from, to);
		dynamicsWorld->rayTest(from, to, closestResults);

		

		if (closestResults.hasHit()) {
			btVector3 hit_vector = closestResults.m_hitPointWorld;

			closestResults.m_collisionObject;

			glm::vec3 pos = glm::vec3(0.0, 0.0, 0.0);
			pos.x = hit_vector.getX();
			pos.y = hit_vector.getY();
			pos.z = hit_vector.getZ();
			world->children[0]->transform.position = pos;
		}

		//world->children[0]->transform.position = camera->position + rayMouse * 4.0f;



	
		glm::mat4 view2 = glm::mat4(glm::mat3(world_camera->view));
		glDepthMask(GL_FALSE);
		sky_shader.Use();
		sky_shader.setMat4("projection", projection);
		sky_shader.setMat4("view", view2);
		sky_shader.setFloat("value", sky_val);


		glBindVertexArray(SKY_VAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(GL_TRUE);


		

		//emission
		emission_shader.Use();
		emission_shader.setMat4("projection", projection);
		emission_shader.setMat4("view", world_camera->view);
		//emission_shader.setVec3("color", glm::vec3(ambient_color[0], ambient_color[1], ambient_color[2]));

		\
		// shader standart
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

		shader.Use();
		shader.setVec3("lightColor", glm::vec3(ambient_color[0], ambient_color[1], ambient_color[2]));
		shader.setVec3("viewPos", world_camera->position);
		shader.setMat4("projection", projection); 
		shader.setMat4("view", world_camera->view);
		shader.setFloat("sky_val", sky_val);

		
		
		//test
		if (point_lights.size() > point_lights_max_size) {
			throw;
		}

		float near_plane = 1.0f, far_plane = 40.0f;
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f,
			near_plane, far_plane);
		glm::mat4 dirlightView = glm::mat4(1.0);
		
		
		int point_lights_size = 0;
		int directional_lights_number = 0;
		int n = 0;
		int n2 = 0;
		for (BaseObject* obj : treeObjects) {
			if (obj->type == POINT_LIGHT) {
				PointLight* light = static_cast<PointLight*>(obj);


				shader.setVec3("pointLights[" + std::to_string(n) + "].position", light->transform.position);
				shader.setVec3("pointLights[" + std::to_string(n) + "].color", light->color);
				shader.setFloat("pointLights[" + std::to_string(n) + "].strength", light->strength);
				shader.setFloat("pointLights[" + std::to_string(n) + "].radius", light->radius);
				n += 1;
				point_lights_size += 1;
			}

			if (obj->type == DIRECTIONAL_LIGHT) {
				DirectionalLight* light = static_cast<DirectionalLight*>(obj);


				dirlightView = glm::lookAt(light->transform.position,
					light->transform.position + light->direction,
					glm::vec3(0.0f, -1.0f, 0.0f));

				//dirlightView = glm::translate(dirlightView, light->transform.position);

				shader.setVec3("directionalLights[" + std::to_string(n2) + "].direction", light->direction);
				shader.setVec3("directionalLights[" + std::to_string(n2) + "].color", light->color);
				shader.setFloat("directionalLights[" + std::to_string(n2) + "].strength", light->strength);
				directional_lights_number++;
				n2++;
			}
		}
		shader.setInt("light_number", point_lights_size);
		shader.setInt("directional_light_number", directional_lights_number);
		


		ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
		if (ImGui::BeginMainMenuBar())
		{
			ImGui::Text("LN %f", directional_lights_number);
			ImGui::Text("Light Number %i / %i", point_lights_size, point_lights_max_size);



			ImGui::EndMainMenuBar();
		}
		ImGui::PopStyleColor();

		
		screen_shader.Use();


		

		//ImGui::Begin("test");
		

		


		//ImGui::SliderFloat("size", &size, 0.00f, 0.1f);
		//ImGui::SliderFloat("quality", &quality, 0.0f, 15.0f);
		//ImGui::SliderFloat("directions", &directions, 0.0f, 360.0f);

		screen_shader.setFloat("Size", size);
		screen_shader.setFloat("Quality", quality);
		screen_shader.setFloat("Directions", directions);

		//ImGui::End();


		for (Line* line : lines) {
			line->visible = show_lines;
			line->draw();
			
		}




		test = 0.5 * (1+sin(glfwGetTime()/5.0));



		if (update_physics) {
			dynamicsWorld->stepSimulation(1.0f / 60.0f);
		}
		
		//obj = dynamicsWorld->getCollisionObjectArray()[2];
		//body = btRigidBody::upcast(obj);

	
		//simpleDepthShader.Use();
		//simpleDepthShader.setMat4("projection", projection);
		//simpleDepthShader.setMat4("view", camera->view);

		glm::mat4 lightSpaceMatrix = lightProjection * dirlightView;

		//lightSpaceMatrix = glm::scale(lightSpaceMatrix, glm::vec3(1.0, -1.0, 1.0));

		
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		
		draw_depth = true;
		depth_shader.Use();
		depth_shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		
		//glCullFace(GL_FRONT);
		for (BaseObject* obj : treeObjects) {
				obj->draw();		

		}
		//glCullFace(GL_BACK);

		ImGui::Begin("depth");
		ImGui::Image(depthMap, ImVec2(400, 400));
		ImGui::End();


		shader.Use();
		shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		
		glViewport(0, 0, window_width, window_height);
		
		


		draw_depth = false;
		for (BaseObject* obj : treeObjects) {
			if (selected_p == obj && selected == true && show_wireframe_if_selected == true) {
				obj->draw();
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				shader.setFloat("emission", 0.5f);
				obj->transform.scale += glm::vec3(0.05f, 0.05f, 0.05f);
				obj->draw();
				obj->transform.scale -= glm::vec3(0.05f, 0.05f, 0.05f);
				//glEnable(GL_DEPTH_TEST);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				shader.setFloat("emission", 0.0);
			}
			else {
				obj->draw();
			}
			obj->lua["time"] = glfwGetTime();
			obj->update();
		}

		emission_shader.Use();
		emission_shader.setMat4("projection", projection);
		emission_shader.setMat4("view", world_camera->view);
		emission_shader.setVec3("color", glm::vec3(1.0, 0.0, 0.0));

		
		if (debugDraw == true) {
			dynamicsWorld->debugDrawWorld();
		}

		glBindVertexArray(0);
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	NFD_Quit();
	glfwTerminate();

	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	window_width = width;
	window_height = height;
	aspectRatio = (float)width / (float)height;
	if ((int)height == 0) {
		aspectRatio = 1;
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == 1) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == 0) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

}


void drop_callback(GLFWwindow* window, int count, const char** paths)
{
	
	int i = 0;

	for (i = 0; i < count; i++) {
		std::filesystem::path path = paths[i];
		if (path.extension() == ".scene") {
			//load_scene(path.generic_string());

		}
	}
}
