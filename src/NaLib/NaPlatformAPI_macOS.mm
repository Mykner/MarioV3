//
//  NaPlatformAPI_macOS.mm
//  MarioV3
//
//  Created by neoarc on 2016. 8. 24..
//  Copyright © 2016년 neoarc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

#include "NaPlatformAPI.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "dirent.h"

void NaPlatform::AdjustWindowRect(NaRect &rc)
{
    //::AdjustWindowRect(lpRect, WS_OVERLAPPEDWINDOW, FALSE);
    rc.bottom += 26;
}

SIZE NaPlatform::GetScreenSize()
{
    NSRect screenRect;
    NSArray *screenArray = [NSScreen screens];
    unsigned screenCount = [screenArray count];
    unsigned index = 0;
    
    for (index; index < screenCount; index++)
    {
        NSScreen *screen = [screenArray objectAtIndex: index];
        screenRect = [screen visibleFrame];
    }
    
    SIZE s;
    s.cx = screenRect.size.width;
    s.cy = screenRect.size.height;
    
    return s;
}

void NaPlatform::Sleep(DWORD dwMilliSeconds)
{
    // #TODO Check Param
    ::sleep(dwMilliSeconds / 1000);
}

DWORD NaPlatform::GetTickCount()
{
	struct timeval tv;
    gettimeofday(&tv, 0);
    return unsigned((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

NaString NaPlatform::GetCurrentPath()
{
    NaString str;
    char szBuf[256];
    getcwd(szBuf, 256);
    
    str = szBuf;
    
    return str;
}

NaString NaPlatform::GetDocumentPath()
{
	NSArray *arr = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentPath = [arr lastObject];
	const char *szDocumentPath = [documentPath cStringUsingEncoding:NSUTF8StringEncoding];

	// documentPath = /Users/UserName/Documents
	static NaString str = L"";
	if (str.GetLength() == 0)
	{
		str = szDocumentPath;
		str += L"/MarioV3";

        CheckAndCreatePath(str);
	}

	return str;
}

void NaPlatform::FileOpen(FILE **fp, const char* szName, char* szOption)
{
    NaString strName = szName;
    strName.ReplaceAll(L"\\", L"/");
    
	FILE *fp2;
    const char* szName2 = strName.cstr();
	if (szOption != nullptr && szOption[0] == 'w')
	{
		NaString strFilePath = szName2;
		NaStrArray ar = strFilePath.Split(L"/");
		ar.Pop();
		strFilePath = ar.Join(L"/");

		CheckAndCreatePath(strFilePath);
	}

    fp2 = fopen(szName2, szOption);
    *fp = fp2;
}

NaStrArray NaPlatform::GetFileList(NaString strPath)
{
    strPath.ReplaceAll(L"\\", L"/");
    
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
    strPath.ReplaceAll(L"\\", L"/");
    
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
	struct stat info;
    if (stat(strPath.cstr(), &info) != 0)
    {
        mkdir(strPath.cstr(), 0755);
    }
}

void NaPlatform::MoveWindow(NaPlatformWindowHandle hHandle, int x, int y, int width, int height)
{
    NSWindow *window = (__bridge NSWindow*)hHandle;
    
    NSRect frame = [window frame];
    frame.origin.x = x;
    frame.origin.y = y;
    frame.size.width = width;
    frame.size.height = height;
    [window setFrame: frame
             display: YES
             animate: NO];
}

void NaPlatform::GetWindowRect(NaPlatformWindowHandle hHandle, LPRECT lprc)
{
    NSWindow *window = (__bridge NSWindow*)hHandle;
    
    NSRect frame = [window frame];
    lprc->left = frame.origin.x;
    lprc->top = frame.origin.y;
    lprc->right = frame.size.width + lprc->left;
    lprc->bottom = frame.size.height + lprc->top;
}

void NaPlatform::ShowWindow(NaPlatformWindowHandle hHandle, bool bShow)
{
	//::ShowWindow(hHandle, bShow ? SW_SHOW : SW_HIDE);
}

NaPlatformWindowHandle NaPlatform::GetTopWindow(NaPlatformWindowHandle hHandle)
{
	/*
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
	*/

	return nullptr;
}

bool NaPlatform::IsActiveWindow(NaPlatformWindowHandle hHandle)
{
	/*
	if (::GetForegroundWindow() != hHandle)
		return false;
	return true;
	*/

	return true;
}