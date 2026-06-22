#include "Input.h"
#include <windows.h>

bool Input::s_currentKeys[KEY_COUNT];
bool Input::s_previousKeys[KEY_COUNT];

void Input::Initialize()
{
	for (int i = 0; i < KEY_COUNT; ++i)
	{
		s_currentKeys[i] = false;
		s_previousKeys[i] = false;
	}
}

void Input::Update()
{
	for (int i = 0; i < KEY_COUNT; ++i)
	{
		s_previousKeys[i] = s_currentKeys[i];
		
		// GetAsyncKeyState returns short. Most significant bit is 1 if key is down.
		s_currentKeys[i] = (GetAsyncKeyState(i) & 0x8000) != 0;
	}
}

bool Input::GetKey(int keyCode)
{
	if (keyCode < 0 || keyCode >= KEY_COUNT) return false;
	return s_currentKeys[keyCode];
}

bool Input::GetKeyDown(int keyCode)
{
	if (keyCode < 0 || keyCode >= KEY_COUNT) return false;
	return s_currentKeys[keyCode] && !s_previousKeys[keyCode];
}

bool Input::GetKeyUp(int keyCode)
{
	if (keyCode < 0 || keyCode >= KEY_COUNT) return false;
	return !s_currentKeys[keyCode] && s_previousKeys[keyCode];
}
