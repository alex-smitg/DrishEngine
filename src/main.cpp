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

#include <algorithm>
#include <filesystem>

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

#include "objects.h"
#include "cube.h"
#include "shader.h"

#include "inspector.h"
#include "santa_claus.h"

#include "bullet/btBulletDynamicsCommon.h"




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

float fov = 75;

double offs_cur_x, offs_cur_y = 0;

bool show_lines = true;
bool show_lights = true;

GLuint icon;
GLuint icon2;
GLuint icon3;


Material* selectedMaterial;
Camera* world_camera;


glm::vec3 ambientCol(0.0f, 0.0f, 0.0f);

float horizontalAngle = 3.14f;
// vertical angle : 0, look at the horizon
float verticalAngle = 0.0f;


std::vector<BaseObject*> objects;

std::string save_scene_as = "test";


std::string script = "";


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

ResourceManager resourceManager;

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



void add_mesh() {
}

void save_scene(std::vector<BaseObject*>* tree, std::string name, Camera* camera);
void load_scene(std::string path);


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
				//	//mod = static_cast<MeshHolder*>(selected_p)->mesh->models[0];
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
	//-----------------------------------------------------------------------



	
	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true); // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}


	std::fstream config_file;
	config_file.open("conf.ini");
	if (config_file.peek() == std::ifstream::traits_type::eof()) {
		;
	}

	else {


	}



	bool should_close = 0;
	std::filesystem::path project_path;
	project_path = "C:/Users/AlexSmith/Desktop/GAME/game.drish";




	while (project_path.empty()) {
		glClearColor(0.0f, 0.1f, 0.0f, 1.0f); //bg color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

	
		



		ImGui::Begin("test");
		if (ImGui::Button("Load Project")) {
			nfdu8char_t* outPath = NULL;
			nfdfilteritem_t filterItem[1] = { {"Project", "drish"} };
			nfdresult_t result = NFD_OpenDialogU8(&outPath, filterItem, 1, NULL);

			if (result == NFD_OKAY) {
				std::filesystem::path new_path = std::filesystem::u8path(outPath);
				free(outPath);
				project_path = new_path;
				should_close = 0;

			}

		}

		if (ImGui::Button("New Project")) {
			nfdu8char_t* outPath = NULL;
			nfdfilteritem_t filterItem[1] = { {"Project", "drish"} };
			nfdresult_t result = NFD_SaveDialogU8(&outPath, filterItem, 1, "", "");

			if (result == NFD_OKAY) {
				std::filesystem::path new_path = std::filesystem::u8path(outPath);
				free(outPath);
				project_path = new_path;
				should_close = 0;

				std::ofstream file;
				file.open(project_path);
				file << "Test";

			}
		}


		ImGui::End();



		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents();

		if (should_close == 0) {
			break;
		}


	}

	if (should_close == 1) {
		NFD_Quit();
		glfwTerminate();
		std::cout << "Bye";
		return 0;
	}

	std::cout << project_path;





	Shader shader("standart.vertex", "standart.fragment", true);
	Shader screen_shader("screen.vertex", "screen.fragment", true);
	Shader emission_shader("emission.vertex", "emission.fragment", true);

	shader_ptr = &shader;
	emission_shader_ptr = &emission_shader;
	//Shader space("space.vertex", "space.fragment", true);
	//Space* space_obj = new Space(&space);


	BaseObject *world = new BaseObject();
	world->name = "root";
	

	MeshHolder* obj1 = new MeshHolder("C:/Users/AlexSmith/Desktop/GAME/models/sphere_smooth.obj", &shader, "Cube");
	//Mesh* obj3 = new Mesh("C:/Users/AlexSmith/Desktop/GAME/models/sphere_smooth.obj", &shader, "CubeChild");
	//Mesh* obj4 = new Mesh("C:/Users/AlexSmith/Desktop/GAME/models/sphere_smooth.obj", &shader, "CubeChildChild");
	//Mesh* obj5 = new Mesh("C:/Users/AlexSmith/Desktop/GAME/models/sphere_smooth.obj", &shader, "Last");
	MeshHolder* obj2 = new MeshHolder("C:/Users/AlexSmith/Desktop/GAME/models/plane.obj", &shader, "Phys");
	PointLight* obj6 = new PointLight(&emission_shader, "PointLight");
	PointLight* obj7 = new PointLight(&emission_shader, "PointLight");
	obj6->transform.position.x = 2.0;

	
	

	resourceManager.create_mesh("C:\\Users\\AlexSmith\\Desktop\\GAME\\models\\mat\\cube2.obj", &shader);
	obj1->mesh = resourceManager.meshes[0];

	

	obj6->strength = 3.0f;
	obj6->radius = 15.0f;

	selected_p = obj1;
	selected = true;



	//obj1->add_child(obj3);
	//obj3->add_child(obj4);
	//obj3->add_child(obj5);
	world->add_child(obj7);
	world->add_child(obj6);
	world->add_child(obj2);
	world->add_child(obj1);
	obj7->transform.position.y = 2.0;

	//btCollisionShape* boxCollisionShape = new btSphereShape(1.0);
	//btDefaultMotionState* motionstate = new btDefaultMotionState(btTransform(
		//btQuaternion(0.0, 0.0, 0.0, 1.0),
		//btVector3(obj2->model[3].x, obj2->model[3].y, obj2->model[3].z)
	//));
	///


	//btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(
		//1,                  // mass, in kg. 0 -> Static object, will never move.
		//motionstate,
		//boxCollisionShape,  // collision shape of body
		//btVector3(0, 0, 0)    // local inertia
	//);
	//btRigidBody* rigidBody = new btRigidBody(rigidBodyCI);

	//dynamicsWorld->setGravity(btVector3(0, -10, 0));
	//dynamicsWorld->addRigidBody(rigidBody);


	Camera* camera = new Camera();

	world_camera = camera;


	
	std::vector<PointLight*> point_lights;
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







	//icon = load_texture("assets/icons/icon.png");
	//icon2 = load_texture("assets/icons/icon2.png");
	//icon3 = load_texture("assets/icons/icon3.png");

	shader.Use();
	glUniform1i(glGetUniformLocation(shader.ID, "oText"), 0);
	glUniform1i(glGetUniformLocation(shader.ID, "sText"), 1);
	glUniform1i(glGetUniformLocation(shader.ID, "nText"), 2);
	glUniform1i(glGetUniformLocation(shader.ID, "skybox"), 3);

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


	camera->view = glm::translate(camera->view, glm::vec3(0.0f, -0.2f, -3.0f));


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
			camera->position = glm::vec3(l->GetPoint(test).x, l->GetPoint(test).y, l->GetPoint(test).z);
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
			camera->position += direction * speed;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			camera->position -= direction * speed;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			camera->position += right * speed;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			camera->position -= right * speed;
		}

		camera->view = glm::lookAt(
			camera->position,           // Camera is here
			camera->position + direction, // and looks here : at the same position, plus "direction"
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
			ImGuiID dock1 = ImGui::DockBuilderSplitNode(idd, ImGuiDir_Left, 0.5f, nullptr, &idd);
			ImGuiID dock2 = ImGui::DockBuilderSplitNode(dock1, ImGuiDir_Down, 0.75f, nullptr, &dock1);
			ImGuiID dock3 = ImGui::DockBuilderSplitNode(idd, ImGuiDir_Right, 0.7f, nullptr, &idd);
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
				resourceManager.create_mesh(model_path, &shader);
			}

		}
		ImGui::SameLine();
		if (ImGui::Button("Add material")) {

		}

		
		ImGui::BeginChild("Textures", ImVec2(200, 0), ImGuiChildFlags_Border);

		for (Texture* texture : resourceManager.textures) {
			
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
			ImGui::ImageButton(("id##"+std::to_string(texture->id)).c_str(), texture->id, 
				ImVec2(32, 32), ImVec2(0.0f,0.0f), ImVec2(1.0f, 1.0f), ImVec4(0.0, 0.0, 0.0, 1.0), ImVec4(1.0, 1.0, 1.0, 1.0));
			
			ImGui::PopStyleVar();

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
				ImGui::SetDragDropPayload("TEXTURE", texture, sizeof(Texture*));

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

		//inspector
		ImGui::Begin("Inspector", 0);
		if (selected) {
			inspector_draw_attributes(selected_p);


			/*if (selected_p->name != "root") {
				ImGui::SeparatorText("Actions");
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.639f, 0.153f, 0.153f, 1.0f));
				if (ImGui::Button("Delete")) {
					selected_p->remove();
					selected = false;
				}
				ImGui::PopStyleColor();

				if (ImGui::Button("Rename")) {

				}
			}*/
		}

		ImGui::ShowDemoWindow();


		if (selected && selected_p->type == MESH_HOLDER) {
			MeshHolder* obj = static_cast<MeshHolder*>(selected_p);
			static int item_current_id = 0;


			ImGui::SeparatorText("Mesh");
			std::string text = "";

			if (obj->mesh != nullptr) {
				text = "Mesh";
			}
			else {
				text = "Empty";
			}


			ImGui::Button(text.c_str(), { 64, 64 });
			ImGui::SameLine();
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MESH"))
				{
					IM_ASSERT(payload->DataSize == sizeof(int));
					int mesh_i = *(const int*)payload->Data;

					obj->mesh = resourceManager.meshes[mesh_i];

				}
				ImGui::EndDragDropTarget();
			}
					
			

			if (obj->mesh != nullptr) {
				if (ImGui::BeginListBox("##Materials", ImVec2(0, 50))) {
					for (int n = 0; n < obj->mesh->vertexGroups.size(); n++)
					{
						const bool is_selected = (item_current_id == n);
						if (ImGui::Selectable((obj->mesh->vertexGroups[n]->material->name + "##" + std::to_string(n)).c_str(), is_selected)) {
							item_current_id = n;
						}
						if (is_selected) {
							ImGui::SetItemDefaultFocus();
							selected_vertex_group = obj->mesh->vertexGroups[item_current_id];
						}
						
					}
					ImGui::EndListBox();
				}

			}

			if (selected_vertex_group != nullptr) {
				if (selected_vertex_group->material != nullptr) {
					
					Material *material = selected_vertex_group->material;
					VertexGroup *svg = selected_vertex_group;
					ImGui::SeparatorText(material->name.c_str());

					ImGui::SliderFloat("uv x", &(svg->material->uv_scale.x), 0.0, 25.0);
					ImGui::SliderFloat("uv y", &(svg->material->uv_scale.y), 0.0, 25.0);

					ImGui::BeginGroup();
					ImGui::Text("Diffuse");
					ImGui::Checkbox("Enabled", &(svg->material->use_diffuse)); ImGui::SameLine();
					ImGui::Checkbox("Emit", &(svg->material->emit)); ImGui::SameLine();

					float col[3] = { svg->material->diffuse_color.r, svg->material->diffuse_color.g, svg->material->diffuse_color.b };
					ImGui::SetColorEditOptions(ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_Float);
					ImGui::ColorEdit3("Color", col);
					svg->material->diffuse_color = glm::vec3(col[0], col[1], col[2]);

					ImGui::SliderFloat("##diff", &(svg->material->diffuse_value), 0.0, 1.0);

					ImGui::EndGroup();

				}
				else {
					ImGui::Button("Create new material");
				}
			}

			//
			//ImGui::Image(obj->mesh->vertexGroups[0]->material->diffuse_texture->id, ImVec2(64, 64));

			//if (ImGui::BeginDragDropTarget()) {
			//	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE")) {
			//		assert(payload->DataSize == sizeof(Texture*));

			//		//Texture* texture = (Texture*)(payload->Data);
			//		//Texture tex = *(Texture*)(payload->Data);
			//		//problems with addres

			//		//mod->material->diffuse_texture = texture;

			//	}
			//	ImGui::EndDragDropTarget();
			//}


			

			//ImGui::Image(mod->material->specular_texture->id, ImVec2(64, 64));
			//ImGui::SameLine();
			//ImGui::BeginGroup();
			//ImGui::Text("Specular");
			//ImGui::Checkbox("On##s", &(mod->material->use_specular));
			//ImGui::SliderFloat("##spect", &(mod->material->specular_value), 0.0, 5.0);

			//ImGui::EndGroup();

			//ImGui::Image(mod->material->normalmap_texture->id, ImVec2(64, 64));
			//ImGui::SameLine();
			//ImGui::BeginGroup();
			//ImGui::Text("Normal");
			//ImGui::Checkbox("On##n", &(mod->material->use_normalmap));
			//ImGui::SliderFloat("##normal", &(mod->material->normalmap_value), 0.0, 5.0);

			//ImGui::EndGroup();

			//ImGui::SeparatorText("Material vars");
			//ImGui::SliderFloat("SkyReflection", &(mod->material->reflection_value), 0.0, 1.0);
			//ImGui::SliderFloat("RefRatio", &(mod->material->refract_value), 0.0, 1.0);
			//ImGui::SliderFloat("Shine", &(mod->material->shine_value), 0.01, 64.0);
			
		
			/*static int e = 0;
			ImGui::RadioButton("Phong", &e, 0); ImGui::SameLine();
			ImGui::RadioButton("Blinn", &e, 1);*/

			//mod->material->use_blinn = e;
		}

		if (selected && selected_p->type == CURVE) { //3 = curve
			Bezier* obj = static_cast<Bezier*>(selected_p);

			ImGui::InputFloat("X", &(obj->p1.x), 0.05f);
			ImGui::InputFloat("Y", &(obj->p1.y), 0.05f);
			ImGui::InputFloat("Z", &(obj->p1.z), 0.05f);
			ImGui::Separator();
			ImGui::InputFloat("X1", &(obj->p2.x), 0.05f);
			ImGui::InputFloat("Y1", &(obj->p2.y), 0.05f);
			ImGui::InputFloat("Z1", &(obj->p2.z), 0.05f);
			ImGui::Separator();
			ImGui::InputFloat("X2", &(obj->p3.x), 0.05f);
			ImGui::InputFloat("Y2", &(obj->p3.y), 0.05f);
			ImGui::InputFloat("Z2", &(obj->p3.z), 0.05f);
			
		}

		

		ImGui::End();


		//other 
		ImGui::Begin("Other", 0, ImGuiWindowFlags_AlwaysAutoResize);

		ImGui::SeparatorText("Add");

		if (ImGui::Button("Add .obj model")) {
			nfdu8char_t* outPath = NULL;

			nfdfilteritem_t filterItem[1] = { { "Models", "obj" } };
			nfdresult_t result = NFD_OpenDialogU8(&outPath, filterItem, 1, NULL);

			if (result == NFD_OKAY && selected) {
				std::filesystem::path new_path = std::filesystem::u8path(outPath);
				//MeshHolder* obj_ptr = new MeshHolder(new_path.generic_string(), &shader, std::filesystem::path(outPath).filename().stem().string());

				//selected_p->add_child(obj_ptr);
				
				

				free(outPath);
			}
			else if (result == NFD_CANCEL) {
				puts("User pressed cancel.");
			}
			else {
				printf("Error: %s\n", NFD_GetError());
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Add point light")) {
			PointLight* ob = new PointLight(&emission_shader, "PointLight");
			
			if (selected) {
				selected_p->add_child(ob);
			}

		}


		
		ImGui::SeparatorText("Scene");
		ImGui::Text("Drag and drop .scene file to load it");
		ImGui::Text("Name");
		ImGui::SameLine();
		ImGui::PushItemWidth(64.0f);
		ImGui::InputText("##SceneName", &save_scene_as); 
		ImGui::PopItemWidth();
		ImGui::SameLine();
		if (ImGui::Button("Save")) {
			save_scene(&objects, save_scene_as, camera);
		}



		ImGui::SeparatorText("Settings");
		ImGui::Text("Skybox Light");
		ImGui::Text("FOV");
		ImGui::SliderFloat("##FOV", &fov, 1.0f, 179.0f);
		ImGui::Checkbox("Show wireframe", &show_wireframe_if_selected);
		ImGui::Checkbox("Show lines", &show_lines);
		ImGui::Checkbox("Show lights", &show_lights);
		ImGui::Checkbox("Update Physics", &update_physics);

		ImGui::Separator();



		if (selected) {
			ImGui::SeparatorText("Script");
			ImGui::Text(selected_p->script.c_str());
			ImGui::InputTextMultiline("##script", &script);

			if (ImGui::Button("Apply")) {
				selected_p->script = script;
			}
		}
		
		ImGui::End();

		
		
		//proj mat
		glm::mat4 projection(1.0f);

		projection = glm::perspective(fov / 180.0f * 3.14f, aspectRatio, 0.1f, 1000.0f);


		


		ImGui::Begin("rays");

		double xpos = 0;
		double ypos = 0;

		glfwGetCursorPos(window, &xpos, &ypos);



		

		glm::vec3 worldPos = glm::unProject(glm::vec3(xpos, window_height-ypos, 1.0),
			camera->view, projection,
			glm::vec4(0, 0, window_width, window_height));
		glm::vec3 rayMouse = glm::normalize(worldPos - camera->position);

		ImGui::Text("world pos %f", worldPos.x);
		ImGui::Text("ray_mouse %f,   %f,    %f", rayMouse.x, rayMouse.y, rayMouse.z);

		ImGui::End();


		//btCollisionWorld::ClosestRayResultCallback closestResults(from, to);
		//dynamicsWorld->rayTest(camera->position, camera->position*100);

		world->children[0]->transform.position = camera->position + rayMouse * 4.0f;



	
		glm::mat4 view2 = glm::mat4(glm::mat3(camera->view));
		glDepthMask(GL_FALSE);
		sky_shader.Use();
		sky_shader.setMat4("projection", projection);
		sky_shader.setMat4("view", view2);
		sky_shader.setFloat("value", 1.0);


		glBindVertexArray(SKY_VAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(GL_TRUE);


		

		//emission
		emission_shader.Use();
		emission_shader.setMat4("projection", projection);
		emission_shader.setMat4("view", camera->view);
		//emission_shader.setVec3("color", glm::vec3(ambient_color[0], ambient_color[1], ambient_color[2]));

		// shader standart
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

		shader.Use();
		shader.setVec3("lightColor", glm::vec3(ambient_color[0], ambient_color[1], ambient_color[2]));
		shader.setVec3("viewPos", camera->position);
		shader.setMat4("projection", projection); 
		shader.setMat4("view", camera->view);
		shader.setFloat("sky_val", 1.0);
		
		//test
		if (point_lights.size() > point_lights_max_size) {
			throw;
		}
		
		
		int point_lights_size = 0;
		int n = 0;
		for (BaseObject* obj : treeObjects) {
			if (obj->type == POINT_LIGHT) {
				PointLight* light = static_cast<PointLight*>(obj);


;				light->visible = show_lights;
				shader.setVec3("pointLights[" + std::to_string(n) + "].position", light->transform.position);
				shader.setVec3("pointLights[" + std::to_string(n) + "].color", light->color);
				shader.setFloat("pointLights[" + std::to_string(n) + "].strength", light->strength);
				shader.setFloat("pointLights[" + std::to_string(n) + "].radius", light->radius);
				n += 1;
				point_lights_size += 1;
			}
		}
		shader.setInt("light_number", point_lights_size);


		ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
		if (ImGui::BeginMainMenuBar())
		{
			ImGui::Text("Project Name:");
			std::string filename = project_path.filename().string();
			ImGui::Text(filename.c_str());
			ImGui::Text("Light Number %i / %i", point_lights_size, point_lights_max_size);



			ImGui::EndMainMenuBar();
		}
		ImGui::PopStyleColor();

		
		screen_shader.Use();
		ImGui::Begin("test");
		


		ImGui::SliderFloat("size", &size, 0.00f, 0.1f);
		ImGui::SliderFloat("quality", &quality, 0.0f, 15.0f);
		ImGui::SliderFloat("directions", &directions, 0.0f, 360.0f);

		screen_shader.setFloat("Size", size);
		screen_shader.setFloat("Quality", quality);
		screen_shader.setFloat("Directions", directions);

		ImGui::End();


		for (Line* line : lines) {
			line->visible = show_lines;
			line->draw();
			
		}




		test = 0.5 * (1+sin(glfwGetTime()/5.0));


		//obj1->model = glm::mat4(1.0);
		//obj1->model = glm::translate(obj1->model, glm::vec3(l->GetPoint(test).x, l->GetPoint(test).y, l->GetPoint(test).z)); //rem
		//obj1->model = glm::scale(obj1->model, glm::vec3(0.1, 0.1, 0.1));

		//if (update_physics) {
			//dynamicsWorld->stepSimulation(1.0f / 60.0f, 10);
		//}
		
		//btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[0];
		//btRigidBody * body = btRigidBody::upcast(obj);
		//btTransform trans;
		//if (body && body->getMotionState())
		//{
			//body->getMotionState()->getWorldTransform(trans);
		//}
		//else
		//{
			//trans = obj->getWorldTransform();
		//}
		//body->applyCentralForce(btVector3(0.1, 0.0, 0.0));

		//obj2->model[3][0] = trans.getOrigin().getX();
		//obj2->model[3][1] = trans.getOrigin().getY();
		//obj2->model[3][2] = trans.getOrigin().getZ();

		//obj2->rotation.x = trans.getRotation().getX();
		//obj2->rotation.y = trans.getRotation().getY();
		//obj2->rotation.z = trans.getRotation().getZ();

	

		//obj = dynamicsWorld->getCollisionObjectArray()[2];
		//body = btRigidBody::upcast(obj);
		//trans;
		//if (body && body->getMotionState())
		//{
	//		body->getMotionState()->getWorldTransform(trans);
		//}
		///else
		//{
		//	trans = obj->getWorldTransform();
		//}
		//body->applyCentralForce(btVector3(0.1, 0.0, 0.0));

		//obj4->model[3][0] = trans.getOrigin().getX();
		//obj4->model[3][1] = trans.getOrigin().getY();
		//obj4->model[3][2] = trans.getOrigin().getZ();


		
		//obj4->rotation.x = trans.getRotation().getX();
		//obj4->rotation.y = trans.getRotation().getY();
		//obj4->rotation.z = trans.getRotation().getZ();

		
		
		

		//simpleDepthShader.Use();
		//simpleDepthShader.setMat4("projection", projection);
		//simpleDepthShader.setMat4("view", camera->view);
		glViewport(0, 0, window_width, window_height);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_FRAMEBUFFER_SRGB);

		for (BaseObject* obj : treeObjects) {



			if (selected_p == obj && selected == true && show_wireframe_if_selected == true) {
				obj->draw();
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				shader.setFloat("emission", 0.7f);
				obj->transform.scale += glm::vec3(0.03f, 0.03f, 0.03f);
				obj->draw();
				obj->transform.scale -= glm::vec3(0.03f, 0.03f, 0.03f);
				//glEnable(GL_DEPTH_TEST);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				shader.setFloat("emission", 0.0);
			}
			else {
				obj->draw();
			}



			obj->lua["time"] = glfwGetTime();
			obj->update();

			for (BaseObject* child : obj->children) {
				obj->draw();
				obj->lua["time"] = glfwGetTime();
				obj->update();
			}



		}
		//glDisable(GL_FRAMEBUFFER_SRGB);
		//glViewport(0, 0, window_width, window_height);

		//glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
		//glClearColor(1.0f, 1.0f, 0.5f, 1.0f);
		//glDisable(GL_DEPTH_TEST);
		//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT);




	
		//screen_shader.Use();
		//glBindVertexArray(quadVAO);
		//glDisable(GL_DEPTH_TEST);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
		//glDrawArrays(GL_TRIANGLES, 0, 6);


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

	//glViewport(0, 0, width, height);
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


void save_scene(std::vector<BaseObject*>* tree, std::string name, Camera* camera) {
	std::ofstream file;


	if (!name.empty()) {
		file.open(name + ".scene");

		json j;

		j["fov"] = fov;	
		j["camera_pos"] = {camera->position.x, camera->position.y, camera->position.z};
		j["hor_ang"] = horizontalAngle;
		j["ver_ang"] = verticalAngle;


		int n = 0;
		for (BaseObject* object : (*tree)) {
			j["objects"][n]["name"] = object->name;
			j["objects"][n]["type"] = object->type;
			/*j["objects"][n]["position"] = { object->model[3].x, object->model[3].y, object->model[3].z };*/

			if (object->type == 1) { //light 
				PointLight* obj = static_cast<PointLight*>(object);


				j["objects"][n]["color"] = {obj->color.r, obj->color.g, obj->color.b};
				j["objects"][n]["radius"] = obj->radius;
				j["objects"][n]["strength"] = obj->strength;
			}

			if (object->type == 0) {
				//Mesh* obj = static_cast<Mesh*>(object);
				//j["objects"][n]["path"] = obj->rel_path;

				//for (int i = 0; i < obj->models.size(); i++) {
					//j["objects"][n]["materials"][i]["texture_paths"][0] = obj->models[i]->texturesPath.diffuse_path;
					//j["objects"][n]["materials"][i]["texture_paths"][1] = obj->models[i]->texturesPath.specular_path;
					//j["objects"][n]["materials"][i]["texture_paths"][2] = obj->models[i]->texturesPath.normal_path;
					//j["objects"][n]["materials"][i]["use_texture"]["diffuse"] = obj->models[i]->material.use_diffuse;
					//j["objects"][n]["materials"][i]["use_texture"]["specular"] = obj->models[i]->material.use_specular;
					//j["objects"][n]["materials"][i]["use_texture"]["normal"] = obj->models[i]->material.use_normal;
				//}
			}


			n += 1;
		}

		file << j << std::endl;

		file.close();
	}
	

}

void load_scene(std::string path) {
	std::ifstream file(path);
	json data = json::parse(file);

	fov = data["fov"];
	horizontalAngle = data["hor_ang"];
	verticalAngle = data["ver_ang"];
	world_camera->position = glm::vec3(data["camera_pos"][0], data["camera_pos"][1], data["camera_pos"][2]);

	for (auto& elem : data["objects"]) {
		//std::cout << elem["name"];

		int type = elem["type"];
		//std::cout << type;

		if (type == 1) {
			glm::vec3 color(elem["color"][0], elem["color"][1], elem["color"][2]);

	
			PointLight* ob = new PointLight(emission_shader_ptr, "PointLight", color, elem["strength"]);
			/*ob->model[3] = glm::vec4(elem["position"][0], elem["position"][1], elem["position"][2], 1.0);*/
			ob->radius = elem["radius"];
			objects.push_back(ob);
		}

		if (type == 0) {
			//Mesh* obj = new Mesh(elem["path"], shader_ptr, elem["name"]);
			//objects.push_back(obj);
			int n = 0;
			for (auto& elem2 : elem["materials"]) {
				//obj->models[n]->textures.diffuse = obj->models[n]->load_tex(elem2["texture_paths"][0], 0);
				//obj->models[n]->textures.specular = obj->models[n]->load_tex(elem2["texture_paths"][1], 1);
				//obj->models[n]->textures.normal = obj->models[n]->load_tex(elem2["texture_paths"][2], 2);

				//obj->models[n]->material.use_diffuse = elem2["use_texture"]["diffuse"];
				//obj->models[n]->textures.specular = elem2["use_texture"]["specular"];
				//obj->models[n]->textures.normal = elem2["use_texture"]["normal"];

				n += 1;
			}
		}
	}
}





void drop_callback(GLFWwindow* window, int count, const char** paths)
{
	
	int i = 0;

	for (i = 0; i < count; i++) {
		std::filesystem::path path = paths[i];
		if (path.extension() == ".scene") {
			
			

			for (BaseObject* object : objects) {
				delete object;
			}
			objects.clear();
			selected = false;

			load_scene(path.generic_string());

		}
	}
}
