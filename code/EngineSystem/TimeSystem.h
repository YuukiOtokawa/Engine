#pragma once

#include <Windows.h>

#include <vector>

class Time {

    static DWORD m_FrameCount; // フレームカウント

    static int m_FramePerSecond;

    static float m_TimeScale;

    static std::vector<float> m_FPSHistory; // 過去のFPS履歴を保存するベクター

    static constexpr int FPS_HISTORY_SIZE = 120; // 保存するFPS履歴の最大数

public:
    static DWORD deltaTime; // 前フレームからの経過時間（ミリ秒）
    static DWORD elapsedTime; // 起動からの累積時間（ミリ秒）
    static DWORD lastTime; // 前フレーム実行時のの時間（ミリ秒）
    static DWORD currentTime; // 今フレーム実行時の時間（ミリ秒）


    void Update();
    void FrameUpdate();

    void Reset();

    static DWORD DeltaTime() { return deltaTime; }
    static DWORD ElapsedTime() { return elapsedTime; }
    static int FramePerSecond() { return m_FramePerSecond; }
    static void SetTimeScale(float scale) { m_TimeScale = scale; }

    static const std::vector<float>& GetFPSHistory() { return m_FPSHistory; }

    static void DrawFPSGraph();
};
