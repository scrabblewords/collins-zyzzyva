//---------------------------------------------------------------------------
// DefineForm.h
//
// A form for looking up and displaying word definitions.
//
// Copyright 2015 Twilight Century Computing.
// Copyright 2004-2012 North American SCRABBLE Players Association.
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

#ifndef ZYZZYVA_DEFINE_FORM_H
#define ZYZZYVA_DEFINE_FORM_H

#include "ActionForm.h"
#include "ZPushButton.h"

class DefinitionBox;
class LexiconSelectWidget;
class WordEngine;
class WordLineEdit;

class DefineForm : public ActionForm
{
    Q_OBJECT
    public:
    DefineForm(WordEngine* e, QWidget* parent = 0, Qt::WindowFlags f = 0);
    QIcon getIcon() const;
    QString getTitle() const;
    QString getStatusString() const;
    QString getDetailsString() const;

    // Reimplemented virtual methods
    public slots:
    void selectInputArea();

    public slots:
    void lexiconActivated(const QString& lexicon);
    void wordChanged(const QString& word);
    void displayDefinition();
    void displayAllInfo();

    private:
    void displayInfo(bool allInfo = false);

    private:
    WordEngine*    engine;
    LexiconSelectWidget* lexiconWidget;
    WordLineEdit*  wordLine;
    ZPushButton*   defineButton;
    ZPushButton*   infoButton;
    DefinitionBox* resultBox;

    QString detailsString;
};

#endif // ZYZZYVA_DEFINE_FORM_H
