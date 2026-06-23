#include "EditorUIManager.h"
#include "EditorScene.h"
#include "EditorPlacementManager.h"
#include "EditorPlayModeController.h"
#include "EditorBuildExporter.h"
#include "EditorCommandManager.h"
#include "EditorSerializer.h"

#include "Renderer.h"
#include <GLFW/glfw3.h>

#include "imgui.h"
#include <iostream>
#include <filesystem>
#include <string>
#include <algorithm>
#include <fstream>
#include <cstdlib>

namespace fs = std::filesystem;

#include "Texture.h"
#include "Sprite.h"

Sprite* EditorUIManager::GetOrLoadSprite(const std::string& path)
{
    if (path.empty()) return nullptr;

    auto it = m_brushSprites.find(path);
    if (it != m_brushSprites.end())
        return it->second;

    bool isSpriteFile = (path.size() >= 7 && path.compare(path.size() - 7, 7, ".sprite") == 0);
    
    std::string texPath = path;
    int srcX = 0, srcY = 0, width = 0, height = 0;

    if (isSpriteFile)
    {
        std::ifstream ifs(path);
        if (ifs.is_open())
        {
            std::string line;
            while (std::getline(ifs, line))
            {
                auto pos = line.find('=');
                if (pos != std::string::npos)
                {
                    std::string key = line.substr(0, pos);
                    std::string val = line.substr(pos + 1);
                    if (key == "Texture") texPath = val;
                    else if (key == "SrcX") srcX = std::stoi(val);
                    else if (key == "SrcY") srcY = std::stoi(val);
                    else if (key == "Width") width = std::stoi(val);
                    else if (key == "Height") height = std::stoi(val);
                }
            }
        }
    }

    Texture* tex = new Texture();
    if (tex->LoadFromFile(texPath))
    {
        Sprite* spr = new Sprite();
        spr->texture = tex;
        spr->srcX = srcX;
        spr->srcY = srcY;
        spr->width = width > 0 ? width : tex->GetWidth();
        spr->height = height > 0 ? height : tex->GetHeight();
        m_brushSprites[path] = spr;
        return spr;
    }
    
    delete tex;
    return nullptr;
}

