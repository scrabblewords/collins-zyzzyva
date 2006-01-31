//---------------------------------------------------------------------------
// WordVariationDialog.h
//
// A dialog for displaying the front and back hooks of a word.
//
// Copyright 2005, 2006 Michael W Thelen <mike@pietdepsi.com>.
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

#ifndef ZYZZYVA_WORD_VARIATION_DIALOG_H
#define ZYZZYVA_WORD_VARIATION_DIALOG_H

#include "WordVariationType.h"
#include <QDialog>
#include <QLabel>

class DefinitionLabel;
class WordEngine;
class WordTableModel;
class WordTreeView;
class ZPushButton;

class WordVariationDialog : public QDialog
{
    Q_OBJECT
    public:
    WordVariationDialog (WordEngine* we, const QString& word,
                         WordVariationType variation, QWidget* parent = 0,
                         Qt::WFlags f = 0);
    ~WordVariationDialog();

    private:
    void setWordVariation (const QString& word, WordVariationType variation);
    bool needsRightList (WordVariationType variation);

    private:
    WordEngine*      wordEngine;
    DefinitionLabel* wordLabel;
    QLabel*          leftLabel;
    QLabel*          rightLabel;
    WordTreeView*    leftView;
    WordTableModel*  leftModel;
    WordTreeView*    rightView;
    WordTableModel*  rightModel;
    ZPushButton*     closeButton;
};

#endif // ZYZZYVA_WORD_VARIATION_DIALOG_H

