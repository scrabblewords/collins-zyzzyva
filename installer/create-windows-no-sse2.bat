rem -------------------------------------------------------------------------
rem create-windows.sh
rem
rem Create a Windows directory for the installer to use.
rem
rem Copyright 2016 Twilight Century Computing.
rem Copyright 2006-2012 North American SCRABBLE Players Association.
rem
rem This file is part of Zyzzyva.
rem
rem Zyzzyva is free software; you can redistribute it and/or modify
rem it under the terms of the GNU General Public License as published by
rem the Free Software Foundation; either version 2 of the License, or
rem (at your option) any later version.
rem
rem Zyzzyva is distributed in the hope that it will be useful,
rem but WITHOUT ANY WARRANTY; without even the implied warranty of
rem MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
rem GNU General Public License for more details.
rem
rem You should have received a copy of the GNU General Public License
rem along with this program; if not, write to the Free Software
rem Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
rem -------------------------------------------------------------------------

setlocal

cd ..

set QTVER=Qt5.5.0-no-sse2\qtbase
set QTDIR=E:\Qt\%QTVER%
set INSTDIR=installer
set PLATFORM=windows
set OUTDIR=%INSTDIR%\windows-no-sse2

qmake
CALL mingw32-make

mkdir %OUTDIR%

rem Copy Qt libraries
copy %QTDIR%\bin\assistant.exe %OUTDIR%
rem copy %QTDIR%\bin\QtAssistantClient4.dll %OUTDIR%
rem copy %QTDIR%\bin\QtCore4.dll %OUTDIR%
rem copy %QTDIR%\bin\QtGui4.dll %OUTDIR%
rem copy %QTDIR%\bin\QtNetwork4.dll %OUTDIR%
rem copy %QTDIR%\bin\QtSql4.dll %OUTDIR%
rem copy %QTDIR%\bin\QtXml4.dll %OUTDIR%
rem copy %QTDIR%\bin\mingwm10.dll %OUTDIR%
rem copy %QTDIR%\bin\QtAssistantClient5.dll %OUTDIR%
copy %QTDIR%\bin\Qt5Core.dll %OUTDIR%
copy %QTDIR%\bin\Qt5Gui.dll %OUTDIR%
copy %QTDIR%\bin\Qt5Network.dll %OUTDIR%
copy %QTDIR%\bin\Qt5Sql.dll %OUTDIR%
copy %QTDIR%\bin\Qt5Xml.dll %OUTDIR%
copy %QTDIR%\bin\Qt5Widgets.dll %OUTDIR%
copy %QTDIR%\bin\Qt5PrintSupport.dll %OUTDIR%
copy %QTDIR%\bin\Qt5Help.dll %OUTDIR%
copy %QTDIR%\bin\Qt5WebKit.dll %OUTDIR%
copy %QTDIR%\bin\Qt5WebKitWidgets.dll %OUTDIR%
copy %QTDIR%\bin\Qt5CLucene.dll %OUTDIR%
copy %QTDIR%\bin\Qt5Qml.dll %OUTDIR%
copy %QTDIR%\bin\Qt5Quick.dll %OUTDIR%
copy %QTDIR%\bin\Qt5QuickWidgets.dll %OUTDIR%
copy %QTDIR%\bin\Qt5Positioning.dll %OUTDIR%
copy %QTDIR%\bin\Qt5Multimedia.dll %OUTDIR%
copy %QTDIR%\bin\Qt5MultimediaQuick_p.dll %OUTDIR%
copy %QTDIR%\bin\Qt5MultimediaWidgets.dll %OUTDIR%
copy %QTDIR%\bin\Qt5Sensors.dll %OUTDIR%
copy %QTDIR%\bin\Qt5WebChannel.dll %OUTDIR%

rem ** Following Qt5 .dlls added preemptively.  They may not be necessary. (JGM)
copy %QTDIR%\bin\Qt5Concurrent.dll %OUTDIR%
rem copy %QTDIR%\bin\Qt5DBus.dll %OUTDIR%
copy %QTDIR%\bin\Qt5Declarative.dll %OUTDIR%
copy %QTDIR%\bin\Qt5Designer.dll %OUTDIR%
copy %QTDIR%\bin\Qt5DesignerComponents.dll %OUTDIR%
copy %QTDIR%\bin\Qt5Location.dll %OUTDIR%
copy %QTDIR%\bin\Qt5OpenGL.dll %OUTDIR%
copy %QTDIR%\bin\Qt5Script.dll %OUTDIR%
copy %QTDIR%\bin\Qt5ScriptTools.dll %OUTDIR%
copy %QTDIR%\bin\Qt5WinExtras.dll %OUTDIR%
copy %QTDIR%\bin\Qt5XmlPatterns.dll %OUTDIR%
rem copy %QTDIR%\bin\mingwm10.dll %OUTDIR%
copy %QTDIR%\bin\libgcc_s_dw2-1.dll %OUTDIR%

xcopy /I /E /H /R /Y %QTDIR%\bin\libstdc++-6.dll %OUTDIR%

copy %QTDIR%\bin\libwinpthread-1.dll %OUTDIR%

rem ** !!!! **
copy E:\gtk\bin\libxml2-2.dll %OUTDIR%
copy E:\gtk\bin\zlib1.dll %OUTDIR%
copy E:\gtk\bin\liblzma-5.dll %OUTDIR%
copy E:\gtk\bin\libiconv-2.dll %OUTDIR%

copy %QTDIR%\bin\icuuc54.dll %OUTDIR%
copy %QTDIR%\bin\icuin54.dll %OUTDIR%
copy %QTDIR%\bin\icudt54.dll %OUTDIR%

mkdir %OUTDIR%\platforms
copy %QTDIR%\plugins\platforms\qwindows.dll %OUTDIR%\platforms

rem Copy Qt plugins
mkdir %OUTDIR%\sqldrivers
rem copy %QTDIR%\plugins\sqldrivers\qsqlite4.dll %OUTDIR%\sqldrivers
copy %QTDIR%\plugins\sqldrivers\qsqlite.dll %OUTDIR%\sqldrivers
mkdir %OUTDIR%\printsupport
copy %QTDIR%\plugins\printsupport\windowsprintersupport.dll %OUTDIR%\printsupport

rem Copy Zyzzyva images
mkdir %OUTDIR%\images
copy images\Zyzzyva.ico %OUTDIR%\images

rem Copy Zyzzyva stuff
copy bin\zyzzyva.exe %OUTDIR%
copy bin\zyzzyva5.dll %OUTDIR%
copy zyzzyva.top %OUTDIR%
copy AUTHORS %OUTDIR%\AUTHORS.txt
copy CHANGES %OUTDIR%\CHANGES.txt
copy COPYING %OUTDIR%\COPYING.txt
copy LICENSE %OUTDIR%\LICENSE.txt
copy README %OUTDIR%\README.txt

rem Copy Zyzzyva data directory
xcopy /I /E /H /R /Y data %OUTDIR%\data

endlocal
