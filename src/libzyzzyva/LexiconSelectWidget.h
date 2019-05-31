//---------------------------------------------------------------------------
// LexiconSelectWidget.h
//
// A simple widget for selecting a lexicon from the available lexicons.
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

#ifndef ZYZZYVA_LEXICON_SELECT_WIDGET_H
#define ZYZZYVA_LEXICON_SELECT_WIDGET_H

#include <QComboBox>
#include <QLabel>
#include <QWidget>

class LexiconSelectWidget : public QWidget
{
    Q_OBJECT
    public:
    LexiconSelectWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
    ~LexiconSelectWidget();

    void setFont(const QFont& font);
    void setLabelVisible(bool visible);

    const QComboBox* getComboBox() const { return lexiconCombo; }
    bool setCurrentLexicon(const QString& lexicon);
    QString getCurrentLexicon() const;

    public slots:
    void updateLexicons();

    private:
    QLabel* lexiconLabel;
    QComboBox* lexiconCombo;
};

#endif // ZYZZYVA_LEXICON_SELECT_WIDGET_H

