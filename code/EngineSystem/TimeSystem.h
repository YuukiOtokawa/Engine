#pragma once

#include <Windows.h>

#include <vector>

#include <chrono>
#include <thread>

class Time {

    static DWORD m_FrameCount; // フレームカウント

    static float m_FramePerSecond;

    static float m_TimeScale;

    static std::vector<float> m_FPSHistory; // 過去のFPS履歴を保存するベクター

    static constexpr int FPS_HISTORY_SIZE = 120; // 保存するFPS履歴の最大数


public:
    static constexpr int FRAME_PER_SECOND = 60;

    static std::chrono::duration<double> targetFrameDuration;

    static std::chrono::duration<double, std::micro> deltaTime; // 前フレームからの経過時間（マイクロ秒）
    static std::chrono::duration<double, std::micro> elapsedTime; // 起動からの累積時間（マイクロ秒）
    static std::chrono::high_resolution_clock::time_point lastFrameTime; // 前フレーム実行時のの時間（ミリ秒）
    static std::chrono::high_resolution_clock::time_point lastSystemTime; // 前システム情報取得時のシステム時間（ミリ秒）
    static std::chrono::high_resolution_clock::time_point currentTime; // 今フレーム実行時の時間（ミリ秒）

    Time();

    void Update();
    void FrameUpdate();
    void SystemTimeUpdate();

    void Reset();

    static double DeltaTime() { return deltaTime.count() / 1000000; }
    static double ElapsedTime() { return elapsedTime.count() / 1000000; }
    static float FramePerSecond() { return m_FramePerSecond; }
    static void SetTimeScale(float scale) { m_TimeScale = scale; }

    static const std::vector<float>& GetFPSHistory() { return m_FPSHistory; }

    static void DrawFPSGraph();
};
