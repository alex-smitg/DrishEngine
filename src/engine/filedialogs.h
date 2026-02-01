#pragma once

#include <string>
#include <nfd.h>
#include <objbase.h>

#include "logger.h"



namespace drishengine {
	
	static std::filesystem::path openFileOpenDialog(const char* name, const char* ext) {
		nfdu8char_t* outPath = NULL;
		nfdfilteritem_t filterItem[1] = { {name, ext} };
		nfdresult_t result = NFD_ERROR;

		//TODO: Make imgui engine file dialog;
		try {
			result = NFD_OpenDialogU8(&outPath, filterItem, 1, NULL);
		}
		catch (...) {
			logError("[FILE DIALOGS] ", "Nfd error");
		}

		if (result == NFD_OKAY) {
			std::filesystem::path path = std::filesystem::u8path(outPath);
			free(outPath);
			logDebug("[FILE DIALOGS] ", "Drish path: ", path.generic_string());
			logDebug("[FILE DIALOGS] ", "Drish parent path: ", path.parent_path());
			return path;
		}
		return std::filesystem::path();
	}

	static std::filesystem::path openDrishOpenFileDialog() {
		return openFileOpenDialog("Project file", "drish");
	}

	static std::filesystem::path openImageOpenFileDialog() {
		return openFileOpenDialog("Image", "png,jpeg,jpg,webp,bmp");
	}
	static std::filesystem::path openModelOpenFileDialog() {
		return openFileOpenDialog("Model", "obj");
	}

	static std::filesystem::path openDrishSaveDialog() {
		nfdu8char_t* outPath = NULL;
		nfdresult_t result = NFD_SaveDialogU8(&outPath, NULL, 0, NULL, "world.drish");
		if (result == NFD_OKAY) {
			std::filesystem::path new_path = std::filesystem::u8path(outPath);
			free(outPath);
			logDebug("[FILE DIALOGS] ", "Save drish path: ", new_path.generic_string());
			logDebug("[FILE DIALOGS] ", "Save drish parent path: ", new_path.parent_path());
			return new_path;
		}
		return std::filesystem::path();
	}
}