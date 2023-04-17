#pragma once

#include "NaCommon.h"

#if defined(WIN32)
typedef HWND NaPlatformWindowHandle;
#else
typedef void* NaPlatformWindowHandle; // temp
#endif
