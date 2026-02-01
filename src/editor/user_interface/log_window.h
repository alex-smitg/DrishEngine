#pragma once

#include <string>

#include "imgui_docking/imgui.h"
#include "imgui_docking/imgui_impl_glfw.h"
#include "imgui_docking/imgui_impl_opengl3.h"

#include "../../engine/logger.h"

#include "editor_window.h"

class LogWindow: public EditorWindow {
public:
	bool showInfo = true;
	bool showError = true;
	bool showDebug = true;
	bool showWarning = true;

	void draw() override {
		if (open) {
			ImGui::SetNextWindowSize(ImVec2(520, 300), ImGuiCond_FirstUseEver);
			if (ImGui::Begin("Log", &open)) {

				ImGui::Checkbox("Info", &showInfo);
				ImGui::SameLine();
				ImGui::Checkbox("Error", &showError);
				ImGui::SameLine();
				ImGui::Checkbox("Debug", &showDebug);
				ImGui::SameLine();
				ImGui::Checkbox("Warning", &showWarning);;
				ImGui::Separator();

				if (ImGui::BeginChild("scroll", ImVec2(0, 0), ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_HorizontalScrollbar)) {
					for (int i = 0; i < logMessages.size(); i++) {
						Message message = logMessages[i];

						if (!showInfo && message.type == LogType::Info) continue;
						if (!showError && message.type == LogType::Error) continue;
						if (!showDebug && message.type == LogType::Debug) continue;
						if (!showWarning && message.type == LogType::Warning) continue;

						switch (logMessages[i].type)
						{
						case LogType::Info:
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8, 0.8, 0.8, 1.0));
							break;
						case LogType::Error:
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.45, 0.45, 1.0));
							break;
						case LogType::Debug:
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3, 0.7, 1.0, 1.0));
							break;
						case LogType::Warning:
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.7, 0.0, 1.0));
							break;

						default:
							break;
						}
						ImGui::Text(logMessages[i].text.c_str());
						ImGui::PopStyleColor();
					}
					if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
						ImGui::SetScrollHereY(1.0f);
					}
				}
				ImGui::EndChild();
			}
			ImGui::End();
		}
	}
};