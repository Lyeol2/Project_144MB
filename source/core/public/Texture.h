#pragma once
#include <string>

class Texture
{
public:
	Texture();
	~Texture();

	bool LoadFromFile(const std::string& filepath);

	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }
	int GetChannels() const { return m_channels; }
	
	// WINAPI에서 사용할 수 있는 BGRA 형식의 픽셀 데이터 (또는 stb_image 기본 로딩 결과물)
	unsigned char* GetPixels() const { return m_pixels; }

private:
	int m_width;
	int m_height;
	int m_channels;
	unsigned char* m_pixels;
};
