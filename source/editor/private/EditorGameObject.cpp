#include "EditorGameObject.h"
#include <sstream>
#include <vector>

EditorGameObject::EditorGameObject(int id, EditorObjectType type, const std::string& name)
    : m_id(id), m_type(type), m_name(name), m_positionX(0.0f), m_positionY(0.0f)
{
}

void EditorGameObject::SetPosition(float x, float y)
{
    m_positionX = x;
    m_positionY = y;
}

float EditorGameObject::GetX() const { return m_positionX; }
float EditorGameObject::GetY() const { return m_positionY; }
int EditorGameObject::GetID() const { return m_id; }
EditorObjectType EditorGameObject::GetType() const { return m_type; }
std::string EditorGameObject::GetName() const { return m_name; }

std::string EditorGameObject::SerializeToString() const
{
    std::stringstream ss;
    ss << m_id << "," 
       << static_cast<int>(m_type) << "," 
       << m_name << "," 
       << m_positionX << "," 
       << m_positionY;
    return ss.str();
}

void EditorGameObject::DeserializeFromString(const std::string& data)
{
    std::stringstream ss(data);
    std::string token;
    std::vector<std::string> tokens;
    
    while (std::getline(ss, token, ','))
    {
        tokens.push_back(token);
    }

    if (tokens.size() >= 5)
    {
        m_id = std::stoi(tokens[0]);
        m_type = static_cast<EditorObjectType>(std::stoi(tokens[1]));
        m_name = tokens[2];
        m_positionX = std::stof(tokens[3]);
        m_positionY = std::stof(tokens[4]);
    }
}
