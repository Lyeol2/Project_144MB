#include <windows.h>
#include "Application.h"
#include "Renderer.h"

namespace {
constexpr wchar_t kWindowClassName[] = L"Project144MBLauncherWindow";

LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam,
                            LPARAM lParam) {
  switch (message) {
  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;
  default:
    return DefWindowProcW(window, message, wParam, lParam);
  }
}
} // namespace

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int showCommand) {
  WNDCLASSEXW windowClass{};
  windowClass.cbSize = sizeof(windowClass);
  windowClass.style = CS_HREDRAW | CS_VREDRAW;
  windowClass.lpfnWndProc = WindowProc;
  windowClass.hInstance = instance;
  windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
  windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
  windowClass.lpszClassName = kWindowClassName;

  if (!RegisterClassExW(&windowClass)) {
    return 1;
  }

  // 클라이언트 영역(실제 게임 화면) 크기를 800x600으로 맞추기 위해 윈도우 크기 계산
  RECT rect = { 0, 0, 800, 600 };
  AdjustWindowRectEx(&rect, WS_POPUP, FALSE, 0);
  int windowWidth = rect.right - rect.left;
  int windowHeight = rect.bottom - rect.top;

  HWND window = CreateWindowExW(0, kWindowClassName, L"Project 144MB", WS_POPUP,
                                CW_USEDEFAULT, CW_USEDEFAULT, windowWidth, windowHeight,
                                nullptr, nullptr, instance, nullptr);

  if (window == nullptr) {
    return 1;
  }

  ShowWindow(window, showCommand);
  UpdateWindow(window);

  Application app;
  if (!app.Initialize(800, 600)) {
    return 1;
  }

  MSG message{};
  bool bIsQuit = false;
  
  while (!bIsQuit && app.IsRunning()) {
    // 큐에 있는 모든 메시지를 처리
    while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
      if (message.message == WM_QUIT) {
        bIsQuit = true;
        break; // 내부 while문 탈출
      }
      TranslateMessage(&message);
      DispatchMessageW(&message);
    }

    // 메시지 처리가 끝나면 (또는 메시지가 없으면) 매 프레임 게임 로직 실행
    if (!bIsQuit && app.IsRunning()) {
      app.Run();
      app.Render();
      if (app.GetRenderer()) {
        app.GetRenderer()->Present(window);
      }
    }
  }

  app.Shutdown();

  return static_cast<int>(message.wParam);
}
