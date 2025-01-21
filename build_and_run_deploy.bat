@echo off
echo Baue und starte Qt-Anwendung...

:: Qt und MinGW Pfade setzen
set "PATH=C:\Qt\6.5.3\mingw_64\bin;C:\Qt\Tools\mingw1120_64\bin;%PATH%"

:: Sicherstellen, dass keine Instanz läuft
taskkill /F /IM mein_gui_projekt.exe 2>NUL

:: Build-Verzeichnis vorbereiten
if exist build (rd /s /q build)
mkdir build
cd build

:: Kompilieren
cmake -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH=C:/Qt/6.5.3/mingw_64 ..
mingw32-make

:: Deployment
C:\Qt\6.5.3\mingw_64\bin\windeployqt.exe mein_gui_projekt.exe

:: Release-Ordner erstellen
cd ..
if exist release (rd /s /q release)
mkdir release

:: Alles aus build in release kopieren
xcopy /E /I /Y build\*.* release\

:: Programm im release-Ordner starten
cd release
start mein_gui_projekt.exe
cd .. 