unsigned int EditorUIManager::GetOrCreateGLTexture(Texture* tex)
{
    if (!tex || !tex->GetPixels()) return 0;
    
    auto it = m_glTextures.find(tex);
    if (it != m_glTextures.end())
        return it->second;
        
    unsigned int glID;
    glGenTextures(1, &glID);
    glBindTexture(GL_TEXTURE_2D, glID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    // GDI+ 픽셀은 기본적으로 BGRA 순서입니다.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex->GetWidth(), tex->GetHeight(), 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, tex->GetPixels());
    
    m_glTextures[tex] = glID;
    return glID;
}

EditorUIManager::EditorUIManager(
    std::shared_ptr<EditorScene> scene,
    EditorPlacementManager* placementMgr,
    EditorPlayModeController* playModeCtrl,
    EditorBuildExporter* buildExporter)
    : m_scene(scene), m_placementMgr(placementMgr), m_playModeCtrl(playModeCtrl), m_buildExporter(buildExporter)
{
    m_commandMgr = std::make_unique<EditorCommandManager>();
    m_commandMgr->RecordSnapshot(m_scene);

    m_runtimeRenderer = new Renderer(800, 600);

    glGenTextures(1, &m_sceneTextureID);
    glBindTexture(GL_TEXTURE_2D, m_sceneTextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

EditorUIManager::~EditorUIManager()
{
    delete m_runtimeRenderer;
    for (auto& pair : m_brushSprites)
    {
        delete pair.second->texture;
        delete pair.second;
    }
    m_brushSprites.clear();
}

void EditorUIManager::Initialize()
{
    // TODO: 초기화 로직 (필요시)
}

void EditorUIManager::Render()
{
    ProcessShortcuts();

    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

    RenderMenuBar();
    RenderToolbar();
    RenderViewport();
    RenderTilePalette();
    RenderHierarchy();
    RenderInspector();
    RenderAssetBrowser();

    if (m_state.showSpriteEditor)
    {
        RenderSpriteEditor();
    }
}

void EditorUIManager::RenderMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New Scene"))
            {
                m_scene->ClearScene();
            }
            if (ImGui::MenuItem("Save Scene"))
            {
                EditorSerializer::SaveToFile(m_scene, m_state.savePath);
            }
            if (ImGui::MenuItem("Load Scene"))
            {
                EditorSerializer::LoadFromFile(m_scene, m_state.savePath);
                m_commandMgr->RecordSnapshot(m_scene);
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit"))
            {
                exit(0);
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void EditorUIManager::RenderToolbar()
{
    ImGui::Begin("Toolbar");
    
    if (ImGui::Button("Play")) m_playModeCtrl->StartPlayMode();
    ImGui::SameLine();
    if (ImGui::Button("Stop")) m_playModeCtrl->StopPlayMode();

    ImGui::SameLine();
    ImGui::Text("|");
    ImGui::SameLine();

    ImGui::InputText("Save Path", m_state.savePath, 256);
    ImGui::SameLine();
    if (ImGui::Button("Export Build"))
    {
        m_buildExporter->ExportSceneForGame(m_scene, m_state.savePath);
    }
    
    ImGui::End();
}

void EditorUIManager::RenderViewport()
{
    ImGui::Begin("Scene Viewport", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    ImVec2 vMin = ImGui::GetWindowContentRegionMin();
    ImVec2 vMax = ImGui::GetWindowContentRegionMax();
    vMin.x += ImGui::GetWindowPos().x;
    vMin.y += ImGui::GetWindowPos().y;
    vMax.x += ImGui::GetWindowPos().x;
    vMax.y += ImGui::GetWindowPos().y;

    ImVec2 viewSize = ImVec2(vMax.x - vMin.x, vMax.y - vMin.y);

    m_runtimeRenderer->Clear(0xFF202020);

    if (m_scene->GetTileMap())
    {
        auto& tiles = const_cast<std::unordered_map<std::pair<int, int>, TileData, PairHash>&>(m_scene->GetTileMap()->GetTiles());
        for (auto& pair : tiles) {
            if (!pair.second.sprite && pair.second.brushId >= 0 && pair.second.brushId < m_scene->GetTileBrushes().size()) {
                pair.second.sprite = GetOrLoadSprite(m_scene->GetTileBrushes()[pair.second.brushId].spritePath);
            }
        }
        m_scene->GetTileMap()->Render(m_runtimeRenderer);
    }

    glBindTexture(GL_TEXTURE_2D, m_sceneTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_runtimeRenderer->m_width, m_runtimeRenderer->m_height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, m_runtimeRenderer->m_screenbuffer[m_runtimeRenderer->m_backBufferIndex]);

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    
    // 배경 (빈 공간) 클리어
    drawList->AddRectFilled(vMin, vMax, IM_COL32(30, 30, 30, 255));

    float sx = viewSize.x / m_runtimeRenderer->m_width;
    float sy = viewSize.y / m_runtimeRenderer->m_height;
    float baseScale = (sx < sy) ? sx : sy;
    float finalScale = baseScale * m_state.viewportZoom;
    ImVec2 baseOffset((viewSize.x - m_runtimeRenderer->m_width * baseScale) * 0.5f, (viewSize.y - m_runtimeRenderer->m_height * baseScale) * 0.5f);

    if (ImGui::IsWindowHovered())
    {
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle, 0.0f))
        {
            ImVec2 delta = ImGui::GetIO().MouseDelta;
            m_state.viewportOffsetX += delta.x;
            m_state.viewportOffsetY += delta.y;
        }

        if (ImGui::GetIO().MouseWheel != 0.0f)
        {
            float wheel = ImGui::GetIO().MouseWheel;
            float oldZoom = m_state.viewportZoom;
            m_state.viewportZoom += wheel * 0.1f * m_state.viewportZoom;
            if (m_state.viewportZoom < 0.1f) m_state.viewportZoom = 0.1f;
            if (m_state.viewportZoom > 10.0f) m_state.viewportZoom = 10.0f;

            ImVec2 mousePos = ImGui::GetMousePos();
            float localMouseX = mousePos.x - (vMin.x + baseOffset.x + m_state.viewportOffsetX);
            float localMouseY = mousePos.y - (vMin.y + baseOffset.y + m_state.viewportOffsetY);
            
            float zoomRatio = m_state.viewportZoom / oldZoom;
            m_state.viewportOffsetX -= localMouseX * (zoomRatio - 1.0f);
            m_state.viewportOffsetY -= localMouseY * (zoomRatio - 1.0f);
        }
    }

    ImVec2 pMin = ImVec2(vMin.x + baseOffset.x + m_state.viewportOffsetX, vMin.y + baseOffset.y + m_state.viewportOffsetY);
    ImVec2 pMax = ImVec2(pMin.x + m_runtimeRenderer->m_width * finalScale, pMin.y + m_runtimeRenderer->m_height * finalScale);

    drawList->AddImage((void*)(intptr_t)m_sceneTextureID, pMin, pMax);

    float displayGridSize = 16.0f * finalScale;

    // 무한 그리드 렌더링 (vMin ~ vMax)
    float startX = std::fmod(pMin.x - vMin.x, displayGridSize);
    if (startX < 0.0f) startX += displayGridSize;
    startX += vMin.x;

    float startY = std::fmod(pMin.y - vMin.y, displayGridSize);
    if (startY < 0.0f) startY += displayGridSize;
    startY += vMin.y;

    for (float x = startX; x <= vMax.x; x += displayGridSize)
        drawList->AddLine(ImVec2(x, vMin.y), ImVec2(x, vMax.y), IM_COL32(100, 100, 100, 50));
    for (float y = startY; y <= vMax.y; y += displayGridSize)
        drawList->AddLine(ImVec2(vMin.x, y), ImVec2(vMax.x, y), IM_COL32(100, 100, 100, 50));

    const auto& objects = m_scene->GetGameObjects();
    for (const auto& obj : objects)
    {
        ImVec2 pos = ImVec2(pMin.x + obj->GetX() * displayGridSize, pMin.y + obj->GetY() * displayGridSize);
        ImU32 color = IM_COL32(255, 255, 255, 255);
        if (m_state.selectedObjectID == obj->GetID())
            color = IM_COL32(255, 255, 0, 255);

        switch(obj->GetType()) {
            case EditorObjectType::Player: color = IM_COL32(0, 255, 0, 255); break;
            case EditorObjectType::Anchor: color = IM_COL32(0, 255, 255, 255); break;
            case EditorObjectType::Obstacle: color = IM_COL32(255, 0, 0, 255); break;
            case EditorObjectType::Item: color = IM_COL32(255, 255, 0, 255); break;
            case EditorObjectType::WindZone: color = IM_COL32(100, 100, 255, 100); break;
        }

        drawList->AddRectFilled(pos, ImVec2(pos.x + displayGridSize, pos.y + displayGridSize), color);
        drawList->AddRect(pos, ImVec2(pos.x + displayGridSize, pos.y + displayGridSize), IM_COL32(0,0,0,255));
    }

    if (ImGui::IsWindowHovered())
    {
        ImVec2 mousePos = ImGui::GetMousePos();
        float localX = mousePos.x - pMin.x;
        float localY = mousePos.y - pMin.y;
        int gridX = std::floor(localX / displayGridSize);
        int gridY = std::floor(localY / displayGridSize);

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            int clickedID = -1;
            for (auto it = objects.rbegin(); it != objects.rend(); ++it)
            {
                if (gridX == (*it)->GetX() && gridY == (*it)->GetY())
                {
                    clickedID = (*it)->GetID();
                    break;
                }
            }
            m_state.selectedObjectID = clickedID;
        }

        if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && m_state.selectedObjectID == -1)
        {
            if (ImGui::GetIO().KeyShift)
            {
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                {
                    auto obj = std::make_shared<EditorGameObject>(
                        0, 
                        (EditorObjectType)m_state.selectedObjectType, 
                        "PlayerSpawn"
                    );
                    obj->SetPosition(gridX, gridY);
                    m_scene->AddGameObject(obj);
                }
            }
            else
            {
                auto& brushes = m_scene->GetTileBrushes();
                if (m_state.selectedBrushIndex >= 0 && m_state.selectedBrushIndex < (int)brushes.size())
                {
                    auto& brush = brushes[m_state.selectedBrushIndex];
                    TileData tdata;
                    tdata.brushId = m_state.selectedBrushIndex;
                    tdata.type = brush.type;
                    tdata.hasCollider = brush.hasCollider;
                    tdata.zLayer = brush.zLayer;
                    tdata.sprite = GetOrLoadSprite(brush.spritePath);

                    m_scene->GetTileMap()->SetTile(gridX, gridY, tdata);
                }
            }
        }
        else if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) // Right click for remove tile
        {
            m_scene->GetTileMap()->RemoveTile(gridX, gridY);
        }

        if (m_state.selectedObjectID != -1 && ImGui::IsKeyPressed(ImGuiKey_Delete))
        {
            m_scene->RemoveGameObject(m_state.selectedObjectID);
            m_state.selectedObjectID = -1;
            m_commandMgr->RecordSnapshot(m_scene);
        }

        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) || ImGui::IsMouseReleased(ImGuiMouseButton_Right))
        {
            m_commandMgr->RecordSnapshot(m_scene);
        }
    }

    ImGui::End();
}

