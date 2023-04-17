@echo off
pushd %~dp0
set TIME_HOUR=%time:~0,2%
if "%TIME_HOUR%"==" 0" (
	set TIME_HOUR=00
)
if "%TIME_HOUR%"==" 1" (
	set TIME_HOUR=01
)
set TIME_STAMP=%date:~0,4%%date:~5,2%%date:~8,2%_%TIME_HOUR%%time:~3,2%%time:~6,2%
set SOLUTION_DIR=%~dp0
set OUT_DIR=%SOLUTION_DIR%Output\ReleaseDebug\
echo TIME_HOUR : %TIME_HOUR%
echo TIME_STAMP : %TIME_STAMP%
echo SOLUTION_DIR : %SOLUTION_DIR%
echo OUT_DIR : %OUT_DIR%
pause
Output\post_build_mario_app_release_debug.bat
pause