mkdir build
cd /D "%~dp0"
cd build
cmake ..
cmake --build .
@echo off 
echo build done, you can find the exe in build/bin/debug
pause