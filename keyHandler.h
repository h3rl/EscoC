#pragma once
#include <Windows.h>
struct Key {
	bool pressed = false;
};

class KeyHandler
{
public:
	KeyHandler();
	~KeyHandler();
	void setup();
	LRESULT CALLBACK callBackFnc(int nCode, WPARAM wParam, LPARAM lParam);
private:
	HHOOK keyboardHook;
};

KeyHandler::KeyHandler()
{

}

KeyHandler::~KeyHandler()
{

}

void KeyHandler::setup()
{
	keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, callBackFnc, NULL, NULL);
	MSG msg{ 0 };
	//our application loop
	while (GetMessage(&msg, NULL, 0, 0) != 0);
	UnhookWindowsHookEx(keyboardHook);
}

LRESULT CALLBACK KeyHandler::callBackFnc(int nCode, WPARAM wParam, LPARAM lParam)
{
	PKBDLLHOOKSTRUCT key = (PKBDLLHOOKSTRUCT)lParam;
	//a key was pressed
	/*
	if (wParam == WM_KEYDOWN && nCode == HC_ACTION)
	{
		std::cout << key->vkCode << std::endl;
	}
	*/
	if (nCode == HC_ACTION) {
		LPKBDLLHOOKSTRUCT key = (LPKBDLLHOOKSTRUCT)lParam;

		if ((key->flags >> LLKHF_INJECTED) & 1) {
			// ignore injected events
		}
		else {
			//short state = KEYUNDEF;
			DWORD vEventKey = key->vkCode;
			switch (wParam)
			{
			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:
				//state = KEYDOWN;
				_D(vEventKey << " Down" << std::endl);
				break;
			case WM_KEYUP:
			case WM_SYSKEYUP:
				_D(vEventKey << " Up" << std::endl);
				//state = KEYUP;
			default:
				break;
			}
		}
	}
	return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}