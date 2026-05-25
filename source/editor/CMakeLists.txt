# source/Application/CMakeLists.txt

add_library(editor INTERFACE)

target_include_directories(editor INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/public
)

target_link_libraries(editor INTERFACE core)
