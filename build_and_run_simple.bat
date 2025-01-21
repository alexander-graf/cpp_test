@echo off
echo Baue und starte Qt-Anwendung...

if exist build (rd /s /q build)
mkdir build
cd build

cmake -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH=C:/Qt/6.5.3/mingw_64 ..
mingw32-make

C:\Qt\6.5.3\mingw_64\bin\windeployqt.exe mein_gui_projekt.exe
start mein_gui_projekt.exe

cd .. 