echo off
IF "/%1"=="/name" ECHO:GCC-ARM compiler UNO R4 WIFI BSP 1.2.2 & exit /b 0

:: Core Arduino tools
set ARDUINO_BSP_VER=1.2.2
set ARDUINO_ROOT=%NQBP_PKG_ROOT%arduino\
set ARDUINO_BOSSAC=%ARDUINO_ROOT%tools\bossac\1.9.1-arduino5

:: Add Compiler to the path. Note: Leverage the same cross compiler used for the STM32 MCUs
call set PATH=%NQBP_XPKGS_ROOT%\stm32-gcc-arm\bin;%PATH%

::
echo:Environment set for ARM-Cortex M4, Uno R4 Wifi (v%ARDUINO_BSP_VER%)
