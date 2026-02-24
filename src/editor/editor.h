#pragma once

#include <math.h>


#include "../engine/asset_repository.h"
#include "../engine/loaders/image_loader.h"
#include "../engine/shader.h"
#include "../engine/graphics.h"
#include "../engine/window.h"
#include "../engine/lua_runner.h"
#include "../engine/logger.h"
#include "../engine/game_config.h"
#include "../engine/canvas.h"
#include "../engine/line.h"
#include "../engine/game_object_types.h"
#include "../engine/game_objects/model.h"
#include "../engine/game_objects/camera.h"
#include "../engine/game_objects/node.h"
#include "../engine/loaders/drish_loader.h"
#include "../engine/loaders/model_loader.h"
#include "../engine/filedialogs.h"

#include "executor.h"

#include "user_interface/log_window.h"
#include "user_interface/assets_window.h"
#include "user_interface/script_window.h"
#include "user_interface/properties_window.h"

#include "../engine/resource.h"

#include "../version.h"

class Editor {
public:
	drishengine::Window *window = nullptr;

	Node* world = nullptr;
	LuaRunner* luaRunner = nullptr;
	AssetRepository* assetRepository = nullptr;
	NodeRepository* nodeRepository = nullptr;
	Graphics* graphics = nullptr;


	Camera* camera = nullptr;
	Camera* oldCamera = nullptr;

	float horizontalAngle = 0;
	float verticalAngle = 0;



	std::filesystem::path drishPath;

	Node* selectedNode = nullptr;
	Node* selectedTreeNode = nullptr;

	float timeSinceLastSave = 0;

	Texture* icoTexture = nullptr;



	GameConfig gameConfig;

	bool createPopupPopened = false;
	bool startPopupPopened = true;

	bool previewCamera = false;

	Canvas* canvas;


	LogWindow logWindow;

	AssetWindow* assetWindow = nullptr;
	ScriptWindow* scriptWindow = nullptr;
	PropertiesWindow* propertiesWindow = nullptr;


	Editor(drishengine::Window *window,
		AssetRepository *assetRepository,
		NodeRepository *nodeRepository,
		LuaRunner* luaRunner) {
		this->window = window;
		this->luaRunner = luaRunner;

		this->assetRepository = assetRepository;
		this->nodeRepository = nodeRepository;

		this->assetWindow = new AssetWindow(assetRepository);
		this->assetWindow->drishPath = &drishPath;
		this->scriptWindow = new ScriptWindow(assetRepository, luaRunner);
		this->propertiesWindow = new PropertiesWindow(assetRepository, luaRunner, &selectedNode);

		this->canvas = new Canvas();

		logWarning("Drish;Engine is not drish enough");
		
		//test

		HMODULE hModule = GetModuleHandle(NULL); 
		HRSRC hResource = FindResource(hModule, MAKEINTRESOURCE(IDB_PNG1), RT_RCDATA);
		HGLOBAL hMemory = LoadResource(hModule, hResource);
		DWORD dwSize = SizeofResource(hModule, hResource);
		LPVOID lpAddress = LockResource(hMemory);

		unsigned char* bytes = new unsigned char[dwSize];
		memcpy(bytes, lpAddress, dwSize);

		Texture* texture = new Texture(-1);
		icoTexture = texture;
		ImageLoader::loadImage(bytes, (int) dwSize, texture);


		
		HRSRC hResource2 = FindResource(hModule, MAKEINTRESOURCE(DR_FONT), RT_RCDATA);
		HGLOBAL hMemory2 = LoadResource(hModule, hResource2);
		DWORD dwSize2 = SizeofResource(hModule, hResource2);
		LPVOID lpAddress2 = LockResource(hMemory2);

		unsigned char* bytes2 = new unsigned char[dwSize2];
		memcpy(bytes2, lpAddress2, dwSize2);
		



		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui_ImplGlfw_InitForOpenGL(window->getWindow(), true);
		ImGui_ImplOpenGL3_Init();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.IniFilename = NULL;

		ImGuiStyle& style = ImGui::GetStyle();

		//ImFont* font_body = io.Fonts->AddFontFromFileTTF("AdwaitaMonoNerdFont-Regular.ttf", 17.0f, NULL, io.Fonts->GetGlyphRangesDefault());
		ImFont* font_body = io.Fonts->AddFontFromMemoryTTF(bytes2, (int)dwSize2, 17.0);
		style.Alpha = 1.0f;
		style.DockingSeparatorSize = 1.0f;
		style.WindowBorderSize = 1.0f;

		style.FramePadding = ImVec2(15, 2);
		style.Colors[ImGuiCol_ModalWindowDimBg] = ImColor(0.0f, 0.0f, 0.0f, 0.5f);


		
		//ImageLoader::loadImage(std::filesystem::path("icon.png"), texture);

		logInfo("Editor started");
	}

