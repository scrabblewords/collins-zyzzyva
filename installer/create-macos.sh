#!/bin/sh

export QTDIR=/usr/local/opt/qt
export PATH=$QTDIR/bin:$PATH
export CPPFLAGS=-I$QTDIR/include
export LDFLAGS=-L$QTDIR/lib
export PKG_CONFIG_PATH=$QTDIR/lib/pkgconfig

qmake
make
cd bin
mv libzyzzyva5*dylib Zyzzyva.app/Contents/MacOS/
install_name_tool -change libzyzzyva5.1.dylib @executable_path/libzyzzyva5.1.dylib Zyzzyva.app/Contents/MacOS/Zyzzyva
cp $QTDIR/libexec/Assistant.app/Contents/MacOS/Assistant Zyzzyva.app/Contents/MacOS
install_name_tool -add_rpath '@loader_path/../Frameworks' Zyzzyva.app/Contents/MacOS/Assistant
macdeployqt Zyzzyva.app -executable=Zyzzyva.app/Contents/MacOS/Assistant
cp -R ../data Zyzzyva.app/Contents/MacOS
rm -f Collins-Zyzzyva-5.1.0.zip
zip -r Collins-Zyzzyva-5.1.0.zip Zyzzyva.app
zip -j Collins-Zyzzyva-5.1.0.zip ../installer/MacOS-Installation-Instructions.txt
