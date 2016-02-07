//---------------------------------------------------------------------------
// LexiconStyleDialog.h
//
// A dialog for selecting specialized display styles for words that belong to
// a particular combination of lexicons.
//
// Copyright 2016 Twilight Century Computing.
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

#ifndef ZYZZYVA_LEXICON_STYLE_DIALOG_H
#define ZYZZYVA_LEXICON_STYLE_DIALOG_H

#include <QDialog>
#include <QList>
#include <QSignalMapper>
#include <QVBoxLayout>

class LexiconStyle;
class LexiconStyleWidget;

class LexiconStyleDialog : public QDialog
{
    Q_OBJECT
    public:
    LexiconStyleDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);
    ~LexiconStyleDialog();

    QList<LexiconStyle> getLexiconStyles() const;
    void setLexiconStyles(const QList<LexiconStyle>& styles);

    public slots:
    void addButtonClicked();
    void insertStyleWidget(int index);
    void removeStyleWidget(int index);

    private:
    QVBoxLayout* symbolVlay;
    QList<LexiconStyleWidget*> styleWidgets;
    QSignalMapper* deleteMapper;
};

#endif // ZYZZYVA_LEXICON_STYLE_DIALOG_H

