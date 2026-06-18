#pragma once


#include "imgui_docking/imgui.h"

#include "editor_window_base.h"

#include "../../engine/window.h"
#include "../../engine/canvas.h"
#include "../../engine/game_objects/camera.h"

class ViewportWindow : public EditorWindowBase {
public:
	Canvas* canvas = nullptr;
	Camera* camera = nullptr;
	drishengine::Window* window = nullptr;

	float horizontalAngle = 0;
	float verticalAngle = 0;
	
	ViewportWindow(Canvas* canvas, Camera* camera, drishengine::Window* window) {
		this->canvas = canvas;
		this->camera = camera;
		this->window = window;
	}

	void draw() override {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Viewport");
		canvas->resize(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);

		ImGui::SetCursorPos(ImVec2(0, 0));
		ImGui::Image(canvas->texture, ImGui::GetWindowSize(), ImVec2(0, 0), ImVec2(1, -1));
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
		if (
			(ImGui::IsItemHovered() || captured) &&
			glfwGetMouseButton(window->getWindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
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

			float delta = 0.016; //I don't know how to use delta from editor here, so created a variable

			double mouseSpeed = 2.5 * delta;

			horizontalAngle -= delta * dx;
			verticalAngle -= delta * dy;

			verticalAngle = glm::clamp(verticalAngle, -glm::half_pi<float>() + 0.05f, glm::half_pi<float>() - 0.05f);

			glm::vec3 direction(
				cos(verticalAngle) * sin(horizontalAngle),
				sin(verticalAngle),
				cos(verticalAngle) * cos(horizontalAngle)
			);

			direction = glm::normalize(direction);


			glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0, 1, 0), direction));



			glm::vec3 up = glm::cross(-right, direction);



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
				camera->transform.position -= right * speed;
			}
			if (glfwGetKey(window->getWindow(), GLFW_KEY_A) == GLFW_PRESS) {
				camera->transform.position += right * speed;
			}
		}



		ImGui::End();
	}
};