	void save() {
		timeSinceLastSave = 0;

		logInfo("Save");
		logDebug("Version: ", DRISH_ENGINE_VERSION);


		nlohmann::json j;
		std::vector<Node*> nodes;
		world->getAllChildNodes(world, &nodes);
		for (Node* n : nodes) {
			switch (n->type)
			{
			case Type::MODEL:
				j["nodes"].push_back(*static_cast<Model*>(n));
				break;
			case Type::CAMERA:
				j["nodes"].push_back(*static_cast<Camera*>(n));
				break;
			case Type::POINT_LIGHT:
				j["nodes"].push_back(*static_cast<PointLight*>(n));
				break;
			default:
				j["nodes"].push_back(*n);
				break;
			}
			
		}

		j["version"] = DRISH_ENGINE_VERSION;
		j["scripts"] = {};
		j["textures"] = {};
		j["vertices"] = {};
		j["gameConfig"] = gameConfig;

		for (auto const& pair : assetRepository->scriptsMap) {
			j["scripts"].push_back(*pair.second);
		}
		for (auto const& pair : assetRepository->texturesMap) {
			j["textures"].push_back(*pair.second);
		}
		for (auto const& pair : assetRepository->verticesMap) {
			j["vertices"].push_back(*pair.second);
		}
		for (auto const& pair : assetRepository->materialsMap) {
			if (pair.second->name != "default") {
				j["materials"].push_back(*pair.second);
			}
		}

		j["nextNodeId"] = nextNodeId;
		j["nextAssetId"] = nextAssetId;

		logDebug("Next node id: ", nextNodeId);
		
		std::ofstream file(drishPath, std::ofstream::trunc);
		if (file.is_open()) {
			file << std::setw(4) << j << std::endl;
		}

		logDebug("Save success");
	}

	void showStartPopup() {
		if (startPopupPopened) {
			ImGui::OpenPopup("Start");
		}
		if (ImGui::BeginPopupModal("Start", (bool*)0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar)) {
			ImGui::Image(icoTexture->glid, ImVec2(256, 256));

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5, 0.5, 0.5, 1.0));
			ImGui::Text(DRISH_ENGINE_VERSION);
			ImGui::PopStyleColor();

			if (ImGui::Button("Load .drish project", ImVec2(-1.0f, 0.0f))) {
				drishPath = drishengine::openDrishOpenFileDialog();
				
				if (drishPath.empty()) {} 
				else {
					DrishLoader::load(drishPath, world, assetRepository, &gameConfig, nodeRepository);

					startPopupPopened = false;
					ImGui::CloseCurrentPopup();
				}
			};
			
