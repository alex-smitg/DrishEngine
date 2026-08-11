#pragma once

#include <string>
#include <vector>
#include <ios>
#include <fstream>
#include <chrono>



#define IMAGE_LOADER "IMAGE LOADER "

enum class LogType { Info, Debug, Warning, Error };

inline std::string logTypeString[] = { "INFO", "DEBUG", "WARNING", "ERROR" };

struct Message {
	std::string text;
	LogType type;
};

inline std::ofstream file("log.txt", std::ios_base::trunc | std::ios_base::out);
inline std::vector<Message> logMessages;

template<class... Args>
void log(LogType type, Args&&... args) {
	std::ostringstream oss;
	(oss << ... << args);
	std::string textStr = oss.str();



	if (file) {
		auto now = std::chrono::system_clock::now();
		file << "[" << now << "] ";
		file << "[" << logTypeString[static_cast<unsigned int>(type)] << "] " << textStr << std::endl;
	}

	std::cout << "[" << logTypeString[static_cast<unsigned int>(type)] << "] ";
	std::cout << textStr << std::endl;

	Message message = Message();
	message.text = textStr;
	message.type = type;
	logMessages.push_back(message);
};

template<class... Args>
void logInfo(Args... args) {
	log(LogType::Info, std::forward<Args>(args)...);
}

template<class... Args>
void logError(Args... args) {
	log(LogType::Error, std::forward<Args>(args)...);
}

template<class... Args>
void logWarning(Args... args) {
	log(LogType::Warning, std::forward<Args>(args)...);
}

template<class... Args>
void logDebug(Args... args) {
	log(LogType::Debug, std::forward<Args>(args)...);
}




