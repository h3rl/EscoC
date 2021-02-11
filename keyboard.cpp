#include <Windows.h>
#include <thread>
#include "keyboard.h"

void key_press(int vkey, int delay_ms) {
	INPUT ip;
	ip.type = INPUT_KEYBOARD;
	ip.ki.wVk = vkey;
	SendInput(1, &ip, sizeof(INPUT)); //pressing key
	std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT)); //releasing key
}