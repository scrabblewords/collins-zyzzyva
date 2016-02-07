//---------------------------------------------------------------------------
// WordAttribute.h
//
// An enumeration of word attributes.
//
// Copyright 2016 Twilight Century Computing.
// Copyright 2006-2012 North American SCRABBLE Players Association.
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

#ifndef ZYZZYVA_WORD_ATTRIBUTE_H
#define ZYZZYVA_WORD_ATTRIBUTE_H

enum WordAttribute
{
    WordAttrInvalid = 0,
    WordAttrWord,
    WordAttrDefinition,
    WordAttrFrontHooks,
    WordAttrBackHooks,
    WordAttrInnerHooks,
    WordAttrLexiconSymbols,
    WordAttrProbabilityOrder,
    WordAttrPlayabilityOrder,
    WordAttrFrontExtensions,
    WordAttrBackExtensions,
    WordAttrDoubleExtensions,
    WordAttrAlphagram
};

#endif // ZYZZYVA_WORD_ATTRIBUTE_H

