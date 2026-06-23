#include "TileMap.h"
#include "Renderer.h"
#include "Sprite.h"

TileMap::TileMap()
    : m_tileSize(16.0f)
{
}

TileMap::~TileMap()
{
    Clear();
}

void TileMap::Initialize()
{
}

void TileMap::Render(Renderer* renderer)
{
    if (!renderer) return;

    for (auto& pair : m_tiles)
    {
        int gridX = pair.first.first;
        int gridY = pair.first.second;
        TileData& tile = pair.second;

        if (tile.sprite)
        {
            // 월드 좌표 계산 (단순화: 1 그리드 = m_tileSize 픽셀)
            int pixelX = (int)(gridX * m_tileSize);
            int pixelY = (int)(gridY * m_tileSize);
            renderer->DrawSprite(pixelX, pixelY, tile.sprite);
        }
    }
}

void TileMap::SetTile(int x, int y, const TileData& data)
{
    m_tiles[{x, y}] = data;
}

void TileMap::RemoveTile(int x, int y)
{
    m_tiles.erase({x, y});
}

TileData* TileMap::GetTile(int x, int y)
{
    auto it = m_tiles.find({x, y});
    if (it != m_tiles.end())
    {
        return &it->second;
    }
    return nullptr;
}

void TileMap::Clear()
{
    m_tiles.clear();
}
