cmake_minimum_required(VERSION 3.20.21)

set(PROJECT_NAME drish_engine)
project(${PROJECT_NAME})

add_subdirectory(lib)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")

set(CMAKE_DISABLE_PRECOMPILE_HEADERS ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)

set(INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include")

set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CMAKE_COMMAND} -E time")



set(GAME game)
add_executable(${GAME})
#hide console
#target_link_options(${GAME} PRIVATE /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup)
set_target_properties(${GAME} PROPERTIES
    COMPILE_OPTIONS "$<$<CONFIG:Debug>:/MTd>$<$<CONFIG:Release>:/MT>"
    )

target_link_libraries(${GAME}
	PRIVATE
		glew
		glfw
		lua
		nfd
		opengl32.lib
)
target_include_directories(${GAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_features(${GAME} PRIVATE cxx_std_20)

target_sources(${GAME}
 PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/include/imgui_docking/imgui.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/imgui_docking/imgui_draw.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/imgui_docking/imgui_impl_glfw.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/imgui_docking/imgui_impl_opengl3.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/imgui_docking/imgui_stdlib.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/imgui_docking/imgui_widgets.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/imgui_docking/imgui_tables.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/imgui_docking/imgui_demo.cpp"
	"src/game/game.cpp"
	"src/engine/lua_runner.cpp"
	"src/engine/game_objects/node.cpp"
	"src/engine/assets/asset.cpp"
	"src/engine/field.h")






set(ENGINE engine)
add_executable(${ENGINE})
set_target_properties(${ENGINE} PROPERTIES
    COMPILE_OPTIONS "$<$<CONFIG:Debug>:/MTd>$<$<CONFIG:Release>:/MT>"
    )
target_link_libraries(${ENGINE}
	PRIVATE
		glew
		glfw
		lua
		nfd
		opengl32.lib
)
target_include_directories(${ENGINE} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_features(${ENGINE} PRIVATE cxx_std_20)


target_sources(${ENGINE}
 PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/include/imgui_docking/imgui.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/imgui_docking/imgui_draw.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/imgui_docking/imgui_impl_glfw.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/imgui_docking/imgui_impl_opengl3.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/imgui_docking/imgui_stdlib.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/imgui_docking/imgui_widgets.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/imgui_docking/imgui_tables.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/imgui_docking/imgui_demo.cpp"
	"src/editor/editor.cpp"
	"src/engine/lua_runner.cpp"
	"src/engine/game_objects/node.cpp"
	"src/engine/assets/asset.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/assets/drish_engine.rc"
	"src/engine/field.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/ImGuiColorTextEdit/TextEditor.cpp")

