//---------------------------------------------------------------------------
// LexiconStyle.h
//
// A class to represent a specialized display style for words that belong to a
// particular combination of lexicons.
//
// Copyright 2015-2016 Twilight Century Computing.
// Copyright 2008-2012 North American SCRABBLE Players Association.
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

#ifndef ZYZZYVA_LEXICON_STYLE_H
#define ZYZZYVA_LEXICON_STYLE_H

//#include <QColor>
//#include <QFont>
#include <QString>

class LexiconStyle
{
    public:
    LexiconStyle() : inCompareLexicon(false) { }
    bool isValid() const {
        return (!lexicon.isEmpty() && !compareLexicon.isEmpty() &&
            !symbol.isEmpty() && (lexicon != compareLexicon));
    }
    bool operator==(const LexiconStyle& other) {
        return ((lexicon == other.lexicon) &&
                (compareLexicon == other.compareLexicon) &&
                (inCompareLexicon == other.inCompareLexicon) &&
                (symbol == other.symbol));
    }

    public:
    QString lexicon;
    QString compareLexicon;
    bool    inCompareLexicon;
    QString symbol;
    //QFont   font;
    //QColor  color;
};

#endif // ZYZZYVA_LEXICON_STYLE_H
