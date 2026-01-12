@echo off

echo.
set /p action=Zadaj akciu (0 - play, 1 - load): 
set /p player_name=Zadaj meno hraca: 
echo.

if not "%action%"=="0" if not "%action%"=="1" (
    echo Chyba: Neplatna akcia! Musi byt 0 alebo 1.
    echo.
    pause
    exit /b 1
)

if "%player_name%"=="" (
    echo Chyba: Nezadal si ziadne meno!
    echo.
    pause
    exit /b 1
)

cd /d "%~dp0\.."

if "%action%"=="0" (
    echo Spustenie hry pre hraca "%player_name%"
    game.exe -n "%player_name%"
) else (
    echo Nacitanie ulozenej hry pre hraca "%player_name%"
    game.exe -l "%player_name%"
)

echo.
pause