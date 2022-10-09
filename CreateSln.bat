@echo off
chcp 65001

if exist "build" (rd /s/q build)
mkdir build
cd build

cmake ..
cmake --build .

if %ERRORLEVEL% EQU 0 (cmake --open .)
else (PAUSE)

@echo on