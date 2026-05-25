# source/CMakeLists.txt

add_subdirectory(core)
add_subdirectory(editor)
# add_subdirectory(gam)

add_executable(launcher WIN32
    core/private/Launch.cpp
)

target_link_libraries(launcher PRIVATE
    editor
)

target_compile_definitions(launcher PRIVATE
    UNICODE
    _UNICODE
)
