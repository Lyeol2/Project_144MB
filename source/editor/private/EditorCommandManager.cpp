#include "EditorCommandManager.h"
#include "EditorSerializer.h"

void EditorCommandManager::RecordSnapshot(const std::shared_ptr<EditorScene>& scene)
{
    if (m_currentState.empty())
    {
        // First snapshot
        m_currentState = EditorSerializer::SerializeSceneToString(scene);
    }
    else
    {
        std::string newState = EditorSerializer::SerializeSceneToString(scene);
        if (newState != m_currentState)
        {
            m_undoStack.push_back(m_currentState);
            m_currentState = newState;
            m_redoStack.clear();
        }
    }
}

bool EditorCommandManager::Undo(const std::shared_ptr<EditorScene>& scene)
{
    if (m_undoStack.empty()) return false;

    m_redoStack.push_back(m_currentState);
    m_currentState = m_undoStack.back();
    m_undoStack.pop_back();

    EditorSerializer::DeserializeSceneFromString(scene, m_currentState);
    return true;
}

bool EditorCommandManager::Redo(const std::shared_ptr<EditorScene>& scene)
{
    if (m_redoStack.empty()) return false;

    m_undoStack.push_back(m_currentState);
    m_currentState = m_redoStack.back();
    m_redoStack.pop_back();

    EditorSerializer::DeserializeSceneFromString(scene, m_currentState);
    return true;
}
