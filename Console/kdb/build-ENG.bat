chcp 65001
cls

@echo off
call bat/info-kostenuksoft-ENG.bat

REM ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

echo. All listed dependencies above installed on this computer ?
:input

echo. Build projects ? (Yes/No) 
set /p input=

if /i "%input%" == "yes" goto continue
if /i "%input%" == "no" exit /b
echo. Invalid. Input "YES" or "NO". (case insensitive)

goto input
:continue

cls
cd codeit 
mkdir build 
cd build
cmake ..
cmake --build . --config Release --config Debug

cd ..\..\

mkdir build  
cd build 
cmake ..
cmake --build . --config Release --config Debug

echo. Press any key to exit...
pause >nul

REM (C) kostenuksoft 
REM ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::