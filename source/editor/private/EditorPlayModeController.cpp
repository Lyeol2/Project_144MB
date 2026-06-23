#include "EditorPlayModeController.h"
#include <windows.h>
#include <iostream>

EditorPlayModeController::EditorPlayModeController()
    : m_isPlaying(false), m_processHandle(nullptr)
{
}

void EditorPlayModeController::StartPlayMode()
{
    if (m_isPlaying) return;
    
    // TODO: 현재 씬 저장 (임시 파일)

    STARTUPINFOW si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // launcher.exe 실행
    wchar_t cmd[] = L"launcher.exe";
    if (CreateProcessW(NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        m_isPlaying = true;
        m_processHandle = pi.hProcess;
        CloseHandle(pi.hThread);
    }
}

void EditorPlayModeController::StopPlayMode()
{
    if (!m_isPlaying) return;

    if (m_processHandle)
    {
        TerminateProcess((HANDLE)m_processHandle, 0);
        CloseHandle((HANDLE)m_processHandle);
        m_processHandle = nullptr;
    }
    
    m_isPlaying = false;
}

bool EditorPlayModeController::IsPlaying() const
{
    return m_isPlaying;
}
