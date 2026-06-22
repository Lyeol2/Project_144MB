#include "Time.h"
#include <windows.h>

double Time::s_secondsPerCount = 0.0;
long long Time::s_startTime = 0;
long long Time::s_prevTime = 0;
float Time::s_deltaTime = 0.0f;
float Time::s_totalTime = 0.0f;

void Time::Initialize()
{
	long long countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	s_secondsPerCount = 1.0 / (double)countsPerSec;

	QueryPerformanceCounter((LARGE_INTEGER*)&s_startTime);
	s_prevTime = s_startTime;
}

void Time::Update()
{
	long long currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	s_deltaTime = (float)((currTime - s_prevTime) * s_secondsPerCount);
	s_prevTime = currTime;

	// 프레임 스파이크 방지 (예: 디버깅 중 브레이크포인트에 걸렸을 때)
	if (s_deltaTime < 0.0f)
	{
		s_deltaTime = 0.0f;
	}
	
	s_totalTime = (float)((currTime - s_startTime) * s_secondsPerCount);
}
