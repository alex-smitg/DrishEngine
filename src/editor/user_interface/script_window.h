#pragma once

#include <string>

#include "imgui_docking/imgui.h"
#include "imgui_docking/imgui_impl_glfw.h"
#include "imgui_docking/imgui_impl_opengl3.h"

#include "../../engine/lua_runner.h"
#include "../../engine/asset_repository.h"
#include "../../engine/assets/script.h"

class ScriptWindow {
private:
	AssetRepository* assetRepository;
	LuaRunner* luaRunner;
public:
	bool open = true;

	ScriptWindow(AssetRepository* assetRepository, LuaRunner* luaRunner) {
		this->assetRepository = assetRepository;
		this->luaRunner = luaRunner;
	}

	void draw() {
		if (open) {
			ImGui::Begin("Script editor", &open);
			ImGui::BeginGroup();

			static std::string scriptName = "script";



			if (ImGui::Button("+")) {
				logDebug("[EDITOR] script name: ", scriptName);
				Script* script = new Script(AUTO_INCREMENT_ID);
				script->name = scriptName;
				//assetRepository->addScript(script);
			}
			if (ImGui::BeginTabBar("tabbar")) {
				/*for (auto const& pair : assetRepository->scriptsMap) {
					std::shared_ptr<Script> script = pair.second;
					if (ImGui::BeginTabItem((script->name + "##" + std::to_string(script->assId)).c_str())) {
						if (ImGui::BeginDragDropSource()) {
							ImGui::SetDragDropPayload("SCRIPT", &script->assId, sizeof(script->assId));
							ImGui::EndDragDropSource();
						}

						ImGui::InputTextMultiline("##script", &script->source, ImVec2(-FLT_MIN,-FLT_MIN), ImGuiInputTextFlags_AllowTabInput);
						ImGui::EndTabItem();
					}
				}*/
				ImGui::EndTabBar();
			}
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
			if (!luaRunner->lastError.empty()) {
				ImGui::Text(luaRunner->lastError.c_str());
			}
			ImGui::PopStyleColor();

			ImGui::EndGroup();

			ImGui::BeginGroup();

			ImGui::EndGroup();

			ImGui::End();
			
		}
	}
};