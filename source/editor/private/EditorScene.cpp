#include "EditorScene.h"

EditorScene::EditorScene()
{
    m_tileMap = new TileMap();
}

EditorScene::~EditorScene()
{
    delete m_tileMap;
    ClearScene();
}

void EditorScene::AddGameObject(std::shared_ptr<EditorGameObject> obj)
{
    if (obj)
    {
        m_gameObjects.push_back(obj);
    }
}

void EditorScene::RemoveGameObject(int id)
{
    for (auto it = m_gameObjects.begin(); it != m_gameObjects.end(); ++it)
    {
        if ((*it)->GetID() == id)
        {
            m_gameObjects.erase(it);
            break;
        }
    }
}

void EditorScene::ClearScene()
{
    m_gameObjects.clear();
}

const std::vector<std::shared_ptr<EditorGameObject>>& EditorScene::GetGameObjects() const
{
    return m_gameObjects;
}

EditorGameObject* EditorScene::FindGameObject(int id) const
{
    for (auto obj : m_gameObjects)
    {
        if (obj->GetID() == id)
        {
            return obj.get();
        }
    }
    return nullptr;
}
