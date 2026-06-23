#include "Texture.h"
#include <windows.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

// GDI+ 초기화를 위한 정적 헬퍼 클래스
struct GdiplusInitializer
{
    ULONG_PTR token;
    GdiplusInitializer()
    {
        GdiplusStartupInput input;
        GdiplusStartup(&token, &input, nullptr);
    }
    ~GdiplusInitializer()
    {
        GdiplusShutdown(token);
    }
};
static GdiplusInitializer g_gdiplusInit;

Texture::Texture()
    : m_width(0), m_height(0), m_channels(0), m_pixels(nullptr)
{
}

Texture::~Texture()
{
    if (m_pixels != nullptr)
    {
        delete[] m_pixels;
        m_pixels = nullptr;
    }
}

bool Texture::LoadFromFile(const std::string& filepath)
{
    if (m_pixels != nullptr)
    {
        delete[] m_pixels;
        m_pixels = nullptr;
    }

    // std::string을 std::wstring으로 변환 (GDI+는 유니코드 경로 필요)
    int len = MultiByteToWideChar(CP_UTF8, 0, filepath.c_str(), -1, nullptr, 0);
    std::wstring wpath(len, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, filepath.c_str(), -1, &wpath[0], len);

    Bitmap* bitmap = Bitmap::FromFile(wpath.c_str());
    if (!bitmap || bitmap->GetLastStatus() != Ok)
    {
        if (bitmap) delete bitmap;
        return false;
    }

    m_width = bitmap->GetWidth();
    m_height = bitmap->GetHeight();
    m_channels = 4; // 강제로 32bpp(BGRA) 4채널 사용

    m_pixels = new unsigned char[m_width * m_height * 4];

    Rect rect(0, 0, m_width, m_height);
    BitmapData bmpData;
    // PixelFormat32bppARGB는 메모리 상에서 BGRA 순서로 배치됩니다. (윈도우 기본)
    if (bitmap->LockBits(&rect, ImageLockModeRead, PixelFormat32bppARGB, &bmpData) == Ok)
    {
        unsigned char* src = (unsigned char*)bmpData.Scan0;
        unsigned char* dst = m_pixels;

        for (int y = 0; y < m_height; ++y)
        {
            memcpy(dst + y * m_width * 4, src + y * bmpData.Stride, m_width * 4);
        }
        bitmap->UnlockBits(&bmpData);
    }
    else
    {
        delete bitmap;
        return false;
    }

    delete bitmap;
    return true;
}
