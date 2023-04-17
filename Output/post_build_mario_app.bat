@echo ### PostBuild Batch - Start ###

:: Create Directory
if not exist %SOLUTION_DIR%..\MarioV3Release mkdir %SOLUTION_DIR%..\MarioV3Release
if not exist %SOLUTION_DIR%..\MarioV3Release\Temp mkdir %SOLUTION_DIR%..\MarioV3Release\Temp
cd %SOLUTION_DIR%..\MarioV3Release\Temp

:: Clear Old Files
if exist Mario.exe (
@del /q /s *.*
@rmdir /q /s
)

:: Copy Binary
copy %OUT_DIR%Mario.exe %SOLUTION_DIR%..\MarioV3Release\Temp
copy %OUT_DIR%MarioConfig.exe %SOLUTION_DIR%..\MarioV3Release\Temp
copy %OUT_DIR%MarioMapEditor.exe %SOLUTION_DIR%..\MarioV3Release\Temp
copy %OUT_DIR%*.dll %SOLUTION_DIR%..\MarioV3Release\Temp

:: Copy Resources
mkdir %SOLUTION_DIR%..\MarioV3Release\Temp\Resources
mkdir %SOLUTION_DIR%..\MarioV3Release\Temp\Resources\BGM
mkdir %SOLUTION_DIR%..\MarioV3Release\Temp\Resources\Sound
mkdir %SOLUTION_DIR%..\MarioV3Release\Temp\Resources\Sprite
mkdir %SOLUTION_DIR%..\MarioV3Release\Temp\Resources\Map
mkdir %SOLUTION_DIR%..\MarioV3Release\Temp\Resources\Map\Classic
mkdir %SOLUTION_DIR%..\MarioV3Release\Temp\Resources\Map\BetaTester
mkdir %SOLUTION_DIR%..\MarioV3Release\Temp\Resources\Map\Development
mkdir %SOLUTION_DIR%..\MarioV3Release\Temp\Resources\Map\DevelopmentShowcase
mkdir %SOLUTION_DIR%..\MarioV3Release\Temp\Resources\GameThemes
xcopy /e /f /y %SOLUTION_DIR%Resources\BGM %SOLUTION_DIR%..\MarioV3Release\Temp\Resources\BGM
xcopy /e /f /y %SOLUTION_DIR%Resources\Sound %SOLUTION_DIR%..\MarioV3Release\Temp\Resources\Sound
xcopy /e /f /y %SOLUTION_DIR%Resources\Sprite %SOLUTION_DIR%..\MarioV3Release\Temp\Resources\Sprite

:: Exclude Maps
:: xcopy /e /f /y %SOLUTION_DIR%Resources\Map %SOLUTION_DIR%..\MarioV3Release\Temp\Resources\Map
xcopy /e /f /y %SOLUTION_DIR%Resources\Map\Classic %SOLUTION_DIR%..\MarioV3Release\Temp\Resources\Map\Classic
xcopy /e /f /y %SOLUTION_DIR%Resources\Map\BetaTester %SOLUTION_DIR%..\MarioV3Release\Temp\Resources\Map\BetaTester
xcopy /e /f /y %SOLUTION_DIR%Resources\Map\Development %SOLUTION_DIR%..\MarioV3Release\Temp\Resources\Map\Development
xcopy /e /f /y %SOLUTION_DIR%Resources\Map\DevelopmentShowcase %SOLUTION_DIR%..\MarioV3Release\Temp\Resources\Map\DevelopmentShowcase

xcopy /e /f /y %SOLUTION_DIR%Resources\GameThemes %SOLUTION_DIR%..\MarioV3Release\Temp\Resources\GameThemes

del %SOLUTION_DIR%..\MarioV3Release\Temp\Resources\Map\*.lnk
del %SOLUTION_DIR%..\MarioV3Release\Temp\Resources\Map\_*.map
del %SOLUTION_DIR%..\MarioV3Release\Temp\Resources\Sprite\*.workinfo_
del %SOLUTION_DIR%..\MarioV3Release\Temp\Resources\GameThemes\SMB3\Sprite\*.workinfo_
del %SOLUTION_DIR%..\MarioV3Release\Temp\Resources\GameThemes\SMW\Sprite\*.workinfo_

:: Copy Default profile
mkdir %SOLUTION_DIR%..\MarioV3Release\Temp\UserData
mkdir %SOLUTION_DIR%..\MarioV3Release\Temp\UserData\Profile
copy %SOLUTION_DIR%References\FirstUserData\Profile1.json %SOLUTION_DIR%..\MarioV3Release\Temp\UserData\Profile\Profile1.json

:: Packaging to zip
7z a MarioV3Release_%TIME_STAMP%.zip
move %SOLUTION_DIR%..\MarioV3Release\Temp\*.zip %SOLUTION_DIR%..\MarioV3Release

:: Backup PDB
set PDB_DIR=%SOLUTION_DIR%..\MarioV3Release\PDB_%TIME_STAMP%
if not exist %PDB_DIR% mkdir %PDB_DIR%
copy %OUT_DIR%*.pdb %PDB_DIR%

@echo ### PostBuild Batch - End ###