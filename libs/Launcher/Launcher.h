#pragma once
#include "pch.h"

static const wchar_t* PROCESS_NAME = (isXbox) ? L"Palworld-WinGDK-Shipping.exe" : L"Palworld-Win64-Shipping.exe";
static const char* DLL_NAME = (isDebug) ? "palcrackd.dll" : "palcrack.dll";

int ExitWithErrorMsg(const char* eMSG, DWORD eCODE);
std::string GetCurrentPath();
bool IsGameRunning(const wchar_t* procName, DWORD* dwPID);
bool LaunchPalworld();
int exec();
