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

#include "EditorSerializer.h"
#include "EditorScene.h"
#include "EditorGameObject.h"
#include "Scene.h"
#include "TileMap.h"
#include "Player.h"
#include "Texture.h"
#include "Sprite.h"
#include <unordered_map>
#include <fstream>
#include <string>

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

  RECT rect = { 0, 0, 960, 720 };
  AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, FALSE, 0);
  int windowWidth = rect.right - rect.left;
  int windowHeight = rect.bottom - rect.top;

  HWND window = CreateWindowExW(0, kWindowClassName, L"Project 144MB", WS_OVERLAPPEDWINDOW,
                                CW_USEDEFAULT, CW_USEDEFAULT, windowWidth, windowHeight,
                                nullptr, nullptr, instance, nullptr);

  if (window == nullptr) {
    return 1;
  }

  ShowWindow(window, showCommand);
  UpdateWindow(window);

  Application app;
  if (!app.Initialize(320, 240)) {
    return 1;
  }

  // Load Scene from level1_build.dat
  auto editorScene = std::make_shared<EditorScene>();
  if (EditorSerializer::LoadFromFile(editorScene, "assets/scenes/level1_build.dat"))
  {
      Scene* scene = app.GetCurrentScene();
      
      // 1. Load Brushes into Texture and Sprite
      std::unordered_map<std::string, Texture*> texCache;
      std::unordered_map<int, Sprite*> brushSprites;
      auto& brushes = editorScene->GetTileBrushes();
      for (size_t i = 0; i < brushes.size(); ++i)
      {
          if (brushes[i].spritePath[0] != '\0' && std::string(brushes[i].spritePath) != "NONE")
          {
              std::string path = brushes[i].spritePath;
              while (!path.empty() && (path.back() == '\r' || path.back() == '\n' || path.back() == ' '))
                  path.pop_back();

              if (path.ends_with(".sprite"))
              {
                  std::ifstream ifs(path);
                  if (ifs.is_open())
                  {
                      std::string line;
                      std::string texPath;
                      int srcX = 0, srcY = 0, width = 0, height = 0;
                      while (std::getline(ifs, line))
                      {
                          while (!line.empty() && (line.back() == '\r' || line.back() == '\n' || line.back() == ' '))
                              line.pop_back();

                          if (line.starts_with("Texture=")) texPath = line.substr(8);
                          else if (line.starts_with("SrcX=")) srcX = std::stoi(line.substr(5));
                          else if (line.starts_with("SrcY=")) srcY = std::stoi(line.substr(5));
                          else if (line.starts_with("Width=")) width = std::stoi(line.substr(6));
                          else if (line.starts_with("Height=")) height = std::stoi(line.substr(7));
                      }
                      
                      Texture* tex = nullptr;
                      if (texCache.count(texPath)) {
                          tex = texCache[texPath];
                      } else {
                          tex = new Texture();
                          if (tex->LoadFromFile(texPath)) {
                              texCache[texPath] = tex;
                          } else {
                              delete tex;
                              tex = nullptr;
                          }
                      }
                      
                      if (tex)
                      {
                          Sprite* spr = new Sprite();
                          spr->texture = tex;
                          spr->srcX = srcX;
                          spr->srcY = srcY;
                          spr->width = width;
                          spr->height = height;
                          brushSprites[i] = spr;
                      }
                  }
              }
              else
              {
                  Texture* tex = new Texture();
                  if (tex->LoadFromFile(path))
                  {
                      Sprite* spr = new Sprite();
                      spr->texture = tex;
                      spr->srcX = 0;
                      spr->srcY = 0;
                      spr->width = tex->GetWidth();
                      spr->height = tex->GetHeight();
                      brushSprites[i] = spr;
                  }
                  else { delete tex; }
              }
          }
      }

      // 2. Load TileMap
      auto editorTileMap = editorScene->GetTileMap();
      auto gameTileMap = scene->GetTileMap();
      if (editorTileMap && gameTileMap)
      {
          for (const auto& pair : editorTileMap->GetTiles())
          {
              TileData td = pair.second;
              if (brushSprites.count(td.brushId))
              {
                  td.sprite = brushSprites[td.brushId];
              }
              gameTileMap->SetTile(pair.first.first, pair.first.second, td);
          }
      }

      // 3. Load GameObjects
      for (auto& obj : editorScene->GetGameObjects())
      {
          if (obj->GetType() == EditorObjectType::Player)
          {
              Player* player = new Player();
              // Convert grid coordinates to world pixel coordinates (1 grid = 16 pixels)
              player->x = obj->GetX() * 16.0f;
              player->y = obj->GetY() * 16.0f;
              
              Texture* tex = new Texture();
              tex->LoadFromFile("assets/black16.png");
              player->sprite = new Sprite();
              player->sprite->texture = tex;
              player->sprite->width = 16;
              player->sprite->height = 16;

              scene->AddEntity(player);
          }
      }
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
