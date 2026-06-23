#include "Application.h"
#include "Input.h"
#include "Renderer.h"
#include "Scene.h"
#include "GameTime.h"

// 나중에 Player 등 생성할 때 필요
#include "Player.h"
#include "Texture.h"

Application::Application() : m_renderer(nullptr), m_currentScene(nullptr) {}

Application::~Application() noexcept {}

bool Application::Initialize(int width, int height) {
  bIsRunning = true;

  Time::Initialize();
  Input::Initialize();

  m_renderer = new Renderer(width, height);

  // 씬 생성
  m_currentScene = new Scene();
  m_currentScene->Initialize();

  return true;
}

void Application::Run() {
  Time::Update();
  Input::Update();

  if (Input::GetKeyDown(VK_ESCAPE)) {
    bIsRunning = false;
  }

  if (m_currentScene) {
    m_currentScene->Update(Time::GetDeltaTime());

    // Simple Camera Follow Logic
    if (m_renderer && !m_currentScene->GetEntities().empty()) {
      // Find the player (assume it's the first entity or find it by type)
      // Since Player is an Entity, we can dynamic_cast or just use the first entity
      for (Entity* entity : m_currentScene->GetEntities()) {
          // In this simple engine, we can check if it has a sprite and just follow the first entity.
          // Or we can assume it's the player if it's the first one. Let's just follow the first entity for now.
          if (entity->sprite) {
              int camX = (int)entity->x - m_renderer->m_width / 2 + entity->sprite->width / 2;
              int camY = (int)entity->y - m_renderer->m_height / 2 + entity->sprite->height / 2;
              m_renderer->SetCamera(camX, camY);
              break;
          }
      }
    }
  }
}

void Application::Render() {
  if (m_renderer) {
    m_renderer->Clear(0xFFFFFFFF); // 흰색 배경

    if (m_currentScene) {
      m_currentScene->Render(m_renderer);
    }
  }
}

void Application::Shutdown() {
  if (m_currentScene) {
    // 임시로 여기서 텍스처 지우기 처리 (엔티티가 소유 안 함)
    // 실제로는 에셋 매니저가 필요함
    if (!m_currentScene->GetEntities().empty()) {
      Player *p = (Player *)m_currentScene->GetEntities()[0];
      if (p->sprite && p->sprite->texture) {
        delete p->sprite->texture;
      }
    }
    delete m_currentScene;
    m_currentScene = nullptr;
  }

  if (m_renderer) {
    delete m_renderer;
    m_renderer = nullptr;
  }

  bIsRunning = false;
}
