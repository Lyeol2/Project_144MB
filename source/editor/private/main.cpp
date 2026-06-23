// editor/main.cpp
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h> 

#include <iostream>
#include <memory>
#include <string>

// 커스텀 에디터 모듈들
#include "EditorScene.h"
#include "EditorPlacementManager.h"
#include "EditorSceneSerializer.h"
#include "EditorPlayModeController.h"
#include "EditorBuildExporter.h"
#include "EditorUIManager.h"

static void glfw_error_callback(int error, const char* description)
{
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

int main(int, char**)
{
    // 1. GLFW 초기화
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    
    // 윈도우 생성
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Editor", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // V-Sync 활성화

    // 2. ImGui 초기화
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; 
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // 도킹 활성화

    ImGui::StyleColorsDark();

    // 3. 백엔드 연동
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // 4. 에디터 코어 데이터 초기화
    auto scene = std::make_shared<EditorScene>();
    EditorPlacementManager placementManager(scene);
    EditorSceneSerializer serializer;
    EditorPlayModeController playController;
    EditorBuildExporter exporter;

    // 5. UIManager 초기화
    EditorUIManager uiManager(scene, &placementManager, &playController, &exporter);
    uiManager.Initialize();

    // 6. 메인 루프
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 모든 UI 렌더링을 UIManager로 위임
        uiManager.Render();

        // 렌더링 파이프라인
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // 멀티 뷰포트 지원
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }

    // 종료 시 정리
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
