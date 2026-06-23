#include "Renderer.h"
#include "Sprite.h"
#include <memory.h>

Renderer::Renderer(int width, int height)
{
	m_screenbuffer[0] = nullptr;
	m_screenbuffer[1] = nullptr;
	Initialize(width, height);
}

Renderer::~Renderer()
{
	if (m_screenbuffer[0]) delete[] m_screenbuffer[0];
	if (m_screenbuffer[1]) delete[] m_screenbuffer[1];
}

bool Renderer::Initialize(int width, int height)
{
	if (m_screenbuffer[0]) delete[] m_screenbuffer[0];
	if (m_screenbuffer[1]) delete[] m_screenbuffer[1];

	m_width = width;
	m_height = height;
	m_backBufferIndex = 0;

	int pixelCount = width * height;
	m_screenbuffer[0] = new unsigned int[pixelCount];
	m_screenbuffer[1] = new unsigned int[pixelCount];

	Clear();
	return true;
}

void Renderer::Clear(unsigned int color)
{
	unsigned int* buffer = m_screenbuffer[m_backBufferIndex];
	int pixelCount = m_width * m_height;
	for (int i = 0; i < pixelCount; ++i)
	{
		buffer[i] = color;
	}
}

void Renderer::DrawSprite(int x, int y, Sprite* sprite)
{
	if (!sprite || !sprite->texture || !sprite->texture->GetPixels()) return;

	unsigned int* buffer = m_screenbuffer[m_backBufferIndex];
	unsigned int* texPixels = (unsigned int*)sprite->texture->GetPixels();

	int texWidth = sprite->texture->GetWidth();
	int texHeight = sprite->texture->GetHeight();
    // 그려야할 실제 너비와 높이 (현재 1:1 스케일 기준)
    int drawWidth = sprite->width > 0 ? sprite->width : texWidth;
    int drawHeight = sprite->height > 0 ? sprite->height : texHeight;
    
    int srcX = sprite->srcX;
    int srcY = sprite->srcY;

    for (int dy = 0; dy < drawHeight; ++dy)
    {
        int destY = y - m_cameraY + dy;
        if (destY < 0 || destY >= m_height) continue;

        int sy = srcY + dy;
        if (sy < 0 || sy >= texHeight) continue;

        for (int dx = 0; dx < drawWidth; ++dx)
        {
            int destX = x - m_cameraX + dx;
            if (destX < 0 || destX >= m_width) continue;

            int sx = srcX + dx;
            if (sx < 0 || sx >= texWidth) continue;

            unsigned int color = texPixels[sy * texWidth + sx];
            // Alpha Test (투명도 0인 픽셀은 건너뜀) - BGRA 포맷 기준
            if ((color & 0xFF000000) != 0) 
            {
                buffer[destY * m_width + destX] = color;
            }
        }
    }
}

void Renderer::Present(HWND hwnd)
{
	if (!hwnd) return;
	
	HDC hdc = GetDC(hwnd);
	
	RECT clientRect;
	GetClientRect(hwnd, &clientRect);
	int clientWidth = clientRect.right - clientRect.left;
	int clientHeight = clientRect.bottom - clientRect.top;

	BITMAPINFO bmi = { 0 };
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = m_width;
	bmi.bmiHeader.biHeight = -m_height; // 위에서 아래로(Top-Down) DIB
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	unsigned int* buffer = m_screenbuffer[m_backBufferIndex];

    int scaleX = clientWidth / m_width;
    int scaleY = clientHeight / m_height;
    int scale = (scaleX < scaleY) ? scaleX : scaleY;
    if (scale < 1) scale = 1;

    int destWidth = m_width * scale;
    int destHeight = m_height * scale;
    int destX = (clientWidth - destWidth) / 2;
    int destY = (clientHeight - destHeight) / 2;

    int oldMode = SetStretchBltMode(hdc, COLORONCOLOR);

    // Clear letterbox/pillarbox areas
    HBRUSH bgBrush = CreateSolidBrush(RGB(0, 0, 0));
    if (destY > 0) {
        RECT topRect = { 0, 0, clientWidth, destY };
        FillRect(hdc, &topRect, bgBrush);
        RECT bottomRect = { 0, destY + destHeight, clientWidth, clientHeight };
        FillRect(hdc, &bottomRect, bgBrush);
    }
    if (destX > 0) {
        RECT leftRect = { 0, destY, destX, destY + destHeight };
        FillRect(hdc, &leftRect, bgBrush);
        RECT rightRect = { destX + destWidth, destY, clientWidth, destY + destHeight };
        FillRect(hdc, &rightRect, bgBrush);
    }
    DeleteObject(bgBrush);

	StretchDIBits(
		hdc,
		destX, destY, destWidth, destHeight,
		0, 0, m_width, m_height,
		buffer,
		&bmi,
		DIB_RGB_COLORS,
		SRCCOPY
	);

    SetStretchBltMode(hdc, oldMode);

	ReleaseDC(hwnd, hdc);
}
