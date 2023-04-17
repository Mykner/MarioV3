#pragma once

#include "resource.h"

#include "Common.h"

#include "Game.h"
#include "RenderManager.h"
#include "FModManager.h"

#include <DbgHelp.h>

#define MAX_LOADSTRING 100

HINSTANCE g_hInstance;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
bool InitEnvironment(HINSTANCE hInstance, Game *pGame, const wchar_t* szCurrentDir);

// Prevent System-key during play
HHOOK g_hKeyboardHook;
bool g_bWindowActive;
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

LONG WINAPI CrashHandler(EXCEPTION_POINTERS* pException);
