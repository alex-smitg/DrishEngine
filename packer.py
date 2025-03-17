import os

with open("src/shader_resources.h", "w") as file:
    file.write("#pragma once\n")
    file.write("#include <map>\n")
    file.write("#include <string>\n")
    file.write("std::map<std::string, std::string> shaders = {\n")
    with os.scandir("assets/shaders/") as it:
        for entry in it:
            if entry.is_file():
                file.write('{"'+entry.name+'",'+'R"(')
                
                with open(entry.path, "r") as f:
                    file.write(f.read())
                file.write(')"},')


    file.write("};")
print("Packing shaders OK")
