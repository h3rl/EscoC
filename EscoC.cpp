/****************************************
 * All credit goes to Whatisthis
 ****************************************/
#include "header.h"
#include "console.hpp"
#include "input.hpp"
#include "config.hpp"

#include "nlohmann/json.hpp"

using namespace std;
using namespace input;

int stance = STANDING;
int profile[2] = { NOPROFILE,NOPROFILE };

int slot = SLOT1;
int lastSlot = NOSLOT;

int force = 0;
int delay = 0;

bool update = true;
bool bNoUpdate = false;
Config* cfg = new Config();


void printStatus() //gui
{
    system("CLS");
    _L("           -ESCO_C-");
    _L("-----------(Status)------------");
    _L("Stance:  " << stringifyStance(stance));
    _L("Force:   " << force);
    _L("Delay:   " << delay);
    _L("-------------------------------");
    _L("Slot:    " << stringifySlot(slot));
    if (slot == NOSLOT) {
        _L("Profile:");
    }
    else {
        _L("Profile: " << stringifyProfile(profile[slot]));
    }
    _L("----------(Keybinds)-----------");
    _L("home: reload config.json");
    _L("-------------------------------");
}

//vector<vector<short>>* table = &recoilTable[profile][stance];
void __stdcall recoilThread()
{
    for (;;)
    {
        if(GetAsyncKeyState(VK_LBUTTON) && GetAsyncKeyState(VK_RBUTTON) && slot != NOSLOT && profile[slot] != NOPROFILE)
        {
            do {
                force = max(cfg->table[profile[slot]][FORCE][stance], 0);
                delay = max(cfg->table[profile[slot]][DELAY][stance], 1);//never under 1

                input::move(0, force);
                std::this_thread::sleep_for(std::chrono::milliseconds(delay));
            } while (GetAsyncKeyState(VK_LBUTTON) && slot != NOSLOT && profile[slot] != NOPROFILE);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

/// <summary>
/// KEYBOARD HANDLER AND THREAD
/// </summary>

HHOOK keyboardHook;

static std::vector<int> monitoredKeys = {
    VK_SPACE,VK_KEY_Z,VK_KEY_C, //STANCEKEYS
    VK_KEY_1,VK_KEY_2,VK_KEY_X, //SLOTKEYS
    VK_F20,VK_F21,VK_F22, //MOUSEKEYS mapped
    VK_KEY_G,VK_KEY_H,VK_KEY_J,
    VK_DIVIDE,VK_HOME
};
std::map<int, bool> keys;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION) {
        LPKBDLLHOOKSTRUCT keyStruct = (LPKBDLLHOOKSTRUCT)lParam;

        if ((keyStruct->flags >> LLKHF_INJECTED) & 1) {
            // ignore injected events
        }
        else {
            DWORD key = keyStruct->vkCode;
            if (std::find(monitoredKeys.begin(), monitoredKeys.end(), key) != monitoredKeys.end()) {//check if key is monitored
                switch (wParam)
                {
                case WM_KEYDOWN:
                case WM_SYSKEYDOWN: {
                    if (!keys[key]) {
                        keys[key] = true;
                        switch (key)
                        {
                        case VK_SPACE: {
                            stance = STANDING;
                            break;
                        }
                        case VK_KEY_1:{
                            slot = SLOT1;
                            break;
                        }
                        case VK_KEY_2: {
                            slot = SLOT2;
                            break;
                        }
                        case VK_KEY_J:
                        case VK_KEY_H:
                        case VK_KEY_G: {
                            slot = NOSLOT;
                            lastSlot = NOSLOT;
                            break;
                        }
                        case VK_KEY_X: {
                            if (slot == NOSLOT) {
                                slot = lastSlot;
                            }
                            else {
                                lastSlot = slot;
                                slot = NOSLOT;
                            }
                            break;
                        }
                        case VK_KEY_C: {
                            if (stance == STANDING || stance == PRONE) {
                                stance = CROUCH;
                            }
                            else if (stance == CROUCH) {
                                stance = STANDING;
                            }
                            break;
                        }
                        case VK_KEY_Z: {
                            if (stance == PRONE) {
                                stance = STANDING;
                            }
                            else {
                                stance = PRONE;
                            }
                            break;
                        }
                        case VK_F20:{
                            profile[slot] = HEAVY;
                            break;
                        }
                        case VK_F21: {
                            profile[slot] = LIGHT;
                            break;
                        }
                        case VK_F22: {
                            profile[slot] = NOPROFILE;
                            break;
                        }
                        case VK_DIVIDE: {
                            cfg->updateVisibility();
                            bNoUpdate = true;
                            break;
                        }
                        case VK_HOME: {
                            cfg->updateCFG();
                            break;
                        }
                        default:
                            break;
                        }
                        update = (bNoUpdate ? false : true);
                        bNoUpdate = false;
                    }
                    break;
                }
                case WM_KEYUP:
                case WM_SYSKEYUP: {
                    keys[key] = false;
                    break;
                }
                default:
                    break;
                }
            }
        }
    }
    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

void __stdcall eventsThread()
{
    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, NULL);
    MSG msg{ 0 };
    //our application loop
    while (GetMessage(&msg, NULL, 0, 0) != 0);
    return;
}

int main() //main function
{
    SetConsoleTitle("EscoC");
    if (!cfg->updateCFG())
        exit_();

    HANDLE hThreadEvents = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)eventsThread, 0, 0, 0);
    HANDLE hThreadRecoil = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)recoilThread, 0, 0, 0);
    if (hThreadEvents && hThreadRecoil) {
        SetThreadPriority(hThreadEvents, 1);
        SetThreadPriority(hThreadRecoil, 1);
        _L("threads created")
        for (;;)
        {
            if (update) {
                if (slot != NOSLOT && profile[slot] != NOPROFILE) {
                    force = max(cfg->table[profile[slot]][FORCE][stance], 0);
                    delay = max(cfg->table[profile[slot]][DELAY][stance], 1);
                }
                else {
                    force = 0;
                    delay = 0;
                }
                cfg->hidden = false;
                printStatus();
                update = false;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    exit_("creating threads failed");
    return 0;
}