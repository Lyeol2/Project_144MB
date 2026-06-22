#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture()
	: m_width(0), m_height(0), m_channels(0), m_pixels(nullptr)
{
}

Texture::~Texture()
{
	if (m_pixels != nullptr)
	{
		stbi_image_free(m_pixels);
		m_pixels = nullptr;
	}
}

bool Texture::LoadFromFile(const std::string& filepath)
{
	if (m_pixels != nullptr)
	{
		stbi_image_free(m_pixels);
		m_pixels = nullptr;
	}

	// 윈도우 비트맵은 보통 4채널(RGBA가 아닌 BGRA)을 요구하므로, 
	// 나중에 렌더러에서 변환하거나 여기서 변환해야 할 수도 있습니다.
	// 우선은 원본 이미지 그대로 파싱(4채널 강제)합니다.
	m_pixels = stbi_load(filepath.c_str(), &m_width, &m_height, &m_channels, 4);
	if (m_pixels == nullptr)
	{
		return false;
	}
	
	// 강제로 4채널로 읽어왔으므로 채널 수는 4로 고정
	m_channels = 4;

	// RGBA -> BGRA 변환 (WINAPI 호환을 위해)
	for (int i = 0; i < m_width * m_height * 4; i += 4)
	{
		unsigned char r = m_pixels[i + 0];
		unsigned char b = m_pixels[i + 2];
		m_pixels[i + 0] = b;
		m_pixels[i + 2] = r;
	}

	return true;
}
