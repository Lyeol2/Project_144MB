#pragma once
#include <unordered_map>
#include <string>

class Renderer;
class Sprite;

enum class TileType {
    Background,
    Wall
};

struct TileData {
    int brushId = -1;
    TileType type = TileType::Background;
    bool hasCollider = false;
    int zLayer = 0;
    Sprite* sprite = nullptr;
};

// 64비트 해시 함수 (std::pair<int, int>용)
struct PairHash {
    std::size_t operator()(const std::pair<int, int>& p) const {
        return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1);
    }
};

class TileMap
{
public:
    TileMap();
    ~TileMap();

    void Initialize();
    void Render(Renderer* renderer);

    void SetTile(int x, int y, const TileData& data);
    void RemoveTile(int x, int y);
    TileData* GetTile(int x, int y);

    void Clear();

    const std::unordered_map<std::pair<int, int>, TileData, PairHash>& GetTiles() const { return m_tiles; }

private:
    std::unordered_map<std::pair<int, int>, TileData, PairHash> m_tiles;
    float m_tileSize;
};
