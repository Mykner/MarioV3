//
//  NaPlatformAPI_Win32.cpp
//  MarioV3
//
//  Created by neoarc on 2016. 8. 24..
//  Copyright (c) 2016 neoarc. All rights reserved.
//

#include "NaPlatformAPI.h"
#include <filesystem>

#include <Iphlpapi.h>
#include <Assert.h>
#pragma comment(lib, "iphlpapi.lib")

namespace fs = std::experimental::filesystem;

#ifdef WIN32

#include "dirent.h"

void NaPlatform::AdjustWindowRect(NaRect &rc)
{
	::AdjustWindowRect((LPRECT)&rc, WS_OVERLAPPEDWINDOW, FALSE);
}

SIZE NaPlatform::GetScreenSize()
{
	SIZE s;
	s.cx = GetSystemMetrics(SM_CXSCREEN);
	s.cy = GetSystemMetrics(SM_CYSCREEN);

	return s;
}

void NaPlatform::Sleep(DWORD dwMilliSeconds)
{
	::Sleep(dwMilliSeconds);
}

DWORD NaPlatform::GetTickCount()
{
	return ::GetTickCount();
}

NaString NaPlatform::GetCurrentPath()
{
	NaString str;
	wchar_t szBuf[256];
	::GetCurrentDirectory(256, szBuf);
	str = szBuf;

	return str;
}

NaString NaPlatform::GetDocumentPath()
{
	static NaString str = L"";
	if (str.GetLength() == 0)
	{
		str = GetCurrentPath();
		str += L"\\UserData";

		CheckAndCreatePath(str);
	}

	return str;
}

void NaPlatform::FileOpen(FILE **fp, const char* szName, char* szOption)
{
	if (szOption != nullptr && szOption[0] == 'w')
	{
		NaString strFilePath = szName;
		NaStrArray ar = strFilePath.Split(L"\\");
		ar.Pop();
		strFilePath = ar.Join(L"\\");

		CheckAndCreatePath(strFilePath);

		*fp = _fsopen(szName, szOption, _SH_DENYNO);
	}
	else
		fopen_s(fp, szName, szOption);
}

NaStrArray NaPlatform::GetFileList(NaString strPath)
{
	NaStrArray ar;

	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(strPath.cstr())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			if (strcmp(ent->d_name, ".") == 0)
				continue;
			else if (strcmp(ent->d_name, "..") == 0)
				continue;
			if (ent->d_type != DT_DIR)
				ar.Add(ent->d_name);
		}
		closedir(dir);
	}

	return ar;
}

NaStrArray NaPlatform::GetSubDirectoryList(NaString strPath)
{
	NaStrArray ar;

	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(strPath.cstr())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL) 
		{
			if (strcmp(ent->d_name, ".") == 0)
				continue;
			else if (strcmp(ent->d_name, "..") == 0)
				continue;
			if (ent->d_type == DT_DIR)
				ar.Add(ent->d_name);
		}
		closedir(dir);
	}

	return ar;
}

void NaPlatform::CheckAndCreatePath(NaString strPath)
{
	NaStrArray ar = strPath.Split(L"\\");
	NaString strTempPath = L"";
	for (int i = 0; i < ar.GetCount(); i++)
	{
		// 0: c:
		// 1: c:\\game
		// 2: c:\\game\\mario

		strTempPath += ar[i];
		if (i > 0)
		{
			if (!fs::exists(strTempPath.wstr()))
			{
				try
				{
					fs::create_directory(strTempPath.wstr());
				}
				catch(...)
				{
					NaString strException;
					strException.Format(L"FileSystem exception occured.\n%ls", strTempPath.wstr());
					::MessageBox(nullptr, strException.wstr(), L"Please excuse :'(", 0);
				}
			}
		}
		strTempPath += L"\\";
	}
}

void NaPlatform::MoveWindow(NaPlatformWindowHandle hHandle, int x, int y, int width, int height)
{
	::MoveWindow(hHandle, x, y, width, height, FALSE);
}

void NaPlatform::GetWindowRect(NaPlatformWindowHandle hHandle, LPRECT lprc)
{
    ::GetWindowRect(hHandle, lprc);
}

void NaPlatform::ShowWindow(NaPlatformWindowHandle hHandle, bool bShow)
{
	::ShowWindow(hHandle, bShow ? SW_SHOW : SW_HIDE);
}

NaPlatformWindowHandle NaPlatform::GetTopWindow(NaPlatformWindowHandle hHandle)
{
	HWND hTopWnd = hHandle;
	while (true)
	{
		HWND hParent = ::GetParent(hTopWnd);
		if (hParent)
			hTopWnd = hParent;
		else
			break;
	}

	return hTopWnd;
}

bool NaPlatform::IsActiveWindow(NaPlatformWindowHandle hHandle)
{
	if (::GetForegroundWindow() != hHandle)
		return false;
	return true;
}

NaString NaPlatform::GetMacAddress()
{
	NaString str = L"";

	PIP_ADAPTER_INFO AdapterInfo;
	DWORD dwBufLen = sizeof(AdapterInfo);
	char *mac_addr = (char*)malloc(17);

	AdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
	if (AdapterInfo == nullptr)
	{
		NaDebugOut(L"Error: allocating memory needed to call GetAdaptersinfo\n");
		return str;
	}

	// Make an initial call to GetAdaptersInfo to get the necessary size into the dwBufLen     variable
	if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW)
	{

		AdapterInfo = (IP_ADAPTER_INFO *)malloc(dwBufLen);
		if (AdapterInfo == nullptr)
		{
			NaDebugOut(L"Error: allocating memory needed to call GetAdaptersinfo\n");
			return str;
		}
	}

	if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == NO_ERROR)
	{
		PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;// Contains pointer to current adapter info
		do 
		{
			str.Format(L"%02X:%02X:%02X:%02X:%02X:%02X",
				pAdapterInfo->Address[0], pAdapterInfo->Address[1],
				pAdapterInfo->Address[2], pAdapterInfo->Address[3],
				pAdapterInfo->Address[4], pAdapterInfo->Address[5]);
			break;

			pAdapterInfo = pAdapterInfo->Next;
		} while (pAdapterInfo);
	}

	free(AdapterInfo);

	return str;
}

NaString NaPlatform::GetComputerName()
{
	NaString str = L"";

#define INFO_BUFFER_SIZE 32767

	wchar_t infoBuf[INFO_BUFFER_SIZE];
	DWORD bufCharCount = INFO_BUFFER_SIZE;

	// Get and display the name of the computer.
	if (!::GetComputerName(infoBuf, &bufCharCount))
		return str;

	str = infoBuf;

	// Get and display the user name.
	if (!GetUserName(infoBuf, &bufCharCount))
		return str;
		
	return str;
}

#endif

