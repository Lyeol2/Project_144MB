# source/CMakeLists.txt

add_subdirectory(core)
add_subdirectory(editor)

add_executable(launcher WIN32
    core/private/Launch.cpp
)

target_link_libraries(launcher PRIVATE
    core
    editor
)

target_compile_definitions(launcher PRIVATE
    UNICODE
    _UNICODE
)

add_custom_command(TARGET launcher POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:launcher> ${CMAKE_SOURCE_DIR}/launcher.exe
    COMMENT "Copying launcher.exe to root directory..."
)
