//---------------------------------------------------------------------------
// LoadDefinitionsThread.cpp
//
// A class for loading definitions in the background.
//
// Copyright 2006 Michael W Thelen <mike@pietdepsi.com>.
//
// This file is part of Zyzzyva.
//
// Zyzzyva is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// Zyzzyva is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//---------------------------------------------------------------------------

#include "LoadDefinitionsThread.h"
#include "Defs.h"
#include <QFile>
#include <map>

using namespace std;
using namespace Defs;

//---------------------------------------------------------------------------
//  run
//
//! Load definitions from a text file.
//---------------------------------------------------------------------------
void
LoadDefinitionsThread::run()
{
    QFile file (filename);
    if (!file.open (QIODevice::ReadOnly | QIODevice::Text))
        return;

    std::map<QString, std::multimap<QString, QString> > definitions;

    char* buffer = new char [MAX_INPUT_LINE_LEN];
    while (file.readLine (buffer, MAX_INPUT_LINE_LEN) > 0) {
        QString line (buffer);
        line = line.simplified();
        if (!line.length() || (line.at (0) == '#'))
            continue;

        QString word = line.section (' ', 0, 0).toUpper();
        QString definition = line.section (' ', 1);
        addDefinition (word, definition);
    }

    emit loaded();
}

//---------------------------------------------------------------------------
//  addDefinition
//
//! Add a word with its definition.  Parse the definition and separate its
//! parts of speech.
//
//! @param word the word
//! @param definition the definition
//! @param definitions the definition map
//---------------------------------------------------------------------------
void
LoadDefinitionsThread::addDefinition (const QString& word, const QString&
                                      definition)
{
    if (word.isEmpty() || definition.isEmpty())
        return;

    QRegExp posRegex (QString ("\\[(\\w+)"));
    multimap<QString, QString> defMap;
    QStringList defs = definition.split (" / ");
    QString def;
    foreach (def, defs) {
        QString pos;
        if (posRegex.indexIn (def, 0) >= 0) {
            pos = posRegex.cap (1);
        }
        defMap.insert (make_pair (pos, def));
    }
    definitions.insert (make_pair (word, defMap));
}