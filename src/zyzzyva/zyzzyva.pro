#---------------------------------------------------------------------------
# zyzzyva.pro
#
# Build configuration file for Zyzzyva tests using qmake.
#
# Copyright 2015 Twilight Century Computing.
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
#---------------------------------------------------------------------------

TEMPLATE = app
TARGET = zyzzyva
CONFIG += qt thread warn_on assistant
QT += sql xml network widgets gui webkit webkitwidgets printsupport
#CONFIG += qt thread warn_on assistant
#QT += xml gui

ROOT = ../..
DESTDIR = $$ROOT/bin
MOC_DIR = build/moc
OBJECTS_DIR = build/obj
INCLUDEPATH += build/moc $$ROOT/src/libzyzzyva
DEPENDPATH += build/moc

include($$ROOT/zyzzyva.pri)

RESOURCES = $$ROOT/zyzzyva.qrc
macx {
    TARGET = Zyzzyva
    RC_FILE = $$ROOT/images/Zyzzyva.icns
}

unix {
    LIBS = -lzyzzyva -L$$ROOT/bin
}
win32 {
    LIBS = -lzyzzyva5 -L$$ROOT/bin
    RC_ICONS += $$ROOT/images/Zyzzyva.ico
}

# Source files
SOURCES = \
    zyzzyva.cpp