void EditorUIManager::ProcessShortcuts()
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S))
    {
        EditorSerializer::SaveToFile(m_scene, m_state.savePath);
    }
    if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Z))
    {
        m_commandMgr->Undo(m_scene);
    }
    if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Y))
    {
        m_commandMgr->Redo(m_scene);
    }
}

void EditorUIManager::RenderTilePalette()
{
    ImGui::Begin("Tile Palette");
    if (ImGui::BeginTabBar("PaletteTabs"))
    {
        if (ImGui::BeginTabItem("Tiles"))
        {
            if (ImGui::Button("Add New Brush"))
            {
                auto& brushes = m_scene->GetTileBrushes();
                EditorTileBrush newBrush;
                newBrush.name = "Brush_" + std::to_string(brushes.size());
                brushes.push_back(newBrush);
                m_state.selectedBrushIndex = (int)brushes.size() - 1;
            }

            ImGui::Separator();
            ImGui::Text("Brush List:");
            auto& brushes = m_scene->GetTileBrushes();
            for (size_t i = 0; i < brushes.size(); ++i)
            {
                bool isSelected = (m_state.selectedBrushIndex == (int)i);
                if (ImGui::Selectable(brushes[i].name.c_str(), isSelected))
                {
                    m_state.selectedBrushIndex = (int)i;
                }
            }

            ImGui::Separator();
            if (m_state.selectedBrushIndex >= 0 && m_state.selectedBrushIndex < (int)brushes.size())
            {
                EditorTileBrush& brush = brushes[m_state.selectedBrushIndex];
                
                char nameBuf[64];
                strncpy(nameBuf, brush.name.c_str(), sizeof(nameBuf));
                if (ImGui::InputText("Name", nameBuf, sizeof(nameBuf)))
                    brush.name = nameBuf;

                int typeInt = (int)brush.type;
                ImGui::RadioButton("Background", &typeInt, 0); ImGui::SameLine();
                ImGui::RadioButton("Wall", &typeInt, 1);
                brush.type = (TileType)typeInt;

                ImGui::Checkbox("Has Collider", &brush.hasCollider);
                ImGui::InputInt("Z Layer", &brush.zLayer);
                
                ImGui::InputText("Sprite Path", brush.spritePath, sizeof(brush.spritePath));
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_FILE_PATH"))
                    {
                        strncpy(brush.spritePath, (const char*)payload->Data, sizeof(brush.spritePath));
                        brush.spritePath[sizeof(brush.spritePath) - 1] = '\0';
                    }
                    ImGui::EndDragDropTarget();
                }
                ImGui::TextDisabled("(Drag and Drop image from Asset Browser)");
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Objects"))
        {
            ImGui::Text("Select Object Type to Place (Shift + Left Click in Scene):");
            ImGui::RadioButton("Player (Spawn)", &m_state.selectedObjectType, (int)EditorObjectType::Player);
            ImGui::TextDisabled("Other objects are disabled for now.");
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
    ImGui::End();
}

void EditorUIManager::RenderHierarchy()
{
    ImGui::Begin("Scene Hierarchy");
    const auto& objects = m_scene->GetGameObjects();
    ImGui::Text("Total Objects: %d", (int)objects.size());
    ImGui::Separator();
    for (const auto& obj : objects)
    {
        std::string label = "[" + std::to_string(obj->GetID()) + "] " + obj->GetName();
        bool isSelected = (m_state.selectedObjectID == obj->GetID());
        if (ImGui::Selectable(label.c_str(), isSelected))
        {
            m_state.selectedObjectID = obj->GetID();
        }
    }
    ImGui::End();
}

void EditorUIManager::RenderInspector()
{
    ImGui::Begin("Inspector");
    if (m_state.selectedObjectID != -1)
    {
        auto obj = m_scene->FindGameObject(m_state.selectedObjectID);
        if (obj)
        {
            ImGui::Text("Name: %s", obj->GetName().c_str());
            ImGui::Text("ID: %d", obj->GetID());
            ImGui::Text("Type: %d", (int)obj->GetType());
            
            float pos[2] = { obj->GetX(), obj->GetY() };
            if (ImGui::InputFloat2("Position", pos))
            {
                obj->SetPosition(pos[0], pos[1]);
            }

            ImGui::Separator();
            ImGui::Text("Custom Properties (TODO)");
        }
        else
        {
            ImGui::Text("Object not found.");
            m_state.selectedObjectID = -1;
        }
    }
    else
    {
        ImGui::Text("No object selected.");
    }
    ImGui::End();
}

void EditorUIManager::RenderAssetBrowser()
{
    ImGui::Begin("Asset Browser");
    
    ImGui::Columns(2, "AssetBrowserColumns");
    if (ImGui::GetColumnOffset(1) < 150.0f)
        ImGui::SetColumnWidth(0, 150.0f);

    ImGui::BeginChild("FolderTree");
    if (ImGui::CollapsingHeader("assets", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::Selectable("/", m_state.currentAssetDir == "assets"))
            m_state.currentAssetDir = "assets";
            
        if (fs::exists("assets"))
        {
            for (const auto& entry : fs::recursive_directory_iterator("assets"))
            {
                if (entry.is_directory())
                {
                    std::string dirPath = entry.path().string();
                    std::replace(dirPath.begin(), dirPath.end(), '\\', '/');
                    std::string dirName = entry.path().filename().string();
                    
                    int depth = std::count(dirPath.begin(), dirPath.end(), '/') - 1;
                    if (depth < 0) depth = 0;
                    std::string indent(depth * 2, ' ');
                    std::string label = indent + "📁 " + dirName;
                    
                    if (ImGui::Selectable(label.c_str(), m_state.currentAssetDir == dirPath))
                    {
                        m_state.currentAssetDir = dirPath;
                    }
                }
            }
        }
    }
    ImGui::EndChild();

    ImGui::NextColumn();

    ImGui::BeginChild("FileGrid");
    if (fs::exists(m_state.currentAssetDir))
    {
        float windowVisibleX2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
        ImVec2 buttonSize(64, 64);
        ImGuiStyle& style = ImGui::GetStyle();

        for (const auto& entry : fs::directory_iterator(m_state.currentAssetDir))
        {
            if (entry.is_regular_file())
            {
                std::string pathString = entry.path().string();
                std::replace(pathString.begin(), pathString.end(), '\\', '/');
                std::string filename = entry.path().filename().string();
                std::string ext = entry.path().extension().string();

                ImGui::PushID(filename.c_str());
                ImGui::BeginGroup();

                bool isImage = (ext == ".png" || ext == ".jpg" || ext == ".sprite");
                unsigned int glID = 0;
                ImVec2 uv0(0, 0), uv1(1, 1);
                
                if (isImage)
                {
                    Sprite* spr = GetOrLoadSprite(pathString);
                    if (spr && spr->texture)
                    {
                        glID = GetOrCreateGLTexture(spr->texture);
                        if (glID != 0)
                        {
                            uv0 = ImVec2((float)spr->srcX / spr->texture->GetWidth(), (float)spr->srcY / spr->texture->GetHeight());
                            uv1 = ImVec2((float)(spr->srcX + spr->width) / spr->texture->GetWidth(), (float)(spr->srcY + spr->height) / spr->texture->GetHeight());
                        }
                    }
                }

                if (glID != 0)
                {
                    if (ImGui::ImageButton(filename.c_str(), (ImTextureID)(intptr_t)glID, buttonSize, uv0, uv1)) {}
                }
                else
                {
                    if (ImGui::Button(isImage ? "IMG" : "FILE", buttonSize)) {}
                }

                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                {
                    if (isImage)
                    {
                        m_state.showSpriteEditor = true;
                        m_state.spriteEditorTarget = pathString;
                    }
                }

                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
                {
                    char payloadData[256];
                    strncpy(payloadData, pathString.c_str(), sizeof(payloadData) - 1);
                    payloadData[sizeof(payloadData) - 1] = '\0';
                    
                    ImGui::SetDragDropPayload("ASSET_FILE_PATH", payloadData, sizeof(payloadData));
                    ImGui::Text("Dragging %s", filename.c_str());
                    ImGui::EndDragDropSource();
                }

                std::string label = filename;
                if (label.length() > 10) label = label.substr(0, 8) + "..";
                ImGui::TextWrapped("%s", label.c_str());

                ImGui::EndGroup();
                ImGui::PopID();

                float lastButtonX2 = ImGui::GetItemRectMax().x;
                float nextButtonX2 = lastButtonX2 + style.ItemSpacing.x + buttonSize.x;
                if (nextButtonX2 < windowVisibleX2)
                    ImGui::SameLine();
            }
        }
    }
    ImGui::EndChild();

    ImGui::Columns(1);
    ImGui::End();
}

void EditorUIManager::RenderSpriteEditor()
{
    if (!m_state.showSpriteEditor) return;

    ImGui::SetNextWindowSize(ImVec2(600, 500), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Sprite Editor", &m_state.showSpriteEditor))
    {
        ImGui::Text("Target: %s", m_state.spriteEditorTarget.c_str());
        Sprite* spr = GetOrLoadSprite(m_state.spriteEditorTarget);
        if (!spr || !spr->texture)
        {
            ImGui::TextColored(ImVec4(1,0,0,1), "Failed to load sprite.");
            ImGui::End();
            return;
        }

        ImGui::Separator();
        
        ImGui::InputInt("Grid Width", &m_state.sliceGridX);
        ImGui::InputInt("Grid Height", &m_state.sliceGridY);
        
        if (m_state.sliceGridX < 1) m_state.sliceGridX = 1;
        if (m_state.sliceGridY < 1) m_state.sliceGridY = 1;

        if (ImGui::Button("Auto Slice & Generate"))
        {
            int texW = spr->texture->GetWidth();
            int texH = spr->texture->GetHeight();
            int cols = texW / m_state.sliceGridX;
            int rows = texH / m_state.sliceGridY;

            std::string baseName = fs::path(m_state.spriteEditorTarget).stem().string();
            std::string dirPath = fs::path(m_state.spriteEditorTarget).parent_path().string();

            int count = 0;
            for (int y = 0; y < rows; ++y)
            {
                for (int x = 0; x < cols; ++x)
                {
                    std::string outPath = dirPath + "/" + baseName + "_" + std::to_string(count) + ".sprite";
                    std::ofstream ofs(outPath);
                    if (ofs.is_open())
                    {
                        ofs << "Texture=" << m_state.spriteEditorTarget << "\n";
                        ofs << "SrcX=" << (x * m_state.sliceGridX) << "\n";
                        ofs << "SrcY=" << (y * m_state.sliceGridY) << "\n";
                        ofs << "Width=" << m_state.sliceGridX << "\n";
                        ofs << "Height=" << m_state.sliceGridY << "\n";
                        ofs.close();
                    }
                    count++;
                }
            }
            ImGui::OpenPopup("Slice Complete");
        }

        if (ImGui::BeginPopupModal("Slice Complete", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Slicing finished. Sprites generated.");
            if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }

        ImGui::Separator();
        
        ImGui::Text("Preview Image (%d x %d)", spr->texture->GetWidth(), spr->texture->GetHeight());
        
        unsigned int glID = GetOrCreateGLTexture(spr->texture);

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 p = ImGui::GetCursorScreenPos();
        float scale = 1.0f;
        ImVec2 size(spr->texture->GetWidth() * scale, spr->texture->GetHeight() * scale);
        
        if (glID != 0) {
            draw_list->AddImage((void*)(intptr_t)glID, p, ImVec2(p.x + size.x, p.y + size.y));
        } else {
            draw_list->AddRectFilled(p, ImVec2(p.x + size.x, p.y + size.y), IM_COL32(50, 50, 50, 255));
        }
        
        int cols = spr->texture->GetWidth() / m_state.sliceGridX;
        int rows = spr->texture->GetHeight() / m_state.sliceGridY;

        for (int y = 0; y <= rows; ++y)
        {
            float yy = p.y + y * m_state.sliceGridY * scale;
            draw_list->AddLine(ImVec2(p.x, yy), ImVec2(p.x + size.x, yy), IM_COL32(255, 0, 0, 255));
        }
        for (int x = 0; x <= cols; ++x)
        {
            float xx = p.x + x * m_state.sliceGridX * scale;
            draw_list->AddLine(ImVec2(xx, p.y), ImVec2(xx, p.y + size.y), IM_COL32(255, 0, 0, 255));
        }
        ImGui::Dummy(size);
    }
    ImGui::End();
}
