#include "TimeSystem.h"

DWORD Time::m_DeltaTime = 0; // 前フレームからの経過時間（ミリ秒）
DWORD Time::m_ElapsedTime = 0; // 起動からの累積時間（ミリ秒）
DWORD Time::m_LastTime = 0; // 前フレーム実行時のの時間（ミリ秒）
DWORD Time::m_CurrentTime = 0; // 今フレーム実行時の時間（ミリ秒）
int Time::m_FramePerSecond = 0;
float Time::m_TimeScale = 1.0f;

void Time::Update()
{
	m_CurrentTime = timeGetTime();
	m_DeltaTime = m_CurrentTime - m_LastTime;
	m_ElapsedTime += m_DeltaTime;
	m_LastTime = m_CurrentTime;
	m_FramePerSecond = 1000 / m_DeltaTime;
}