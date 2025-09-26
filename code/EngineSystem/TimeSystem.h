#pragma once

#include <Windows.h>

class Time {
    static DWORD m_DeltaTime; // 前フレームからの経過時間（ミリ秒）
    static DWORD m_ElapsedTime; // 起動からの累積時間（ミリ秒）
    static DWORD m_LastTime; // 前フレーム実行時のの時間（ミリ秒）
    static DWORD m_CurrentTime; // 今フレーム実行時の時間（ミリ秒）

    static int m_FramePerSecond;

    static float m_TimeScale;

public:
    void Update();

    static DWORD DeltaTime() { return m_DeltaTime; }
    static DWORD ElapsedTime() { return m_ElapsedTime; }
    static int FramePerSecond() { return m_FramePerSecond; }
    static void SetTimeScale(float scale) { m_TimeScale = scale; }
};
