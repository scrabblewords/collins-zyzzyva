//---------------------------------------------------------------------------
// JudgeDialog.h
//
// A full-screen dialog for Word Judge functionality, in which the user can
// very easily judge the validity of one or more words.
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

#ifndef ZYZZYVA_JUDGE_DIALOG_H
#define ZYZZYVA_JUDGE_DIALOG_H

#include <QDialog>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QStackedWidget>
#include <QTimer>
#include <QWidget>

class WordEngine;
class WordTextEdit;

class JudgeDialog : public QDialog
{
    Q_OBJECT
    public:
    JudgeDialog(WordEngine* e, const QString& lex, const QString& pass =
        QString(), QWidget* parent = 0, Qt::WindowFlags f = 0);

    public slots:
    void textChanged();
    void currentChanged(int index);
    void clearResults();
    void judgeWord();
    void clearAltPressed();
    void clearResultsReleaseHold();
    void displayCount();
    void displayPassword();
    void displayIncorrectPassword();
    void displayInput();
    void clearInput();
    void clearExit();
    void passwordTextChanged();
    void passwordReturnPressed();

    protected:
    virtual bool eventFilter(QObject* object, QEvent* event);
    virtual void closeEvent(QCloseEvent* event);

    protected slots:
    void keyPressEvent(QKeyEvent* e);

    private:
    QString getInstructionMessage();
    QWidget* createTitleWidget();

    private:
    WordEngine*     engine;
    QString         lexicon;
    QString         password;
    bool            altPressed;
    int             count;
    QStackedWidget* widgetStack;
    QWidget*        countWidget;
    QLabel*         countInstLabel;
    QLabel*         countLabel;
    QWidget*        passwordWidget;
    QLabel*         passwordLabel;
    QLineEdit*      passwordLine;
    QLabel*         passwordResultLabel;
    QWidget*        inputWidget;
    QLabel*         inputInstLabel;
    WordTextEdit*   inputArea;
    QFrame*         resultWidget;
    QLabel*         resultPixmapLabel;
    QLabel*         resultLabel;
    QLabel*         resultLexiconLabel;
    QTimer*         altPressedTimer;
    QTimer*         countTimer;
    QTimer*         passwordTimer;
    QTimer*         incorrectPasswordTimer;
    QTimer*         inputTimer;
    QTimer*         resultTimer;
    QTimer*         exitTimer;
    int             clearResultsHold;

    int fontMultiplier;
};

#endif // ZYZZYVA_JUDGE_DIALOG_H
