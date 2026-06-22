#pragma once

class Input
{
public:
	static void Initialize();
	static void Update();

	// 특정 키가 현재 눌려있는지 여부
	static bool GetKey(int keyCode);
	
	// 이번 프레임에 특정 키가 새로 눌렸는지 여부
	static bool GetKeyDown(int keyCode);
	
	// 이번 프레임에 특정 키가 떼어졌는지 여부
	static bool GetKeyUp(int keyCode);

private:
	static const int KEY_COUNT = 256;
	static bool s_currentKeys[KEY_COUNT];
	static bool s_previousKeys[KEY_COUNT];
};
