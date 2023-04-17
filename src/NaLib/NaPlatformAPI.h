#pragma once

#include "NaCommon.h"
#include "NaString.h"
#include "NaRect.h"
#include <stdio.h>
#include "NaPlatformWindow.h"

class NaPlatform
{
public:
	static void AdjustWindowRect(NaRect &rc);
	static SIZE GetScreenSize();
	static void Sleep(DWORD dwMilliSeconds);
	static DWORD GetTickCount();

	static NaString GetCurrentPath();
	static NaString GetDocumentPath();
	static void CheckAndCreatePath(NaString strPath);
	static void FileOpen(FILE **fp, const char* szName, char* szOption);

	static NaStrArray GetFileList(NaString strPath);
	static NaStrArray GetSubDirectoryList(NaString strPath);

	static void MoveWindow(NaPlatformWindowHandle hHandle, int x, int y, int width, int height);
	static void GetWindowRect(NaPlatformWindowHandle hHandle, LPRECT lprc);
	static void ShowWindow(NaPlatformWindowHandle hHandle, bool bShow);
	static NaPlatformWindowHandle GetTopWindow(NaPlatformWindowHandle hHandle);
	static bool IsActiveWindow(NaPlatformWindowHandle hHandle);

	static NaString GetMacAddress();
	static NaString GetComputerName();
};