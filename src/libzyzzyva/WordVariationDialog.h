//---------------------------------------------------------------------------
// WordVariationDialog.h
//
// A dialog for displaying the front and back hooks of a word.
//
// Copyright 2015 Twilight Century Computing.
// Copyright 2005-2012 North American SCRABBLE Players Association.
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

#include "SearchSpec.h"
#include "WordTableModel.h"
#include "WordVariationType.h"
#include <QDialog>
#include <QList>
#include <QLabel>

class DefinitionLabel;
class WordEngine;
class WordTableView;
class ZPushButton;

class WordVariationDialog : public QDialog
{
    Q_OBJECT
    public:
    WordVariationDialog(WordEngine* we, const QString& lex, const QString&
                        word, WordVariationType variation, QWidget* parent =
                        0, Qt::WindowFlags f = 0);
    ~WordVariationDialog();
    WordTableView* getTopView() { return topView; }
    WordTableView* getMiddleView() { return middleView; }
    WordTableView* getBottomView() { return bottomView; }

    private:
    void setWordVariation(const QString& word, WordVariationType variation);
    QList<WordTableModel::WordItem> getWordItems(const QList<SearchSpec>&
                                                 searchSpecs) const;
    int getNumLists(WordVariationType variation);

    private:
    WordEngine*      wordEngine;
    QString          lexicon;
    DefinitionLabel* wordLabel;
    QLabel*          topLabel;
    QLabel*          middleLabel;
    QLabel*          bottomLabel;
    WordTableView*   topView;
    WordTableModel*  topModel;
    WordTableView*   middleView;
    WordTableModel*  middleModel;
    WordTableView*   bottomView;
    WordTableModel*  bottomModel;
    ZPushButton*     closeButton;
};

#endif // ZYZZYVA_WORD_VARIATION_DIALOG_H

