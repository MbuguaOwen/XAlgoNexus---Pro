@echo off
echo ===========================
echo Building XAlgoNexus-1.4...
echo ===========================

:: Create build folder if it doesn't exist
if not exist build (
    mkdir build
)

:: Move into build folder
cd build

:: Run CMake configuration if no Makefiles/Solutions exist
if not exist Makefile (
    cmake ..
)

:: Build the project
cmake --build .

:: Go back to project root
cd ..

echo ===========================
echo Build finished! Press any key to exit...
echo ===========================
pause
