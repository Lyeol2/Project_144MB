@echo off
echo =========================================
echo Editor - Build Script (CMake)
echo =========================================

REM 1. Configure
echo [INFO] Configuring CMake...
cmake -B build -S .

IF %ERRORLEVEL% NEQ 0 (
    echo [ERROR] CMake configuration failed!
    pause
    exit /b %ERRORLEVEL%
)

REM 2. Build
echo [INFO] Building the project...
cmake --build build --config Release --target EditorApp

IF %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Build failed!
    pause
    exit /b %ERRORLEVEL%
)

echo [SUCCESS] Build completed successfully!
echo [INFO] Editor.exe should be in the root directory.
pause
