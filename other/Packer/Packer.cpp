#include <iostream>
#include <fstream>  
#include <string>
#include <filesystem>
namespace fs = std::filesystem;

int main()
{
    std::ofstream file;


   

    file.open("shader_resources.h", std::ofstream::out | std::ofstream::trunc);

    file << "#pragma once" << std::endl;
    file << "#include <map>" << std::endl;
    file << "#include <string>" << std::endl;
    file << "std::map<std::string, std::string> shaders = {" << std::endl;
    std::string path = "shaders";


    for (const auto& entry : std::filesystem::recursive_directory_iterator(path, std::filesystem::directory_options::skip_permission_denied)) {

        std::ifstream shader_file(entry.path());

        std::string line;

        file << "{";

        file << fs::path(entry.path()).filename();

        file << ", R\"(";

        while (getline(shader_file, line)) {
            file << line << "\n";
        }

        file << ")\"";

        file << "}," << std::endl;
    }
        
    file << "};" << std::endl;

    file.close();
}

