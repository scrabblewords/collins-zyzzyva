//---------------------------------------------------------------------------
// MainWindow.h
//
// The main window for the word study application.
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

#ifndef ZYZZYVA_MAIN_WINDOW_H
#define ZYZZYVA_MAIN_WINDOW_H

#include "CardboxRescheduleType.h"
#include "WordVariationDialog.h"
#include <QAction>
#include <QCloseEvent>
#include <QIcon>
#include <QLabel>
#include <QMainWindow>
#include <QSettings>
#include <QSplashScreen>
#include <QTabWidget>
#include <QToolButton>

class AboutDialog;
class ActionForm;
//class HelpDialog;
class QuizSpec;
class QuizEngine;
class WordEngine;
class SettingsDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    public:
    MainWindow(QWidget* parent = 0, QSplashScreen* splash = 0,
               Qt::WindowFlags f = 0);
    ~MainWindow() { }

    static MainWindow* getInstance() { return instance; }

    public slots:
    void fileOpenRequested(const QString& filename);
    void processArguments(const QStringList& args);
    void tryAutoImport();
    void tryConnectToDatabases();
    void processDatabaseErrors();
    void importInteractive();
    void newQuizFormInteractive();
    void newQuizFormInteractive(const QuizSpec& quizSpec);
    void newQuizForm(const QuizSpec& quizSpec);
    void newSearchForm();
    void newCrosswordGameForm();
    void newDefineForm();
    void newIntroForm();
    void newCardboxForm();
    void doSaveAction();
    void doSaveAsAction();
    void doPrintAction();
    void doJudgeAction();
    void editSettings();
    void viewDefinition();
    void viewVariation(int variation);
    void rebuildDatabaseRequested();
    void rescheduleCardboxRequested();
    void displayAbout();
    void displayHelp();
    void displayLexiconError();
    //void helpDialogError(const QString& message);
    void closeCurrentTab();
    void currentTabChanged(int index);
    void tabTitleChanged(const QString& title);
    void tabStatusChanged(const QString& status);
    void tabDetailsChanged(const QString& details);
    void tabSaveEnabledChanged(bool saveEnabled);
    void tabPrintEnabledChanged(bool printEnabled);

    void doTest();

    public:
    QString getLexiconPrefix(const QString& lexicon);
    QString getDatabaseFilename(const QString& lexicon);
    int tryConnectToDatabase(const QString& lexicon);
    bool connectToDatabase(const QString& lexicon);
    // FIXME: these probably belong with WordTableView::addToCardbox in a
    // separate class for manipulating quiz databases.  Hm, how about the
    // QuizDatabase class?
    void rebuildDatabases(const QStringList& lexicons);
    bool rebuildDatabase(const QString& lexicon);
    int rescheduleCardbox(const QStringList& words, const QString& lexicon,
        const QString& quizType, CardboxRescheduleType rescheduleType,
        int rescheduleValue = 0) const;
    void readSettings(bool useGeometry);

    protected:
    virtual void closeEvent(QCloseEvent* event);

    private:
    void setSplashMessage(const QString& message);
    void fixTrolltechConfig();
    void updateSettings();
    void makeUserDirs();
    void renameLexicon(const QString& oldName, const QString& newName);
    bool importLexicon(const QString& lexicon);
    int importText(const QString& lexicon, const QString& file);
    bool importDawg(const QString& lexicon, const QString& file,
                    bool reverse = false, QString* errString = 0,
                    quint16* expectedChecksum = 0);
    QList<quint16> importChecksums(const QString& file);
    int importStems(const QString& lexicon);
    void writeSettings();
    void newTab(ActionForm* form);
    void newQuizFromQuizFile(const QString& filename);
    void newQuizFromWordFile(const QString& filename);
    void copyQActionPartial(const QAction* orig, QAction* dest);

    private slots:
    void clearDialogFromList(QObject* obj);

    private:
    enum LexiconDatabaseError {
        DbNoError = 0,
        DbOutOfDate,
        DbOpenError,
        DbConnectionError,
        DbDoesNotExist,
        DbSymbolsOutOfDate
    };

    private:
    QSplashScreen* splashScreen;
    WordEngine*  wordEngine;
    QTabWidget*  tabStack;
    QToolButton* closeButton;
    QLabel*      messageLabel;
    QLabel*      detailsLabel;

    QAction*     saveAction;
    QAction*     saveAsAction;
    QAction*     printAction;
    QAction*     toolbarSaveAction;
    QAction*     toolbarSaveAsAction;
    QAction*     toolbarPrintAction;

    SettingsDialog* settingsDialog;
    AboutDialog*    aboutDialog;

    QString lexiconError;
    QMap<QString, int> dbErrors;
    QList<WordVariationDialog*> wordVariationDialogs;

    static MainWindow*  instance;
};

#endif // ZYZZYVA_MAIN_WINDOW_H