			if (ImGui::Button("Create new project", ImVec2(-1.0f, 0.0f))) {
				drishPath = drishengine::openDrishSaveDialog();

				if (drishPath.empty()) {}
				else {
					this->save();
					
					startPopupPopened = false;
					ImGui::CloseCurrentPopup();
				}
			}

			
			ImGui::EndPopup();
		}
	}

	void loop(double delta) {
		timeSinceLastSave += delta;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGuiID viewportDockedID = ImGui::DockSpaceOverViewport(0, viewport, ImGuiDockNodeFlags_PassthruCentralNode);


		static bool docked = false;
		if (!docked) {
			ImVec2 workCenter = ImGui::GetMainViewport()->GetWorkCenter();
			ImGuiID id = ImGui::GetID("Drish Engine");
			ImGui::DockBuilderRemoveNode(id);
			ImGui::DockBuilderAddNode(id);
			ImVec2 nodePosition{ 0, 0 };
			ImGui::DockBuilderSetNodeSize(id, ImVec2(window->width, window->height));
			ImGui::DockBuilderSetNodePos(id, nodePosition);
			ImGuiID treeDock = ImGui::DockBuilderSplitNode(viewportDockedID, ImGuiDir_Left, 0.5f, nullptr, &viewportDockedID);
			ImGuiID inspectorDock = ImGui::DockBuilderSplitNode(treeDock, ImGuiDir_Down, 0.75f, nullptr, &treeDock);
			ImGuiID mainDock;
			ImGuiID rightDock = ImGui::DockBuilderSplitNode(viewportDockedID, ImGuiDir_Right, 0.3f, nullptr, &mainDock);
			ImGuiID viewportDock;
			ImGuiID logDock = ImGui::DockBuilderSplitNode(mainDock, ImGuiDir_Down, 0.4f, nullptr, &viewportDock);
			ImGuiID configDock;
			ImGuiID scriptEditorDock = ImGui::DockBuilderSplitNode(rightDock, ImGuiDir_Up, 0.6f, nullptr, &configDock);
			
			ImGui::DockBuilderDockWindow("Tree", treeDock);
			ImGui::DockBuilderDockWindow("Properties", inspectorDock);
			ImGui::DockBuilderDockWindow("Log", logDock);
			ImGui::DockBuilderDockWindow("Script editor", scriptEditorDock);
			ImGui::DockBuilderDockWindow("Viewport", viewportDock);
			ImGui::DockBuilderDockWindow("Game config", configDock);
			ImGui::DockBuilderFinish(id);
			docked = true;
		}

		ImGuiKeyChord chord = ImGuiMod_Ctrl | ImGuiKey_S;
		bool isRouted = ImGui::GetShortcutRoutingData(chord)->RoutingCurr != ImGuiKeyOwner_NoOwner;
		if (!isRouted && ImGui::IsKeyChordPressed(chord)) {
			this->save();
		}

		
		showStartPopup();

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Save project", ImGui::GetKeyChordName(chord))) {
					this->save();
				}
				if (ImGui::MenuItem("Close", "Alt+F4")) {
					glfwSetWindowShouldClose(window->getWindow(), true);
				}
				ImGui::EndMenu();
			}
			//if (ImGui::BeginMenu("Edit"))
			//{
			//	//if (ImGui::MenuItem("Undo", "Ctrl+Z")) {}
			//	//if (ImGui::MenuItem("Redo", "Ctrl+Y", false, false)) {} // Disabled item
			//	//ImGui::Separator();
			//	//if (ImGui::MenuItem("Cut", "Ctrl+X")) {}
			//	//if (ImGui::MenuItem("Copy", "Ctrl+C")) {}
			//	//if (ImGui::MenuItem("Paste", "Ctrl+V")) {}
			//	ImGui::EndMenu();
			//}
			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::MenuItem("Log")) { logWindow.open = true; }
				if (ImGui::MenuItem("Assets")) { assetWindow->open = true; }
				if (ImGui::MenuItem("Script Editor")) { scriptWindow->open = true; }
				if (ImGui::MenuItem("Properties")) { propertiesWindow->open = true; }
				ImGui::EndMenu();
			}
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8, 0.5, 0.5, 1.0));
			ImGui::Text("%i", (int) timeSinceLastSave);
			ImGui::PopStyleColor();
			

			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar;

			if (ImGui::BeginViewportSideBar("#top", viewport, ImGuiDir_Up, 32, window_flags)) {
				if (ImGui::BeginMenuBar()) {
					if (ImGui::Button("Run")) {
						this->save();
						drishexecutor::runGame(window, "game.exe", drishPath.parent_path());
					}
					if (ImGui::Button("Build")) {

					}
					ImGui::EndMenuBar();
				}
				ImGui::End();
			}

			if (ImGui::BeginViewportSideBar("#status", viewport, ImGuiDir_Down, 16, window_flags)) {
				if (ImGui::BeginMenuBar()) {
					ImGui::Text("Status");
					ImGui::EndMenuBar();
				}
				ImGui::End();
			}




			ImGui::EndMainMenuBar();
		}
		
		

		

		
		

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Viewport");
		canvas->resize(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);

		//
		ImGui::SetCursorPos(ImVec2(0, 0));
		
		ImGui::Image(canvas->texture, ImGui::GetWindowSize(), ImVec2(0, 0), ImVec2(1, 1));
		ImGui::PopStyleVar();
		ImVec2 mousePos = ImGui::GetMousePos();

		ImVec2 viewportPos = ImGui::GetItemRectMin();
		ImVec2 viewportSize = ImGui::GetItemRectSize();
		ImVec2 viewportCenter = ImVec2(viewportSize.x / 2.0, viewportSize.y / 2.0);
		ImVec2 absoluteViewportCenter = ImVec2(viewportCenter.x + viewportPos.x,
			viewportCenter.y + viewportPos.y);
		double xpos = 0;
		double ypos = 0;

		static int oldWidth = 0;
		static int oldHeight = 0;
		static int newWidth = 0;
		static int newHeight = 0;

		newWidth = viewportSize.x;
		newHeight = viewportSize.y;

		if (newWidth != oldWidth || newHeight != oldHeight) {
			oldWidth = newWidth;
			oldHeight = newHeight;
			canvas->resize(newWidth, newHeight);
		}

		camera->setWindowAspectRatio(canvas->width, canvas->height);
		static bool captured = false;
		if (glfwGetMouseButton(window->getWindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
			captured = false;
		}
		//camera movement
		if (oldCamera == nullptr && 
			(ImGui::IsItemHovered() || captured) &&
			glfwGetMouseButton(window->getWindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS ) {
			if (captured == true) {
				glfwGetCursorPos(window->getWindow(), &xpos, &ypos);
			}
			else {
				xpos = absoluteViewportCenter.x;
				ypos = absoluteViewportCenter.y;
			}
			captured = true;

			glfwSetCursorPos(window->getWindow(),
				absoluteViewportCenter.x,
				absoluteViewportCenter.y);

			
			double dx = (int)(xpos - absoluteViewportCenter.x);
			double dy = (int)(ypos - absoluteViewportCenter.y);

			double mouseSpeed = 2.5 * delta;

			horizontalAngle += delta * dx;
			verticalAngle -= delta * dy;

			verticalAngle = glm::clamp(verticalAngle, -glm::half_pi<float>() + 0.05f, glm::half_pi<float>() - 0.05f);

			glm::vec3 direction(
				cos(verticalAngle)* sin(horizontalAngle),
				sin(verticalAngle),
				cos(verticalAngle)* cos(horizontalAngle)
			);

			direction = glm::normalize(direction);


			glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0, 1, 0), direction));

			

			glm::vec3 up = glm::cross(right, direction);



			glm::vec3 ok = glm::vec3(direction.x, 0, direction.z);
			ok = glm::normalize(ok);

			float yaw = std::atan2(ok.x, ok.z);

			glm::vec3 forward;
			forward.x = cos(verticalAngle) * sin(horizontalAngle);
			forward.y = sin(verticalAngle);
			forward.z = cos(verticalAngle) * cos(horizontalAngle);
			forward = glm::normalize(forward);
			//camera->transform.rotation.y = direction.y;
			
			
			camera->view = glm::lookAt(camera->transform.position,
				camera->transform.position + direction, up);

			float speed = 5.0 * delta;

			if (glfwGetKey(window->getWindow(), GLFW_KEY_W) == GLFW_PRESS) {
				camera->transform.position += direction * speed;
			}
			if (glfwGetKey(window->getWindow(), GLFW_KEY_S) == GLFW_PRESS) {
				camera->transform.position -= direction * speed;
			}
			if (glfwGetKey(window->getWindow(), GLFW_KEY_D) == GLFW_PRESS) {
				camera->transform.position += right * speed;
			}
			if (glfwGetKey(window->getWindow(), GLFW_KEY_A) == GLFW_PRESS) {
				camera->transform.position -= right * speed;
			}
		}

	

		ImGui::End();


		ImGui::Begin("Info");
		if (ImGui::Button("Test all script nodes")) {
			luaRunner->updateNodesScriptEnvironment(world);
		}
		if (selectedNode != nullptr) {
			if (selectedNode->type == Type::CAMERA) {
				ImGui::Text("%i", camera);			
				if (ImGui::Checkbox("Camera Preview", &previewCamera)) {
					if (previewCamera) {
						oldCamera = camera;
						camera = static_cast<Camera*>(selectedNode);
					}
					else {
						camera = oldCamera;
						oldCamera = nullptr;
					};
				}
			}
			else {
				if (oldCamera != nullptr) {
					camera = oldCamera;
					oldCamera = nullptr;
					previewCamera = false;
				}
			}
		}
		ImGui::Text("Delta %f", delta);
		ImGui::End();

		ImGui::Begin("Tree");
		drawTree(world);
		ImGui::End();
		
		if (createPopupPopened) {
			ImGui::OpenPopup("Create Node");
		}
		if (ImGui::BeginPopupModal("Create Node", &createPopupPopened, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("Create node");

			if (ImGui::Button("Node")) {
				Node* n = NodeCreator::createNode(Type::BASE, "node", nodeRepository);
				selectedTreeNode->appendChild(n);

				createPopupPopened = false;
				ImGui::CloseCurrentPopup();
			};
			ImGui::SetItemTooltip("Simple node");

			if (ImGui::Button("Model")) {
				Node* n = NodeCreator::createNode(Type::MODEL, "model", nodeRepository);
				selectedTreeNode->appendChild(n);
				static_cast<Model*>(n)->material = assetRepository->defaultMaterial;

				createPopupPopened = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SetItemTooltip("3d model");

			if (ImGui::Button("Camera")) {
				Node* n = NodeCreator::createNode(Type::CAMERA, "camera", nodeRepository);
				selectedTreeNode->appendChild(n);

				createPopupPopened = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SetItemTooltip("Camera");

			if (ImGui::Button("PointLight")) {
				Node* n = NodeCreator::createNode(Type::POINT_LIGHT, "p_light", nodeRepository);
				selectedTreeNode->appendChild(n);

				createPopupPopened = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SetItemTooltip("Light");

			if (ImGui::Button("Close")) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		assetWindow->draw();
		scriptWindow->draw();
		logWindow.draw();
		propertiesWindow->draw();


		ImGui::Begin("Game config");

		ImGui::InputInt("Width", &gameConfig.width);
		ImGui::InputInt("Height", &gameConfig.height);
		ImGui::InputText("Game name", &gameConfig.title);
		ImGui::Checkbox("Fullscreen", &gameConfig.useFullscreen);
		ImGui::End();

		ImGui::ShowDemoWindow();
	}
	void drawTree(Node* node) {
		

		ImGuiTreeNodeFlags flags;
		flags = ImGuiTreeNodeFlags_DrawLinesToNodes |
			ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
			ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_DefaultOpen;

		if (selectedNode == node) {
			flags |= ImGuiTreeNodeFlags_Selected;
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0, 1.0, 0.9, 1.0));
		}

		if (node->children.empty()) {
			flags |= ImGuiTreeNodeFlags_Leaf;
		}
		ImGui::PushID(node->id);
		/*switch (node->type) {
		case MODEL:
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 96);
			ImGui::SetNextItemAllowOverlap();
			ImGui::Image(modelIcon->glid, ImVec2(16, 16));
			break;

		default:
			break;
		}
		
		ImGui::SameLine();*/
		ImGui::SetNextItemAllowOverlap();
		bool nodeOpen = ImGui::TreeNodeEx(node->name.c_str(), flags);
		
		if (ImGui::BeginDragDropSource()) {
			ImGui::SetDragDropPayload("NODE_MOVE", static_cast<void*>(node), sizeof(Node*));
			/*ImGui::Text(node->name.c_str());
			ImGui::Text("Dragging object %p", static_cast<void*>(node));*/
			ImGui::EndDragDropSource();
		}
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NODE_MOVE")) {
				/*Node* payload_node = (Node*)payload->Data;
				std::cout << (Node*)payload->Data << std::endl;
				std::cout << static_cast<Node*>(payload->Data);
				std::cout << payload->Data << std::endl;
				std::cout << payload_node->name;*/
				
			}
			ImGui::EndDragDropTarget();
			
		}

		if (selectedNode == node) {
			ImGui::PopStyleColor();
		}
		if (ImGui::IsItemClicked()) {
			if (selectedNode == node) {
				selectedNode = node;
			}
			else {
				selectedNode = node;
			}
		}

		if (ImGui::BeginPopupContextItem())
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0, 0.7, 0.7, 1.0));
			ImGui::Text(node->name.c_str());
			ImGui::PopStyleColor();

			selectedTreeNode = node;


			if (ImGui::MenuItem("Create child node")) {
				ImGui::OpenPopup("Create Node");
				createPopupPopened = true;
				logDebug("[EDITOR] ", "createPopupPopened: ", createPopupPopened);
			}

			if (node->name != "World") {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.0, 0.0, 1.0));
				if (ImGui::MenuItem("Delete")) { 
					node->destroy(); 
					selectedNode = nullptr;
				}
				ImGui::PopStyleColor();

			}

			ImGui::EndPopup();
		}

		if (nodeOpen) {
			for (Node* node : node->children) {
				drawTree(node);
			}

			ImGui::TreePop();

		}
		ImGui::PopID();
	};

	
};