#pragma once

class Renderer;
class Scene;

class Application
{
public:
	Application();
	virtual ~Application() noexcept;

	virtual bool Initialize(int width, int height);

	// 매 프레임 업데이트
	virtual void Run();

	// 프레임 렌더링
	virtual void Render();

	virtual void Shutdown();

	Renderer* GetRenderer() const { return m_renderer; }
	Scene* GetCurrentScene() const { return m_currentScene; }
	bool IsRunning() const { return bIsRunning; }

protected:
	bool bIsRunning = false;
	Renderer* m_renderer = nullptr;
	Scene* m_currentScene = nullptr;
};