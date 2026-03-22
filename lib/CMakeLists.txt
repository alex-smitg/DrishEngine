add_subdirectory(glfw-3.4)
set_target_properties(glfw PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreadedDebug"
)

add_subdirectory(lua-5.4.8)
set_target_properties(lua PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreadedDebug"
)

add_subdirectory(glew-2.3.1)
set_target_properties(glew PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreadedDebug"
)

add_subdirectory(nativefiledialog-extended-1.3.0)
set_target_properties(nfd PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreadedDebug"
)