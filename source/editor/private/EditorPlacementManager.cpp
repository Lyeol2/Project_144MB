#include "EditorPlacementManager.h"
#include <cmath>

EditorPlacementManager::EditorPlacementManager(std::shared_ptr<EditorScene> scene)
    : m_activeScene(scene), m_nextObjectID(1)
{
}

std::shared_ptr<EditorGameObject> EditorPlacementManager::PlaceObject(EditorObjectType type, float x, float y)
{
    if (!m_activeScene) return nullptr;

    std::string defaultName = "Object_" + std::to_string(m_nextObjectID);
    auto newObj = std::make_shared<EditorGameObject>(m_nextObjectID++, type, defaultName);
    newObj->SetPosition(x, y);
    
    m_activeScene->AddGameObject(newObj);
    return newObj;
}

float EditorPlacementManager::SnapToGrid(float value, float gridSize) const
{
    if (gridSize <= 0.0f) return value;
    return std::round(value / gridSize) * gridSize;
}
