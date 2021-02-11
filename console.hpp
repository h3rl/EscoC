#pragma once
#include "header.h"
#include <Windows.h>
#include <string>

using namespace std;

void setcolor(int i)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), i);
}

string stringifyStance(int stance) {
    switch (stance)
    {
    case STANDING:
        return "|";
        break;
    case CROUCH:
        return "/";
        break;
    case PRONE:
        return "_";
        break;
    default:
        return "STANCEERROR";
        break;
    }
}

string stringifyProfile(int profile) {
    switch (profile)
    {
    case LIGHT:
        return "light";
        break;
    case HEAVY:
        return "Heavy";
        break;
    default:
        return "";
        break;
    }
}

string stringifySlot(int slot) {
    switch (slot)
    {
    case SLOT1:
        return "1";
        break;
    case SLOT2:
        return "2";
        break;
    case NOSLOT:
        return "";
        break;
    default:
        return "ERROR";
        break;
    }
}

string b2s(bool b) { if (b)return "True"; return "False"; }
// https://docs.microsoft.com/en-us/windows/desktop/inputdev/virtual-key-codes

void exit_(string msg = "", int exitcode = 0) {
    if (keyboardHook)
        UnhookWindowsHookEx(keyboardHook);
    if (msg != "")
        _L(msg);
    if (exitcode == 0) {
        system("pause");
    }
    ExitProcess(0);
}
