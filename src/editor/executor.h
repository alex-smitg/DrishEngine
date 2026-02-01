#pragma once

#include "../engine/logger.h"

namespace drishexecutor {
	#include <Windows.h>

	void runGame(drishengine::Window *editorWindow, std::filesystem::path gamePath, std::filesystem::path projectFolderPath) {
		logInfo("Run game");
		logDebug ("[EXECUTOR] game path: ", gamePath.string().c_str());
		logDebug("[EXECUTOR] game project foolder path ", projectFolderPath.string().c_str());

		STARTUPINFO info = { sizeof(info) };
		LPSTR commandLineArg = strdup(projectFolderPath.string().c_str());
		PROCESS_INFORMATION processInfo;
		if (CreateProcessA("game.exe", commandLineArg, NULL, NULL,
			TRUE, NULL, NULL, projectFolderPath.string().c_str(),
			&info, &processInfo)) {
			CloseHandle(processInfo.hProcess);
			CloseHandle(processInfo.hThread);
		}

		wchar_t buf[256];
		FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			buf, (sizeof(buf) / sizeof(wchar_t)), NULL);

		std::wstring ws(buf);
		logDebug(std::string(ws.begin(), ws.end()));
	}
}

