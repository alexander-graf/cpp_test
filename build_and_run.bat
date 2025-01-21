@echo off
echo Baue und starte Qt-Anwendung...

:: Verzeichnis erstellen oder leeren
if exist build (
    echo Leere build Verzeichnis...
    rd /s /q build
)
mkdir build
cd build

:: CMake und Build
echo Führe CMake aus...
cmake -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH=C:/Qt/6.5.3/mingw_64 ..
if errorlevel 1 (
    echo CMake-Fehler!
    pause
    exit /b 1
)

echo Kompiliere...
mingw32-make
if errorlevel 1 (
    echo Kompilierungsfehler!
    pause
    exit /b 1
)

:: Qt-DLLs kopieren
echo Kopiere benötigte Qt-Dateien...
mkdir platforms
copy "C:\Qt\6.5.3\mingw_64\plugins\platforms\qwindows.dll" "platforms\"
copy "C:\Qt\6.5.3\mingw_64\bin\Qt6Widgets.dll" .
copy "C:\Qt\6.5.3\mingw_64\bin\Qt6Core.dll" .
copy "C:\Qt\6.5.3\mingw_64\bin\Qt6Gui.dll" .
copy "C:\Qt\6.5.3\mingw_64\bin\libgcc_s_seh-1.dll" .
copy "C:\Qt\6.5.3\mingw_64\bin\libstdc++-6.dll" .
copy "C:\Qt\6.5.3\mingw_64\bin\libwinpthread-1.dll" .

:: Programm ausführen
echo Starte Anwendung...
start mein_gui_projekt.exe

cd .. 