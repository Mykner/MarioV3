#pragma once

#if defined(WIN32)
#include <Windows.h>
#else
typedef unsigned long DWORD;
typedef DWORD COLORREF;
typedef unsigned char BYTE;
typedef unsigned short WORD;

// POINT
typedef struct tagPOINT
{
	long x;
	long y;
} 	POINT;

typedef struct tagPOINT *PPOINT;
typedef struct tagPOINT *LPPOINT;

// RECT
typedef struct tagRECT
{
	long left;
	long top;
	long right;
	long bottom;
} 	RECT;

typedef struct tagRECT *PRECT;
typedef struct tagRECT *LPRECT;
typedef const RECT *LPCRECT;

// SIZE
typedef struct tagSIZE
{
    long cx;
    long cy;
} SIZE, *PSIZE, *LPSIZE;

typedef SIZE SIZEL;
typedef SIZE *PSIZEL, *LPSIZEL;

enum VARENUM
{
    VT_I4 = 3,
    VT_R4 = 4,
    VT_BOOL = 11,
};

enum VirtualKeys
{
    VK_ESCAPE = 0x35,
    
    VK_BACK = 0x33, // 0x08,
    VK_TAB = 0x30,
    VK_SPACE = 0x31, //0x20,
	VK_LEFT = 123, //0x25,
	VK_RIGHT = 124, //0x27,
	VK_UP = 126, //0x26,
	VK_DOWN = 125, //0x28,
    
    VK_F1 = 0x7a, //0x70,
    VK_F2 = 0x78,
    VK_F3 = 0x63,
    VK_F4 = 0x76,
    VK_F5 = 0x60,
    VK_F6 = 0x61,
    VK_F7 = 0x62,
    VK_F8 = 0x64,
    VK_F9 = 0x65,
    VK_F10 = 0x6d,
    VK_F11 = 0x67,
    VK_F12 = 0x6f,
    
    VK_RETURN = 0x24, //0x0d,
    
    VK_ADD = 0x48, //0x6b, // temp: volume up
    VK_SUBTRACT = 0x49, //0x6d, // temp: volume down
    
    VK_1 = 0x12,
    VK_2,
    VK_3,
    VK_4,
    VK_5 = 0x17,
    VK_6 = 0x16,
    VK_7 = 0x1a,
    VK_8 = 0x1c,
    VK_9 = 0x19,
    VK_0 = 0x1d,
    
    VK_Q = 0x0c,
    VK_W = 0x0d,
    VK_E = 0x0e,
    VK_R = 0x0f,
    VK_T = 0x11,
    VK_Y = 0x10,
    VK_U = 0x20,
    VK_I = 0x22,
    VK_O = 0x1f,
    VK_P = 0x23,
    
    VK_A = 0x00,
    VK_S = 0x01,
    VK_D = 0x02,
    VK_F = 0x03,
    VK_G = 0x06,
    VK_H = 0x04,
    VK_J = 0x26,
    VK_K = 0x28,
    VK_L = 0x25,
    
    VK_Z = 0x06,
    VK_X = 0x07,
    VK_C = 0x08,
    VK_V = 0x09,
    VK_B = 0x0b,
    VK_N = 0x2d,
    VK_M = 0x2e,
};

#define CP_ACP                    0
#define CP_UTF8                   65001       // UTF-8 translation

#define LOBYTE(w) ((BYTE)(((unsigned long)(w)) & 0xff))

#define DebugBreak()    while(false) {};
#endif

// maps unsigned 8 bits/channel to COLOR
#define COLOR_ARGB(a,r,g,b) \
    ((DWORD)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

#define COLOR_RGBA(r,g,b,a) COLOR_ARGB(a,r,g,b)
#define COLOR_XRGB(r,g,b)   COLOR_ARGB(0xff,r,g,b)

#define COLOR_R(rgb)      (LOBYTE(rgb))
#define COLOR_G(rgb)      (LOBYTE(((WORD)(rgb)) >> 8))
#define COLOR_B(rgb)      (LOBYTE((rgb)>>16))

#define MIN(a, b)           ((a < b) ? a : b)
#define MAX(a, b)           ((a > b) ? a : b)

#define NA_DEBUGOUT_TEMPBUFFER_SIZE 65535

void NaDebugOut(const wchar_t* pszFormat, ...);