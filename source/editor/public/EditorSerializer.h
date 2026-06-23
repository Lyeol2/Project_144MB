#pragma once
#include "EditorScene.h"
#include <string>
#include <memory>

class EditorSerializer
{
public:
    static bool SaveToFile(const std::shared_ptr<EditorScene>& scene, const std::string& filepath);
    static bool LoadFromFile(const std::shared_ptr<EditorScene>& scene, const std::string& filepath);

    // 메모리 스냅샷 (Undo/Redo 용)
    static std::string SerializeSceneToString(const std::shared_ptr<EditorScene>& scene);
    static bool DeserializeSceneFromString(const std::shared_ptr<EditorScene>& scene, const std::string& data);
};
