@echo off

cd ..
set /p player_name=Zadaj meno hraca, ktoreho save chces vymazat: 

if "%player_name%"=="" (
    echo.
    echo Chyba: Nezadal si ziadne meno!
    echo.
    pause
    exit /b
)

set file_save=saves\%player_name%.dat
set file_logs=logs\%player_name%.log

if exist "%file_save%" (
    del "%file_save%"
    echo.
    echo Ulozena hra "%file_save%" bola uspesne vymazana.
) else (
    echo.
    echo Chyba: Subor "%file_save%" neexistuje. Nic sa nevymazalo.
)

if exist "%file_logs%" (
    del "%file_logs%"
    echo.
    echo Logs hry "%file_logs%" bol uspesne vymazany.
) else (
    echo.
    echo Chyba: Subor "%file_logs%" neexistuje. Nic sa nevymazalo.
)

echo.
pause

:end
