#---------------------------------------------------------------------------
# encrypt.pro
#
# Build configuration file for Zyzzyva using qmake.
#
# Copyright 2015-2016 Twilight Century Computing.
# Copyright 2005-2012 North American SCRABBLE Players Association.
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
TARGET = EncryptLexicon
CONFIG   += console
CONFIG   -= app_bundle
QT       += core
QT       -= gui

#ROOT = ../..
#DESTDIR = $$ROOT/bin
#MOC_DIR = build/moc
#OBJECTS_DIR = build/obj
#INCLUDEPATH += build/moc $$ROOT/src/libzyzzyva
#DEPENDPATH += build/moc

SOURCES += \
    EncryptLexicon.cpp \
    ../src/simplecrypt/simplecrypt.cpp

HEADERS += \
    ../src/simplecrypt/simplecrypt.h
