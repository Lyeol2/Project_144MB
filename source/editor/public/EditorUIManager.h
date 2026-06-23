#pragma once
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

class EditorScene;
class EditorPlacementManager;
class EditorPlayModeController;
class EditorBuildExporter;
class Texture;
class Sprite;
class EditorCommandManager;



struct EditorUIState {
    int selectedObjectType = 0; // 0: Player, 1: Anchor, etc.
    float spawnPos[2] = { 0.0f, 0.0f };
    char savePath[256] = "assets/scenes/level1_build.dat";
    int selectedObjectID = -1; // 인스펙터 창에서 보여줄 오브젝트 ID
    int selectedBrushIndex = -1;

    // 뷰포트 상태
    float viewportOffsetX = 0.0f;
    float viewportOffsetY = 0.0f;
    float viewportZoom = 1.0f;

    // 에셋 브라우저 및 스프라이트 에디터 상태
    std::string currentAssetDir = "assets";
    bool showSpriteEditor = false;
    std::string spriteEditorTarget = "";
    int sliceGridX = 16;
    int sliceGridY = 16;
};

class EditorUIManager {
public:
    EditorUIManager(
        std::shared_ptr<EditorScene> scene,
        EditorPlacementManager* placementMgr,
        EditorPlayModeController* playModeCtrl,
        EditorBuildExporter* buildExporter
    );
    ~EditorUIManager();

    void Initialize();
    void Render();

private:
    void RenderMenuBar();
    void RenderViewport();
    void RenderHierarchy();
    void RenderInspector();
    void RenderAssetBrowser();
    void RenderSpriteEditor(); // 스프라이트 슬라이서 추가
    void RenderToolbar();
    void RenderMainMenuBar();
    void RenderTilePalette();
    void ProcessShortcuts();

    std::shared_ptr<EditorScene> m_scene;
    EditorPlacementManager* m_placementMgr;
    EditorPlayModeController* m_playModeCtrl;
    EditorBuildExporter* m_buildExporter;
    std::unique_ptr<EditorCommandManager> m_commandMgr;

    class Renderer* m_runtimeRenderer; // 런타임 렌더러 연동
    unsigned int m_sceneTextureID;     // OpenGL 텍스처 ID

    EditorUIState m_state;
    std::unordered_map<std::string, Sprite*> m_brushSprites;
    std::unordered_map<Texture*, unsigned int> m_glTextures; // OpenGL 텍스처 ID 캐시

    Sprite* GetOrLoadSprite(const std::string& path);
    unsigned int GetOrCreateGLTexture(Texture* tex); // GL 텍스처 생성 헬퍼
};
