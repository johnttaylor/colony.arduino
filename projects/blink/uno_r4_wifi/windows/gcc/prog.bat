@echo off
:: Helper script to program the board using the Arduino tools.
::
:: Usage:
::   prog.bat [<comport>]
::
:: Example:
::   prog.bat COM3   // Note: the COM port only needs to be specified once per session

:: Executable
set _TEMP_EXE=blink.bin

:: Remember the COM Port
IF NOT "/%1"=="/" set ARDUINO_TARGET_COMPORT=%1

:: Brute reset the board (not needed if using the '-a' option)
::(echo touch & taskkill /f /im plink.exe) | "\Program Files\PuTTY\plink.exe" -serial COM3 -sercfg 1200

:: Program the board
"%ARDUINO_BOSSAC%\bossac" --port=%ARDUINO_TARGET_COMPORT% -U -a -w _arduino\%_TEMP_EXE% -R
