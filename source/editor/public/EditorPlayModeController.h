#pragma once

/// @brief 에디터의 Edit / Play 상태를 전환하고 관리하는 컨트롤러
class EditorPlayModeController
{
public:
    EditorPlayModeController();
    ~EditorPlayModeController() = default;

    /// @brief Play 모드로 전환합니다.
    void StartPlayMode();

    /// @brief Edit 모드로 전환합니다.
    void StopPlayMode();

    /// @brief 현재 Play 모드인지 확인합니다.
    bool IsPlaying() const;

private:
    bool m_isPlaying;
    void* m_processHandle; // HANDLE to the launcher process
};
