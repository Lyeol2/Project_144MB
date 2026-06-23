# source/editor/CMakeLists.txt

# 1. 기존의 editor 라이브러리 (launcher에서 참조)
add_library(editor STATIC 
    private/EditorGameObject.cpp
    private/EditorScene.cpp
    private/EditorPlacementManager.cpp
    private/EditorPlayModeController.cpp
    private/EditorBuildExporter.cpp
    private/EditorSerializer.cpp
    private/EditorCommandManager.cpp
)

target_link_libraries(editor PUBLIC core)
target_include_directories(editor PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/public
)

# 2. 독립 실행형 Editor 프로그램 (ImGui 연동)
include(FetchContent)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.3.8
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/third_party/imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# Editor 독립 실행 파일 생성
add_executable(EditorApp 
    private/main.cpp
    private/EditorUIManager.cpp
    ${IMGUI_SOURCES}
)

target_include_directories(EditorApp PRIVATE
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

# 에디터 로직(editor 라이브러리) 및 외부 라이브러리 링킹
target_link_libraries(EditorApp PRIVATE editor glfw opengl32)

# 컴파일 후 최상위 디렉토리로 복사
add_custom_command(TARGET EditorApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:EditorApp>
    ${CMAKE_SOURCE_DIR}/Editor.exe
    COMMENT "Copying Editor.exe to root directory..."
)
