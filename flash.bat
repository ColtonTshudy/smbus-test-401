@echo off
setlocal enabledelayedexpansion

set "SCRIPT_DIR=%~dp0"
set "SCRIPT_DIR=%SCRIPT_DIR:~0,-1%"
set "SCRIPT_DIR=%SCRIPT_DIR:\=/%"

for %%I in ("%SCRIPT_DIR%") do set "PARENT_DIR_NAME=%%~nxI"

echo %SCRIPT_DIR%
echo %PARENT_DIR_NAME%


cmake --build %SCRIPT_DIR%/build/Debug --

openocd -f interface/stlink.cfg -f target/stm32f4x.cfg ^
    -c init ^
    -c "reset init" ^
    -c "program %SCRIPT_DIR%/build/debug/%PARENT_DIR_NAME%.elf" ^
    -c reset ^
    -c shutdown
