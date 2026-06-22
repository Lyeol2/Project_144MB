#include "Application.h"
#include "Input.h"
#include "Renderer.h"
#include "Scene.h"
#include "Time.h"

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

  // 임시 씬 생성 및 플레이어 배치
  m_currentScene = new Scene();

  Player *player = new Player();
  player->x = width / 2.0f;
  player->y = height / 2.0f;

  // 텍스처 로드 및 스프라이트 설정
  Texture *tex = new Texture();
  tex->LoadFromFile("black16.png");

  player->sprite = new Sprite();
  player->sprite->texture = tex;
  player->sprite->width = 16;
  player->sprite->height = 16;

  m_currentScene->AddEntity(player);
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
