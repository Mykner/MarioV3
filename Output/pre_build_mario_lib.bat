@echo off
SETLOCAL ENABLEDELAYEDEXPANSION

set MAJOR_VER=3
set MINOR_VER=4
set PATCH_VER=0

@set YEAR=%date:~0,4%
@set MONTH=%date:~5,2%
@set DAY=%date:~8,2%
@set HOUR=%time:~0,2%
@if "%HOUR:~0,1%" == " " set HOUR=0%HOUR:~1,1%
@set MINUTE=%time:~3,2%
@if "%MINUTE:~0,1%" == " " set MINUTE=0%MINUTE:~1,1%
@set SECOND=%time:~6,2%
@if "%SECOND:~0,1%" == " " set SECOND=0%SECOND:~1,1%

@set BUILD_DATETIME=%YEAR%%MONTH%%DAY%_%HOUR%%MINUTE%%SECOND%

echo * Build Datetime = %BUILD_DATETIME%
FOR /F "tokens=* USEBACKQ" %%F IN (`git rev-list --count --first-parent HEAD`) DO (
  SET REVISION_NUMBER=%%F
)
(
echo #define MAJOR_VER			%MAJOR_VER%
echo #define MINOR_VER			%MINOR_VER%
echo #define PATCH_VER			%PATCH_VER%
echo #define MAJOR_VER_WSTR		L"%MAJOR_VER%"
echo #define MINOR_VER_WSTR		L"%MINOR_VER%"
echo #define PATCH_VER_WSTR		L"%PATCH_VER%"
echo #define _VERSION			MAJOR_VER, MINOR_VER, PATCH_VER, REVISION_NUMBER
echo #define _VERSION_WSTR		MAJOR_VER_WSTR "." MINOR_VER_WSTR "." PATCH_VER_WSTR "." REVISION_NUMBER_WSTR
echo #define BUILD_DATETIME_WSTR	L"%BUILD_DATETIME%"
echo #define REVISION_NUMBER		%REVISION_NUMBER%
echo #define REVISION_NUMBER_WSTR	L"%REVISION_NUMBER%"
) > ..\MarioLibVersion.h
