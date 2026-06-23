#pragma once
#include <string>

/// @brief 에디터에 배치되는 오브젝트의 종류를 나타내는 열거형
enum class EditorObjectType
{
    Player,
    Anchor,
    Obstacle,
    Item,
    WindZone
};

/// @brief 에디터 상에 배치되는 모든 오브젝트의 베이스 클래스
class EditorGameObject
{
public:
    EditorGameObject(int id, EditorObjectType type, const std::string& name);
    virtual ~EditorGameObject() = default;

    /// @brief 오브젝트의 X, Y 좌표를 설정합니다.
    /// @param x X 좌표
    /// @param y Y 좌표
    void SetPosition(float x, float y);
    
    float GetX() const;
    float GetY() const;
    int GetID() const;
    EditorObjectType GetType() const;
    std::string GetName() const;

    /// @brief 이 오브젝트의 데이터를 문자열 형태로 직렬화합니다.
    /// @return "ID,Type,Name,X,Y" 형태의 콤마 분리 문자열
    virtual std::string SerializeToString() const;
    
    /// @brief 직렬화된 문자열을 파싱하여 속성을 복원합니다.
    /// @param data 콤마로 분리된 문자열 데이터
    virtual void DeserializeFromString(const std::string& data);

protected:
    int m_id;
    EditorObjectType m_type;
    std::string m_name;
    float m_positionX;
    float m_positionY;
};
