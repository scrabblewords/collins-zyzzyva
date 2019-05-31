rem -------------------------------------------------------------------------
rem create-windows-32bit.bat
rem
rem Create a Windows staging directory for the installer packager to use.
rem
rem Copyright 2015-2019 Twilight Century Computing.
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


cd ..

rem *** Update these vars as necessary, mainly the first 3 ***
set QTVER=5.12.2\5.12.3\mingw73_32
set BUILDDIR=F:\Programming_projects\boshvark-zyzzyva-pc-2.2.3-0-g9ddd4ac\build-zyzzyva-Desktop_Qt_5_12_3_MinGW_32_bit2-Release
set INSTDIR=installer\staging\CZ5.1.1
set QTDIR=E:\Qt\%QTVER%
set PLATFORM=windows-32bit
set OUTDIR=%INSTDIR%\%PLATFORM%

rmdir /S /Q %OUTDIR%
mkdir %INSTDIR%
mkdir %OUTDIR%

rem Copy Qt libraries
copy %QTDIR%\bin\Qt5Core.dll %OUTDIR%
copy %QTDIR%\bin\Qt5Gui.dll %OUTDIR%
copy %QTDIR%\bin\Qt5Network.dll %OUTDIR%
copy %QTDIR%\bin\Qt5Sql.dll %OUTDIR%
copy %QTDIR%\bin\Qt5Xml.dll %OUTDIR%
copy %QTDIR%\bin\Qt5Widgets.dll %OUTDIR%
copy %QTDIR%\bin\Qt5PrintSupport.dll %OUTDIR%

rem Copy Qt plugins
mkdir %OUTDIR%\platforms
copy %QTDIR%\plugins\platforms\qwindows.dll %OUTDIR%\platforms
mkdir %OUTDIR%\sqldrivers
copy %QTDIR%\plugins\sqldrivers\qsqlite.dll %OUTDIR%\sqldrivers
mkdir %OUTDIR%\printsupport
copy %QTDIR%\plugins\printsupport\windowsprintersupport.dll %OUTDIR%\printsupport

rem Copy other libraries
xcopy /I /E /H /R /Y %QTDIR%\bin\libstdc++-6.dll %OUTDIR%
copy %QTDIR%\bin\libgcc_s_dw2-1.dll %OUTDIR%
copy %QTDIR%\bin\libwinpthread-1.dll %OUTDIR%

rem Copy Zyzzyva images
copy %BUILDDIR%\bin\zyzzyva.exe %OUTDIR%
copy %BUILDDIR%\bin\zyzzyva5.dll %OUTDIR%
mkdir %OUTDIR%\images
copy images\Zyzzyva.ico %OUTDIR%\images

rem Copy more Zyzzyva stuff
copy AUTHORS %OUTDIR%
copy CHANGES %OUTDIR%
copy COPYING %OUTDIR%
copy LICENSE %OUTDIR%
copy README %OUTDIR%

rem Copy Zyzzyva data directory
xcopy /I /E /H /R /Y data %OUTDIR%\data
