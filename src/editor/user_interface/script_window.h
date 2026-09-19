#pragma once

#include <string>

#include "imgui_docking/imgui.h"

#include "../../engine/lua_runner.h"
#include "../../engine/asset_repository.h"
#include "../../engine/assets/script.h"

#include "editor_window_base.h"
#include "../../engine/file_manager.h"


#include "ImGuiColorTextEdit/TextEditor.h"

class ScriptWindow: public EditorWindowBase {
private:
	AssetRepository* assetRepository;
	LuaRunner* luaRunner;
	FileManager* fileManager;
public:

	TextEditor editor;
	

	ScriptWindow(AssetRepository* assetRepository, LuaRunner* luaRunner, FileManager* fileManager) {
		this->assetRepository = assetRepository;
		this->luaRunner = luaRunner;
		this->fileManager = fileManager;

		editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Lua());  
		editor.SetPalette(TextEditor::GetDarkPalette());
		
	}

	void draw() override {
		if (open) {
			ImGui::Begin("Script editor", &open);
			ImGui::BeginGroup();

			static int lastIndex = -1;
			static bool modified = false;

			if (editor.IsTextChanged()) {
				modified = true;
			}

			if (ImGui::BeginTabBar("tabbar")) {
				for (const AssetSlot<Script>* assetSlot : assetRepository->scripts.slots)
				{
					ImGui::PushID(assetSlot->index);
					if (assetSlot->is_valid) {
						Script* script = assetSlot->asset;

						
						
						if (ImGui::BeginTabItem(script->name.c_str())) {
							if (ImGui::BeginDragDropSource()) {
								ImGui::SetDragDropPayload("SCRIPT", &assetSlot->index, sizeof(assetSlot->index));
								ImGui::EndDragDropSource();
							}
							if (lastIndex != assetSlot->index) {
								/*script->source = editor.GetText();
								fileManager->writeScript(script);
								logDebug("[SCRIPT WINDOW] Script saved");*/

								editor.SetText(script->source);
								logDebug("[SCRIPT WINDOW] Tab changed");
							}
							lastIndex = assetSlot->index;

							if (ImGui::Button("Save")) {
								script->source = editor.GetText();
								fileManager->writeScript(script);
								logDebug("[SCRIPT WINDOW] Script saved");
							}
							ImGui::SameLine(0.0, 1.0);
							ImGui::Spacing();
							
							
							editor.Render("Script");

							if (ImGui::Button("Reload from disk")) {
								fileManager->loadScript(script);
							}
							ImGui::EndTabItem();

						}
					}
					ImGui::PopID();
				}
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