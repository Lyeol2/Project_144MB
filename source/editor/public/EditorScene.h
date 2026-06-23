#pragma once
#include <vector>
#include <memory>
#include "EditorGameObject.h"
#include "TileMap.h"

struct EditorTileBrush {
    std::string name;
    TileType type = TileType::Background;
    bool hasCollider = false;
    int zLayer = 0;
    char spritePath[256] = "";
};

/// @brief 현재 에디터에서 열려 있는 레벨 데이터를 담는 씬 컨테이너 클래스
class EditorScene
{
public:
    EditorScene();
    ~EditorScene();

    /// @brief 씬에 새로운 오브젝트를 추가합니다.
    /// @param obj 추가할 오브젝트의 공유 포인터
    void AddGameObject(std::shared_ptr<EditorGameObject> obj);

    /// @brief 씬에서 특정 ID의 오브젝트를 제거합니다.
    void RemoveGameObject(int id);

    /// @brief 씬에 있는 모든 오브젝트를 삭제하여 초기화합니다.
    void ClearScene();

    /// @brief 배치된 모든 오브젝트 리스트를 반환합니다.
    const std::vector<std::shared_ptr<EditorGameObject>>& GetGameObjects() const;
    
    EditorGameObject* FindGameObject(int id) const;

    TileMap* GetTileMap() const { return m_tileMap; }

    // 타일 브러시 관리
    std::vector<struct EditorTileBrush>& GetTileBrushes() { return m_tileBrushes; }
    const std::vector<struct EditorTileBrush>& GetTileBrushes() const { return m_tileBrushes; }

private:
    std::vector<std::shared_ptr<EditorGameObject>> m_gameObjects;
    TileMap* m_tileMap;
    std::vector<struct EditorTileBrush> m_tileBrushes;
};
