@echo off
setlocal enabledelayedexpansion
echo Baue und starte Qt-Anwendung...

:: Qt und MinGW Pfade setzen
set "PATH=C:\Qt\6.5.3\mingw_64\bin;C:\Qt\Tools\mingw1120_64\bin;%PATH%"

:: Sicherstellen, dass keine Instanz läuft
taskkill /F /IM cpp_espanso_helper.exe 2>NUL

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
if exist build rd /s /q build
mkdir build || exit /b 1
cd build || exit /b 1

:: Kompilieren
echo Führe CMake aus...
cmake -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="C:/Qt/6.5.3/mingw_64;%CD%/../extern" ..
if errorlevel 1 (
    echo CMake-Fehler!
    cd ..
    exit /b 1
)

echo Kompiliere...
mingw32-make
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

:: Deployment
echo Führe windeployqt aus...
C:\Qt\6.5.3\mingw_64\bin\windeployqt.exe cpp_espanso_helper.exe
if errorlevel 1 (
    echo Deployment-Fehler!
    cd ..
    exit /b 1
)

cd ..

:: Release-Ordner vorbereiten
if exist release rd /s /q release
mkdir release || exit /b 1

:: Alles aus build in release kopieren
echo Kopiere Dateien in Release-Ordner...
xcopy /E /I /Y build\*.* release\ || exit /b 1

:: yaml-cpp DLL kopieren
echo Kopiere yaml-cpp DLL...
copy "extern\lib\libyaml-cpp.dll" release\ || exit /b 1

:: Beende laufende Instanz
taskkill /F /IM cpp_espanso_helper.exe 2>NUL

:: Warte kurz
timeout /t 1 /nobreak >nul

:: Programm im release-Ordner starten
cd release || exit /b 1
echo Starte Anwendung aus: %CD%
cpp_espanso_helper.exe
if errorlevel 1 (
    echo Fehler beim Starten der Anwendung!
    echo Errorlevel: %errorlevel%
    pause
)
cd ..

exit /b 0 