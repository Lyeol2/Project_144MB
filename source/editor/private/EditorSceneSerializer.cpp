#include "EditorSceneSerializer.h"
#include <sstream>

std::string EditorSceneSerializer::Serialize(const std::shared_ptr<EditorScene>& scene) const
{
    if (!scene) return "";

    std::stringstream ss;
    ss << "--- SCENE DATA ---\n";
    
    const auto& objects = scene->GetGameObjects();
    ss << "ObjectCount:" << objects.size() << "\n";
    
    for (const auto& obj : objects)
    {
        ss << obj->SerializeToString() << "\n";
    }

    // 타일 데이터 저장
    ss << "\n--- TILE DATA ---\n";
    const auto& brushes = scene->GetTileBrushes();
    ss << "BrushCount:" << brushes.size() << "\n";
    for (size_t i = 0; i < brushes.size(); ++i)
    {
        const auto& b = brushes[i];
        ss << "Brush:" << b.name << "," << (int)b.type << "," << b.hasCollider << "," << b.zLayer << "," << b.spritePath << "\n";
    }

    if (scene->GetTileMap())
    {
        const auto& tiles = scene->GetTileMap()->GetTiles();
        ss << "MapTiles:\n";
        for (const auto& pair : tiles)
        {
            ss << pair.first.first << "," << pair.first.second << "," << pair.second.brushId << "\n";
        }
    }

    return ss.str();
}

bool EditorSceneSerializer::Deserialize(const std::string& data, std::shared_ptr<EditorScene>& scene) const
{
    if (!scene) return false;
    scene->ClearScene();

    std::stringstream ss(data);
    std::string line;

    // 헤더 패스
    if (std::getline(ss, line))
    {
        if (line != "--- SCENE DATA ---") return false;
    }

    // 오브젝트 개수 파싱 (현재는 검증용으로만 사용)
    if (std::getline(ss, line))
    {
        if (line.find("ObjectCount:") != 0) return false;
    }

    // 오브젝트 파싱
    while (std::getline(ss, line))
    {
        if (line.empty()) continue;
        if (line == "--- TILE DATA ---") break;

        auto obj = std::make_shared<EditorGameObject>(0, EditorObjectType::Anchor, "Temp");
        obj->DeserializeFromString(line);
        scene->AddGameObject(obj);
    }

    // 타일 데이터 파싱
    if (line == "--- TILE DATA ---")
    {
        if (std::getline(ss, line) && line.find("BrushCount:") == 0)
        {
            int brushCount = std::stoi(line.substr(11));
            auto& brushes = scene->GetTileBrushes();
            brushes.clear();

            for (int i = 0; i < brushCount; ++i)
            {
                if (std::getline(ss, line) && line.find("Brush:") == 0)
                {
                    EditorTileBrush b;
                    std::string data = line.substr(6);
                    size_t pos1 = data.find(',');
                    size_t pos2 = data.find(',', pos1 + 1);
                    size_t pos3 = data.find(',', pos2 + 1);
                    size_t pos4 = data.find(',', pos3 + 1);

                    if (pos4 != std::string::npos)
                    {
                        b.name = data.substr(0, pos1);
                        b.type = (TileType)std::stoi(data.substr(pos1 + 1, pos2 - pos1 - 1));
                        b.hasCollider = std::stoi(data.substr(pos2 + 1, pos3 - pos2 - 1));
                        b.zLayer = std::stoi(data.substr(pos3 + 1, pos4 - pos3 - 1));
                        std::string spath = data.substr(pos4 + 1);
                        strncpy(b.spritePath, spath.c_str(), sizeof(b.spritePath));
                        brushes.push_back(b);
                    }
                }
            }
        }

        if (std::getline(ss, line) && line == "MapTiles:")
        {
            auto tileMap = scene->GetTileMap();
            tileMap->Clear();
            auto& brushes = scene->GetTileBrushes();

            while (std::getline(ss, line))
            {
                if (line.empty()) continue;
                size_t p1 = line.find(',');
                size_t p2 = line.find(',', p1 + 1);
                if (p2 != std::string::npos)
                {
                    int x = std::stoi(line.substr(0, p1));
                    int y = std::stoi(line.substr(p1 + 1, p2 - p1 - 1));
                    int bId = std::stoi(line.substr(p2 + 1));

                    if (bId >= 0 && bId < (int)brushes.size())
                    {
                        TileData tdata;
                        tdata.brushId = bId;
                        tdata.type = brushes[bId].type;
                        tdata.hasCollider = brushes[bId].hasCollider;
                        tdata.zLayer = brushes[bId].zLayer;
                        // tdata.sprite는 에디터 실행 중 UI(RenderViewport 등)에서 캐싱된 후 할당되어야 합니다.
                        // 일단 직렬화에서는 ID만 복원합니다.
                        tileMap->SetTile(x, y, tdata);
                    }
                }
            }
        }
    }

    return true;
}
