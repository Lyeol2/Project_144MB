#pragma once
#include "EditorScene.h"
#include <string>
#include <memory>

/// @brief 에디터 씬을 인게임 클라이언트가 읽을 수 있는 형태로 최적화하여 빌드/익스포트하는 클래스
class EditorBuildExporter
{
public:
    EditorBuildExporter() = default;
    ~EditorBuildExporter() = default;

    /// @brief 씬을 게임용 데이터로 익스포트합니다.
    /// @param scene 대상 씬
    /// @param outPath 저장할 파일 경로
    /// @return 성공 여부
    bool ExportSceneForGame(const std::shared_ptr<EditorScene>& scene, const std::string& outPath) const;
};
