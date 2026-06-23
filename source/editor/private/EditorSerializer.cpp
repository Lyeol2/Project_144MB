#include "EditorSerializer.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <filesystem>
#include "Sprite.h"
#include "Texture.h"

std::string EditorSerializer::SerializeSceneToString(const std::shared_ptr<EditorScene>& scene)
{
    if (!scene) return "";

    std::ostringstream oss;

    // 1. Brushes
    auto& brushes = scene->GetTileBrushes();
    oss << "BRUSHES\n";
    oss << "COUNT " << brushes.size() << "\n";
    for (const auto& brush : brushes)
    {
        oss << brush.name << ","
            << (int)brush.type << ","
            << (brush.hasCollider ? 1 : 0) << ","
            << brush.zLayer << ","
            << (brush.spritePath[0] == '\0' ? "NONE" : brush.spritePath) << "\n";
    }

    // 2. Tiles
    auto tileMap = scene->GetTileMap();
    if (tileMap)
    {
        auto& tiles = tileMap->GetTiles();
        oss << "TILES\n";
        oss << "COUNT " << tiles.size() << "\n";
        for (const auto& pair : tiles)
        {
            const TileData& t = pair.second;
            std::string path = "NONE";
            if (t.sprite && t.sprite->texture) {
                // We actually don't save sprite pointer directly, we save brushId or spritePath
                // Wait, TileData doesn't store spritePath currently, but it stores brushId!
                // If brushId is valid, we can reconstruct it. But if it's placed without brush, it's harder.
                // Fortunately, tiles are always placed with brushId now!
            }
            oss << pair.first.first << "," << pair.first.second << ","
                << t.brushId << ","
                << (int)t.type << ","
                << (t.hasCollider ? 1 : 0) << ","
                << t.zLayer << "\n";
        }
    }
    else
    {
        oss << "TILES\nCOUNT 0\n";
    }

    // 3. Objects
    auto& objects = scene->GetGameObjects();
    oss << "OBJECTS\n";
    oss << "COUNT " << objects.size() << "\n";
    for (const auto& obj : objects)
    {
        oss << obj->GetID() << ","
            << (int)obj->GetType() << ","
            << obj->GetName() << ","
            << obj->GetX() << ","
            << obj->GetY() << "\n";
    }

    return oss.str();
}

bool EditorSerializer::DeserializeSceneFromString(const std::shared_ptr<EditorScene>& scene, const std::string& data)
{
    if (!scene) return false;

    scene->ClearScene();

    std::istringstream iss(data);
    std::string line;

    // Helper for parsing CSV
    auto split = [](const std::string& str) {
        std::vector<std::string> tokens;
        std::stringstream ss(str);
        std::string token;
        while (std::getline(ss, token, ',')) {
            tokens.push_back(token);
        }
        return tokens;
    };

    while (std::getline(iss, line))
    {
        if (line == "BRUSHES")
        {
            std::getline(iss, line); // COUNT X
            int count = std::stoi(line.substr(6));
            scene->GetTileBrushes().clear();
            for (int i = 0; i < count; ++i)
            {
                std::getline(iss, line);
                auto tokens = split(line);
                if (tokens.size() >= 5)
                {
                    EditorTileBrush brush;
                    brush.name = tokens[0];
                    brush.type = (TileType)std::stoi(tokens[1]);
                    brush.hasCollider = std::stoi(tokens[2]) != 0;
                    brush.zLayer = std::stoi(tokens[3]);
                    if (tokens[4] != "NONE")
                        strncpy(brush.spritePath, tokens[4].c_str(), sizeof(brush.spritePath));
                    scene->GetTileBrushes().push_back(brush);
                }
            }
        }
        else if (line == "TILES")
        {
            std::getline(iss, line); // COUNT X
            int count = std::stoi(line.substr(6));
            auto tileMap = scene->GetTileMap();
            if (tileMap) {
                tileMap->Clear();
                for (int i = 0; i < count; ++i)
                {
                    std::getline(iss, line);
                    auto tokens = split(line);
                    if (tokens.size() >= 6)
                    {
                        int x = std::stoi(tokens[0]);
                        int y = std::stoi(tokens[1]);
                        TileData t;
                        t.brushId = std::stoi(tokens[2]);
                        t.type = (TileType)std::stoi(tokens[3]);
                        t.hasCollider = std::stoi(tokens[4]) != 0;
                        t.zLayer = std::stoi(tokens[5]);
                        
                        // We do NOT reconstruct sprite here. It needs to be reconstructed
                        // via GetOrLoadSprite in RenderViewport or Initialize, but wait!
                        // For gameplay, the tilemap uses the sprite.
                        // EditorUIManager handles loading the sprite when rendering if it's null!
                        // So setting it to nullptr is fine!
                        t.sprite = nullptr;

                        tileMap->SetTile(x, y, t);
                    }
                }
            }
        }
        else if (line == "OBJECTS")
        {
            std::getline(iss, line); // COUNT X
            int count = std::stoi(line.substr(6));
            for (int i = 0; i < count; ++i)
            {
                std::getline(iss, line);
                auto tokens = split(line);
                if (tokens.size() >= 5)
                {
                    int id = std::stoi(tokens[0]);
                    EditorObjectType type = (EditorObjectType)std::stoi(tokens[1]);
                    std::string name = tokens[2];
                    float x = std::stof(tokens[3]);
                    float y = std::stof(tokens[4]);

                    auto obj = std::make_shared<EditorGameObject>(id, type, name);
                    obj->SetPosition(x, y);
                    scene->AddGameObject(obj);
                }
            }
        }
    }

    return true;
}

bool EditorSerializer::SaveToFile(const std::shared_ptr<EditorScene>& scene, const std::string& filepath)
{
    // Create directory if it doesn't exist
    size_t lastSlash = filepath.find_last_of("/\\");
    if (lastSlash != std::string::npos)
    {
        std::string dirPath = filepath.substr(0, lastSlash);
        std::filesystem::create_directories(dirPath);
    }

    std::string data = SerializeSceneToString(scene);
    std::ofstream ofs(filepath);
    if (!ofs.is_open()) return false;
    ofs << data;
    ofs.close();
    return true;
}

bool EditorSerializer::LoadFromFile(const std::shared_ptr<EditorScene>& scene, const std::string& filepath)
{
    std::ifstream ifs(filepath);
    if (!ifs.is_open()) return false;
    std::stringstream buffer;
    buffer << ifs.rdbuf();
    return DeserializeSceneFromString(scene, buffer.str());
}
