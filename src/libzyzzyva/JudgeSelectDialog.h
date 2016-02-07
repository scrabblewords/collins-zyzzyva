//---------------------------------------------------------------------------
// JudgeSelectDialog.h
//
// A dialog for selecting a word judge lexicon.
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

#ifndef ZYZZYVA_JUDGE_SELECT_DIALOG_H
#define ZYZZYVA_JUDGE_SELECT_DIALOG_H

#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>

class LexiconSelectWidget;

class JudgeSelectDialog : public QDialog
{
    Q_OBJECT
    public:
    JudgeSelectDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);
    ~JudgeSelectDialog();

    QString getLexicon() const;
    QString getPassword() const;

    public slots:
    void passwordStateChanged(int state);
    void showTypingStateChanged(int state);
    void passwordTextChanged();

    private:
    void setPasswordAreaEnabled(bool enable);

    private:
    LexiconSelectWidget* lexiconWidget;
    QCheckBox* passwordCbox;
    QCheckBox* passwordShowTypingCbox;
    QLabel* passwordLabel;
    QLineEdit* passwordLine;
    QLabel* confirmPasswordLabel;
    QLineEdit* confirmPasswordLine;
    QLabel* messageLabel;
    QDialogButtonBox* buttonBox;
};

#endif // ZYZZYVA_JUDGE_SELECT_DIALOG_H

