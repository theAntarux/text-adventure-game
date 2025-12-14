@echo off

cd ..
set /p player_name=Zadaj meno hraca: 

if %errorlevel% == 0 (
    game.exe -n %player_name%
) else (
    echo Daky error
)

echo .
pause

:end
