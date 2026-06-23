#pragma once

class Time
{
public:
	static void Initialize();
	static void Update();

	static float GetDeltaTime() { return s_deltaTime; }
	static float GetTotalTime() { return s_totalTime; }

private:
	static double s_secondsPerCount;
	static long long s_startTime;
	static long long s_prevTime;

	static float s_deltaTime;
	static float s_totalTime;
};
