#pragma once
#include "EditorScene.h"
#include <memory>

/// @brief 에디터 상에서 오브젝트의 배치와 트랜스폼을 관리하는 클래스
class EditorPlacementManager
{
public:
    EditorPlacementManager(std::shared_ptr<EditorScene> scene);
    ~EditorPlacementManager() = default;

    /// @brief 씬에 새로운 오브젝트를 배치합니다.
    /// @param type 배치할 오브젝트 타입
    /// @param x 스폰 X 좌표
    /// @param y 스폰 Y 좌표
    /// @return 생성된 오브젝트의 포인터
    std::shared_ptr<EditorGameObject> PlaceObject(EditorObjectType type, float x, float y);

    /// @brief 입력된 좌표를 그리드 크기에 맞게 스냅(Snap)합니다.
    /// @param value 스냅할 좌표값
    /// @param gridSize 그리드 간격 (예: 1.0f)
    /// @return 스냅된 좌표값
    float SnapToGrid(float value, float gridSize) const;

private:
    std::shared_ptr<EditorScene> m_activeScene;
    int m_nextObjectID;
};
