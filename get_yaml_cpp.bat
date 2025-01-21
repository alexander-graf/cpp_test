@echo off
setlocal enabledelayedexpansion
echo Lade und baue yaml-cpp...

:: yaml-cpp herunterladen
if not exist yaml-cpp (
    echo Klone yaml-cpp Repository...
    git clone https://github.com/jbeder/yaml-cpp.git
    if errorlevel 1 (
        echo Git clone fehlgeschlagen!
        exit /b 1
    )
)

:: Build vorbereiten
cd yaml-cpp || exit /b 1
if not exist build mkdir build
cd build || exit /b 1

:: yaml-cpp bauen
echo Konfiguriere yaml-cpp...
cmake -G "MinGW Makefiles" -DYAML_BUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=../../extern ..
if errorlevel 1 (
    echo CMake Konfiguration fehlgeschlagen!
    cd ../..
    exit /b 1
)

echo Baue yaml-cpp...
cmake --build . --config Release
if errorlevel 1 (
    echo Build fehlgeschlagen!
    cd ../..
    exit /b 1
)

echo Installiere yaml-cpp...
cmake --install .
if errorlevel 1 (
    echo Installation fehlgeschlagen!
    cd ../..
    exit /b 1
)

cd ../..

:: Überprüfe Installation und kopiere DLL wenn nötig
if exist extern\bin\libyaml-cpp.dll (
    if not exist extern\lib (
        mkdir extern\lib
    )
    copy extern\bin\libyaml-cpp.dll extern\lib\
)

:: Überprüfe finale Position
if not exist extern\lib\libyaml-cpp.dll (
    echo ERROR: yaml-cpp DLL wurde nicht erstellt!
    echo Suche nach DLL in möglichen Verzeichnissen:
    dir /s /b libyaml-cpp.dll
    exit /b 1
)

echo yaml-cpp erfolgreich installiert!
exit /b 0 