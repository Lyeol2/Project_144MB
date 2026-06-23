#pragma once
#include <windows.h>

class Sprite;

class Renderer {
public:
  Renderer(int width, int height);
  Renderer(const Renderer &) = delete;
  virtual ~Renderer();

  int m_width;
  int m_height;
  unsigned int* m_screenbuffer[2]; // 32비트 ARGB 픽셀 (더블 버퍼링)
  int m_backBufferIndex;

  bool Initialize(int width, int height);

  // 화면을 단색으로 지우기
  void Clear(unsigned int color = 0xFF000000);

  // 스프라이트 그리기 (기본적인 픽셀 복사)
  void DrawSprite(int x, int y, Sprite* sprite);

  // 완성된 백버퍼를 실제 윈도우 화면에 출력
  void Present(HWND hwnd);

  void SetCamera(int x, int y) { m_cameraX = x; m_cameraY = y; }
  int GetCameraX() const { return m_cameraX; }
  int GetCameraY() const { return m_cameraY; }

protected:
  int m_cameraX = 0;
  int m_cameraY = 0;
};
