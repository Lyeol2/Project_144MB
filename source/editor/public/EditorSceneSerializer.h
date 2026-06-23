#pragma once
#include "EditorScene.h"
#include <string>
#include <memory>

/// @brief 외부 의존성 없이 커스텀 텍스트 파서를 통해 씬 데이터를 저장/불러오기 하는 클래스
class EditorSceneSerializer
{
public:
    EditorSceneSerializer() = default;
    ~EditorSceneSerializer() = default;

    /// @brief 씬 객체를 문자열로 직렬화합니다.
    /// @param scene 직렬화할 씬 객체
    /// @return 직렬화된 텍스트 데이터
    std::string Serialize(const std::shared_ptr<EditorScene>& scene) const;

    /// @brief 문자열 데이터를 파싱하여 씬 객체에 복원합니다.
    /// @param data 복원할 텍스트 데이터
    /// @param scene 데이터를 담을 씬 객체
    /// @return 성공 여부
    bool Deserialize(const std::string& data, std::shared_ptr<EditorScene>& scene) const;
};
