@echo off
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE=%cd%\vcpkg\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows
cmake --build build --config Release


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
