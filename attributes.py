import os


classes = {}

with open("attributes.h", "w") as file:
    file.write("//generated with attributes.py\n")
    file.write("#pragma once\n")
    file.write('#include "imgui_docking/imgui.h"\n')

    with os.scandir("src/") as it:
        for entry in it:
            if entry.is_file():
                if entry.name[-1] == "h":
                    with open(entry.path, "r") as f:
                        for line in f:
                            if "__ReadAttributes__" in line:
                                classes[entry.name[:-2]] = []
                            if "__Attribute__" in line:
                                line = line.replace("\n", "").replace("\t", "")
                                sp = line.split(" ")
                                attributes = []
                                attributes.append(sp[-2])
                                attributes.append(sp[-1])
                                classes[entry.name[:-2]].append(attributes)



    for i in classes.keys():
        print(i)
        file.write(f'#include "{i+".h"}" \n')

    file.write("void draw(BaseObject* obj) {\n")

    for i in classes.keys():
        s = i.upper()
        file.write(f"if (obj->type == {s})\n")
        file.write("{}\n")
        


    file.write("};\n")

print("Generating attributes OK")
