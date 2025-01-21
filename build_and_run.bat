@echo off
setlocal enabledelayedexpansion
echo Baue und starte Qt-Anwendung...

:: yaml-cpp installieren wenn nicht vorhanden
if not exist extern\lib\libyaml-cpp.dll (
    echo Installing yaml-cpp...
    call get_yaml_cpp.bat
    if errorlevel 1 (
        echo Fehler bei der yaml-cpp Installation!
        exit /b 1
    )
)

:: Build-Verzeichnis vorbereiten
if exist build (
    echo Leere build Verzeichnis...
    rd /s /q build
)
mkdir build || exit /b 1
cd build || exit /b 1

:: CMake und Build
echo Führe CMake aus...
cmake -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="C:/Qt/6.5.3/mingw_64;%CD%/../extern" ..
if errorlevel 1 (
    echo CMake-Fehler!
    cd ..
    exit /b 1
)

echo Kompiliere...
cmake --build .
if errorlevel 1 (
    echo Kompilierungsfehler!
    cd ..
    exit /b 1
)

:: Prüfe ob die exe existiert
if not exist cpp_espanso_helper.exe (
    echo ERROR: cpp_espanso_helper.exe wurde nicht erstellt!
    cd ..
    exit /b 1
)

:: Qt-DLLs kopieren
echo Kopiere benötigte Qt-Dateien...
mkdir platforms
copy "C:\Qt\6.5.3\mingw_64\plugins\platforms\qwindows.dll" "platforms\" || exit /b 1
copy "C:\Qt\6.5.3\mingw_64\bin\Qt6Widgets.dll" . || exit /b 1
copy "C:\Qt\6.5.3\mingw_64\bin\Qt6Core.dll" . || exit /b 1
copy "C:\Qt\6.5.3\mingw_64\bin\Qt6Gui.dll" . || exit /b 1
copy "C:\Qt\6.5.3\mingw_64\bin\libgcc_s_seh-1.dll" . || exit /b 1
copy "C:\Qt\6.5.3\mingw_64\bin\libstdc++-6.dll" . || exit /b 1
copy "C:\Qt\6.5.3\mingw_64\bin\libwinpthread-1.dll" . || exit /b 1

:: yaml-cpp DLL kopieren
echo Kopiere yaml-cpp DLL...
copy "..\extern\lib\libyaml-cpp.dll" . || exit /b 1

:: Programm ausführen
echo Starte Anwendung...
start "" ".\cpp_espanso_helper.exe"

cd ..
exit /b 0 