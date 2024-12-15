chcp 65001
cls

@echo off
call bat/info-kostenuksoft.bat

REM У цілях практики було вирішено написати пару .bat файлів.

echo. Пересвідчтеся, що перелічені залежності встановлені на ваш комп'ютер.
:input

echo. Збудувати проєкти ? (Так/Ні) 
set /p input=

if /i "%input%" == "так" goto continue
if /i "%input%" == "ні" exit /b
echo. Ввід не по шаблону. Введіть "Так" або "Ні".

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

echo. Настисність будь-яку клавішу, щоб покинути термінал...
pause >nul

REM (C) kostenuksoft - Repetovsky Vladysalv Volodymyrovych

