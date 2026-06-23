#pragma once
#include <vector>
#include <string>
#include <memory>
#include "EditorScene.h"

class EditorCommandManager
{
public:
    EditorCommandManager() = default;
    ~EditorCommandManager() = default;

    void RecordSnapshot(const std::shared_ptr<EditorScene>& scene);
    bool Undo(const std::shared_ptr<EditorScene>& scene);
    bool Redo(const std::shared_ptr<EditorScene>& scene);

private:
    std::vector<std::string> m_undoStack;
    std::vector<std::string> m_redoStack;
    std::string m_currentState;
};
