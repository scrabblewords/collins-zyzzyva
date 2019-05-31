#!/bin/bash
#-----------------------------------------------------------------------------
# create-linux.sh
#
# Create a Linux directory for the installer to use.
#
# Copyright 2015-2016 Twilight Century Computing.
# Copyright 2006-2012 North American SCRABBLE Players Association.
#
# This file is part of Zyzzyva.
#
# Zyzzyva is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# Zyzzyva is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#-----------------------------------------------------------------------------

cd ..

set -e

QTVER=Qt5.4.1/5.4/gcc
INSTDIR=installer
QTDIR=/home/jim/$QTVER

if [ "$QTDIR" = "" ]; then
    QTDIR=/usr/local/Trolltech/Qt-$QTVER
fi

if [ "$(uname -m)" = "x86_64" ]; then
    PLATFORM=linux-x64
else 
    PLATFORM=linux
fi

OUTDIR=$INSTDIR/$PLATFORM

$QTDIR/bin/qmake
make

mkdir -p $OUTDIR/bin
cp $(find /usr/lib -name libstdc++.so.6 | head -n 1) $OUTDIR/bin
cp $(find /usr/lib -name libgcc_s.so.1 | head -n 1) $OUTDIR/bin
cp $(find /usr/lib -name libgstreamer-0.10.so.0 | head -n 1) $OUTDIR/bin
cp $(find /usr/lib -name libgstapp-0.10.so.0 | head -n 1) $OUTDIR/bin
cp $(find /usr/lib -name libgio-2.0.so.0 | head -n 1) $OUTDIR/bin
cp $(find /usr/lib -name libgstinterfaces-0.10.so.0 | head -n 1) $OUTDIR/bin
cp $(find /usr/lib -name libgstpbutils-0.10.so.0 | head -n 1) $OUTDIR/bin
cp $(find /usr/lib -name libgstvideo-0.10.so.0 | head -n 1) $OUTDIR/bin
cp $(find /usr/lib -name libgstbase-0.10.so.0 | head -n 1) $OUTDIR/bin
cp $(find /usr/lib -name libgobject-2.0.so.0 | head -n 1) $OUTDIR/bin
cp $(find /usr/lib -name libgmodule-2.0.so.0 | head -n 1) $OUTDIR/bin
cp $(find /usr/lib -name libgthread-2.0.so.0 | head -n 1) $OUTDIR/bin
cp $(find /usr/lib -name libglib-2.0.so.0 | head -n 1) $OUTDIR/bin
cp $(find /usr/lib -name libffi.so.6 | head -n 1) $OUTDIR/bin

mkdir -p $OUTDIR/bin/platforms
cp $QTDIR/plugins/platforms/libqxcb.so $OUTDIR/bin/platforms/
cp $QTDIR/lib/libicu*.so* $OUTDIR/bin
cp -d $QTDIR/lib/libQt5{Core,Gui,Network,Sql,Xml,Widgets,PrintSupport,Help,WebKit,WebKitWidgets,CLucene,Qml,Quick,QuickWidgets,Positioning,Multimedia,MultimediaQuick_p,MultimediaWidgets,Sensors,WebChannel,Concurrent,DBus,Declarative,Designer,DesignerComponents,Location,OpenGL,Script,ScriptTools,XmlPatterns}.so* $OUTDIR/bin
#rm $OUTDIR/bin/*.debug
cp -d $QTDIR/bin/assistant $OUTDIR/bin
cp -d bin/libzyzzyva.so* $OUTDIR/bin
cp -d bin/zyzzyva $OUTDIR/bin
cp -d installer/zyzzyva.sh $OUTDIR/zyzzyva
chmod ugo+x $OUTDIR/zyzzyva

cp zyzzyva.top $OUTDIR
cp AUTHORS $OUTDIR/AUTHORS
cp CHANGES $OUTDIR/CHANGES
cp COPYING $OUTDIR/COPYING
cp LICENSE $OUTDIR/LICENSE
cp README $OUTDIR/README

mkdir -p $OUTDIR/images
cp images/zyzzyva-32x32.png $OUTDIR/images

mkdir -p $OUTDIR/bin/sqldrivers
cp $QTDIR/plugins/sqldrivers/libqsqlite.so $OUTDIR/bin/sqldrivers

cp -r data $OUTDIR
find $OUTDIR/data -type d -name '.svn' -print0 | xargs -0 rm -rf

pushd $INSTDIR
tar -czf $PLATFORM.tar.gz $PLATFORM
popd

#perl -pi -e "\$f = '$OUTDIR';s/\@\@EXECUTABLE\@\@/zyzzyva/g; s/\@\@INSTALLDIR\@\@/\$f/g;" $OUTDIR/zyzzyva.sh
