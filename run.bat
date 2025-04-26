@echo off
cd build\Release
XAlgoMain.exe
cd ../..


:: Create build folder if missing
if not exist build (
    mkdir build
)

:: Go into build folder
cd build

:: Run CMake configuration if no build files
if not exist Makefile (
    cmake ..
)

:: Build the project
cmake --build .

:: Find the generated executable
set EXECUTABLE=XAlgoMain.exe

:: Check if the executable exists
if exist %EXECUTABLE% (
    echo ===========================
    echo Running %EXECUTABLE% ...
    echo ===========================
    .\%EXECUTABLE%
) else (
    echo ===========================
    echo ERROR: Executable not found!
    echo Expected: %EXECUTABLE% inside /build
    echo ===========================
)

:: Return to project root
cd ..

echo ===========================
echo Run finished! Press any key to exit...
echo ===========================
pause
