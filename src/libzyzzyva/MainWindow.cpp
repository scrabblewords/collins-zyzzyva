//---------------------------------------------------------------------------
// MainWindow.cpp
//
// The main window for the word study application.
//
// Copyright 2015-2016 Twilight Century Computing.
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

#include "MainWindow.h"
#include "AboutDialog.h"
#include "AnalyzeQuizDialog.h"
#include "CardboxForm.h"
#include "CardboxRescheduleDialog.h"
#include "CreateDatabaseThread.h"
#include "CrosswordGameForm.h"
#include "DatabaseRebuildDialog.h"
#include "DefinitionDialog.h"
#include "DefineForm.h"
//#include "HelpDialog.h"
#include "IntroForm.h"
#include "JudgeDialog.h"
#include "JudgeSelectDialog.h"
#include "LexiconSelectDialog.h"
#include "MainSettings.h"
#include "NewQuizDialog.h"
#include "QuizEngine.h"
#include "QuizForm.h"
#include "SearchForm.h"
#include "SettingsDialog.h"
#include "WordEngine.h"
#include "WordEntryDialog.h"
#include "WordTableView.h"
#include "WordVariationDialog.h"
#include "WordVariationType.h"
#include "Auxil.h"
#include "Defs.h"
#include <QApplication>
#include <QDir>
#include <QFileDialog>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QProcess>
#include <QProgressDialog>
#include <QSignalMapper>
#include <QStatusBar>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QToolBar>

#include "LetterBag.h"

MainWindow* MainWindow::instance = 0;

const QString APPLICATION_TITLE = "Collins Zyzzyva 5.1.1";

const QString IMPORT_FAILURE_TITLE = "Load Failed";
const QString IMPORT_COMPLETE_TITLE = "Load Complete";

const QString SETTINGS_MAIN = "/Collins Zyzzyva";
const QString SETTINGS_GEOMETRY = "/geometry";
const QString SETTINGS_GEOMETRY_X = "/x";
const QString SETTINGS_GEOMETRY_Y = "/y";
const QString SETTINGS_GEOMETRY_WIDTH = "/width";
const QString SETTINGS_GEOMETRY_HEIGHT = "/height";

const int DETAILS_FONT_MIN_POINTS = 6;

const QColor SPLASH_MESSAGE_COLOR = QColor("#fff7fa09");

using namespace Defs;

//---------------------------------------------------------------------------
//  MainWindow
//
//! Constructor.
//
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
MainWindow::MainWindow(QWidget* parent, QSplashScreen* splash, Qt::WindowFlags f)
    : QMainWindow(parent, f), splashScreen(splash),
      wordEngine(new WordEngine()), settingsDialog(new SettingsDialog(this)),
      aboutDialog(new AboutDialog(this))//,
      //helpDialog(new HelpDialog(QString(), this))
{
    setSplashMessage("Creating interface...", SPLASH_MESSAGE_COLOR);
    //printf("%d\n", qApp->cursorFlashTime());
    // (JGM) Hack to properly display the cursor on Windows (7-specific?) when cursor blink is turned
    // off.  Qt 5.4/5.5 thinks the value is -2ms instead of 0ms, causing inconsistent cursor display
    // across different GUI events.  Blinkophiles won't notice a 2ms difference in the blink rate when
    // using this application.
    // (JGM) As of Qt 5.6.0, hack is no longer needed.
    //qApp->setCursorFlashTime(qApp->cursorFlashTime() + 2);
    //printf("%d\n", qApp->cursorFlashTime());

    // File Menu
    QMenu* fileMenu = menuBar()->addMenu("&File");
    Q_CHECK_PTR(fileMenu);

    // Test action (only for testing things out)
    //QAction* testAction = new QAction("&Test", this);
    //Q_CHECK_PTR(testAction);
    //connect(testAction, SIGNAL(triggered()), SLOT(doTest()));
    //fileMenu->addAction(testAction);

    // Save
    saveAction = new QAction("&Save", this);
    Q_CHECK_PTR(saveAction);
    saveAction->setIcon(QIcon(":/save-icon"));
    saveAction->setEnabled(false);
    saveAction->setShortcut(QString("Ctrl+S"));
    connect(saveAction, SIGNAL(triggered()), SLOT(doSaveAction()));
    fileMenu->addAction(saveAction);

    // Save As
    saveAsAction = new QAction("Save &As...", this);
    Q_CHECK_PTR(saveAsAction);
    saveAsAction->setIcon(QIcon(":/save-as-icon"));
    saveAsAction->setEnabled(false);
    saveAsAction->setShortcut(QString("Ctrl+Shift+S"));
    connect(saveAsAction, SIGNAL(triggered()), SLOT(doSaveAsAction()));
    fileMenu->addAction(saveAsAction);

    fileMenu->addSeparator();

    // New Search
    QAction* newSearchAction = new QAction("&Search", this);
    Q_CHECK_PTR(newSearchAction);
    newSearchAction->setIcon(QIcon(":/search-icon"));
    connect(newSearchAction, SIGNAL(triggered()), SLOT(newSearchForm()));
    fileMenu->addAction(newSearchAction);

    // New Quiz
    QAction* newQuizAction = new QAction("Qui&z...", this);
    Q_CHECK_PTR(newQuizAction);
    newQuizAction->setIcon(QIcon(":/quiz-icon"));
    connect(newQuizAction, SIGNAL(triggered()), SLOT(newQuizFormInteractive()));
    fileMenu->addAction(newQuizAction);

    // New Cardbox
    QAction* newCardboxAction = new QAction("&Cardbox", this);
    Q_CHECK_PTR(newCardboxAction);
    newCardboxAction->setIcon(QIcon(":/cardbox-icon"));
    connect(newCardboxAction, SIGNAL(triggered()), SLOT(newCardboxForm()));
    fileMenu->addAction(newCardboxAction);

    // New Crossword Game
    //QAction* newCrosswordGameAction = new QAction("Crossword &Game", this);
    //Q_CHECK_PTR(newCrosswordGameAction);
    //newCrosswordGameAction->setIcon(QIcon(":/define-icon"));
    //connect(newCrosswordGameAction, SIGNAL(triggered()),
    //         SLOT(newCrosswordGameForm()));
    //fileMenu->addAction(newCrosswordGameAction);

    // New Definition
    QAction* newDefinitionAction = new QAction("&Definition", this);
    Q_CHECK_PTR(newDefinitionAction);
    newDefinitionAction->setIcon(QIcon(":/define-icon"));
    connect(newDefinitionAction, SIGNAL(triggered()), SLOT(newDefineForm()));
    fileMenu->addAction(newDefinitionAction);

    // New Word Judge
    QAction* newJudgeAction = new QAction("Word &Judge", this);
    Q_CHECK_PTR(newJudgeAction);
    newJudgeAction->setIcon(QIcon(":/judge-icon"));
    connect(newJudgeAction, SIGNAL(triggered()), SLOT(doJudgeAction()));
    fileMenu->addAction(newJudgeAction);

    fileMenu->addSeparator();

    // Print
    printAction = new QAction("&Print...", this);
    Q_CHECK_PTR(printAction);
    printAction->setIcon(QIcon(":/print-icon"));
    printAction->setEnabled(false);
    printAction->setShortcut(QString("Ctrl+P"));
    connect(printAction, SIGNAL(triggered()), SLOT(doPrintAction()));
    fileMenu->addAction(printAction);

    fileMenu->addSeparator();

    // Close Tab
    closeTabAction = new QAction("&Close Tab", this);
    Q_CHECK_PTR(closeTabAction);
    closeTabAction->setEnabled(false);
    closeTabAction->setShortcut(QString("Ctrl+W"));
    connect(closeTabAction, SIGNAL(triggered()), SLOT(closeCurrentTab()));
    fileMenu->addAction(closeTabAction);

    // Quit
    QAction* quitAction = new QAction("&Exit", this);
    Q_CHECK_PTR(quitAction);
    quitAction->setEnabled(true);
    quitAction->setShortcut(QString("Ctrl+Q"));
    connect(quitAction, SIGNAL(triggered()), SLOT(close()));
    fileMenu->addAction(quitAction);

    // Word Menu
    QMenu* wordMenu = menuBar()->addMenu("&Word");
    Q_CHECK_PTR(wordMenu);

    // Word Definition
    QAction* viewDefinitionAction = new QAction("&Definition...", this);
    Q_CHECK_PTR(viewDefinitionAction);
    connect(viewDefinitionAction, SIGNAL(triggered()),
             SLOT(viewDefinition()));
    wordMenu->addAction(viewDefinitionAction);

    QSignalMapper* wordMapper = new QSignalMapper(this);
    Q_CHECK_PTR(wordMapper);

    // Word Anagrams
    QAction* viewAnagramsAction = new QAction("&Anagrams...", this);
    Q_CHECK_PTR(viewAnagramsAction);
    connect(viewAnagramsAction, SIGNAL(triggered()), wordMapper, SLOT(map()));
    wordMapper->setMapping(viewAnagramsAction, VariationAnagrams);
    wordMenu->addAction(viewAnagramsAction);

    // Word Subanagrams
    QAction* viewSubanagramsAction = new QAction("&Subanagrams...", this);
    Q_CHECK_PTR(viewSubanagramsAction);
    connect(viewSubanagramsAction, SIGNAL(triggered()),
            wordMapper, SLOT(map()));
    wordMapper->setMapping(viewSubanagramsAction, VariationSubanagrams);
    wordMenu->addAction(viewSubanagramsAction);

    // Word Hooks
    QAction* viewHooksAction = new QAction("&Hooks...", this);
    Q_CHECK_PTR(viewHooksAction);
    connect(viewHooksAction, SIGNAL(triggered()), wordMapper, SLOT(map()));
    wordMapper->setMapping(viewHooksAction, VariationHooks);
    wordMenu->addAction(viewHooksAction);

    // Word Extensions
    QAction* viewExtensionsAction = new QAction("&Extensions...", this);
    Q_CHECK_PTR(viewExtensionsAction);
    connect(viewExtensionsAction, SIGNAL(triggered()),
            wordMapper, SLOT(map()));
    wordMapper->setMapping(viewExtensionsAction, VariationExtensions);
    wordMenu->addAction(viewExtensionsAction);

    // Word Anagram Hooks
    QAction* viewAnagramHooksAction = new QAction("Anagram Hoo&ks...", this);
    Q_CHECK_PTR(viewAnagramHooksAction);
    connect(viewAnagramHooksAction, SIGNAL(triggered()),
            wordMapper, SLOT(map()));
    wordMapper->setMapping(viewAnagramHooksAction, VariationAnagramHooks);
    wordMenu->addAction(viewAnagramHooksAction);

    // Word Blank Anagrams
    QAction* viewBlankAnagramsAction = new QAction("&Blank Anagrams...", this);
    Q_CHECK_PTR(viewBlankAnagramsAction);
    connect(viewBlankAnagramsAction, SIGNAL(triggered()),
            wordMapper, SLOT(map()));
    wordMapper->setMapping(viewBlankAnagramsAction, VariationBlankAnagrams);
    wordMenu->addAction(viewBlankAnagramsAction);

    // Word Blank Matches
    QAction* viewBlankMatchesAction = new QAction("Blank &Matches...", this);
    Q_CHECK_PTR(viewBlankMatchesAction);
    connect(viewBlankMatchesAction, SIGNAL(triggered()),
            wordMapper, SLOT(map()));
    wordMapper->setMapping(viewBlankMatchesAction, VariationBlankMatches);
    wordMenu->addAction(viewBlankMatchesAction);

    // Word Transpositions
    QAction* viewTranspositionsAction = new QAction("&Transpositions...", this);
    Q_CHECK_PTR(viewTranspositionsAction);
    connect(viewTranspositionsAction, SIGNAL(triggered()),
            wordMapper, SLOT(map()));
    wordMapper->setMapping(viewTranspositionsAction, VariationTranspositions);
    wordMenu->addAction(viewTranspositionsAction);

    // Connect Word signal mappings to viewVariation
    connect(wordMapper, SIGNAL(mapped(int)), SLOT(viewVariation(int)));

    // Tools Menu
    QMenu* toolsMenu = menuBar()->addMenu("&Tools");
    Q_CHECK_PTR(toolsMenu);

    QAction* rebuildDatabaseAction = new QAction("Rebuild &Database...", this);
    Q_CHECK_PTR(rebuildDatabaseAction);
    connect(rebuildDatabaseAction, SIGNAL(triggered()),
            SLOT(rebuildDatabaseRequested()));
    toolsMenu->addAction(rebuildDatabaseAction);

    QAction* rescheduleCardboxAction = new QAction("Reschedule &Cardbox "
                                                   "Contents...", this);
    Q_CHECK_PTR(rescheduleCardboxAction);
    connect(rescheduleCardboxAction, SIGNAL(triggered()),
            SLOT(rescheduleCardboxRequested()));
    toolsMenu->addAction(rescheduleCardboxAction);

    // Preferences
    QAction* editPrefsAction = new QAction("&Preferences", this);
    Q_CHECK_PTR(editPrefsAction);
    editPrefsAction->setIcon(QIcon(":/preferences-icon"));
    connect(editPrefsAction, SIGNAL(triggered()), SLOT(editSettings()));
    toolsMenu->addAction(editPrefsAction);

    // Help Menu
    QMenu* helpMenu = menuBar()->addMenu("&Help");
    Q_CHECK_PTR(helpMenu);

    // Help
    QAction* helpAction = new QAction("&Help", this);
    Q_CHECK_PTR(helpAction);
    helpAction->setIcon(QIcon(":/help-icon"));
    // Start using "Welcome" dialog solely for providing help!
    connect(helpAction, SIGNAL(triggered()), SLOT(newIntroForm()));
    helpMenu->addAction(helpAction);

    // About
    QAction* aboutAction = new QAction("&About", this);
    Q_CHECK_PTR(aboutAction);
    connect(aboutAction, SIGNAL(triggered()), SLOT(displayAbout()));
    helpMenu->addAction(aboutAction);

    // Tool Bar
    QToolBar* toolbar = new QToolBar;
    Q_CHECK_PTR(toolbar);
    toolbar->setIconSize(QSize(36, 36));
    // (JGM) Hacky, for disallowing the toolbar QActions' mnemonics to interfere with form mnemonics in Qt5.
    toolbarSaveAction = new QAction("Save", this);
    copyQActionPartial(saveAction, toolbarSaveAction);
    connect(toolbarSaveAction, SIGNAL(triggered()), SLOT(doSaveAction()));
    toolbar->addAction(toolbarSaveAction);
    toolbarSaveAsAction = new QAction("Save As...", this);
    copyQActionPartial(saveAsAction, toolbarSaveAsAction);
    connect(toolbarSaveAsAction, SIGNAL(triggered()), SLOT(doSaveAction()));
    toolbar->addAction(toolbarSaveAsAction);
    toolbar->addSeparator();
    QAction* toolbarSearchAction = new QAction("Search", this);
    copyQActionPartial(newSearchAction, toolbarSearchAction);
    connect(toolbarSearchAction, SIGNAL(triggered()), SLOT(newSearchForm()));
    toolbar->addAction(toolbarSearchAction);
    QAction* toolbarQuizAction = new QAction("Quiz", this);
    copyQActionPartial(newQuizAction, toolbarQuizAction);
    connect(toolbarQuizAction, SIGNAL(triggered()), SLOT(newQuizFormInteractive()));
    toolbar->addAction(toolbarQuizAction);
    QAction* toolbarCardboxAction = new QAction("Cardbox", this);
    copyQActionPartial(newCardboxAction, toolbarCardboxAction);
    connect(toolbarCardboxAction, SIGNAL(triggered()), SLOT(newCardboxForm()));
    toolbar->addAction(toolbarCardboxAction);
    QAction* toolbarDefinitionAction = new QAction("Definition", this);
    copyQActionPartial(newDefinitionAction, toolbarDefinitionAction);
    connect(toolbarDefinitionAction, SIGNAL(triggered()), SLOT(newDefineForm()));
    toolbar->addAction(toolbarDefinitionAction);
    QAction* toolbarJudgeAction = new QAction("Word Judge", this);
    copyQActionPartial(newJudgeAction, toolbarJudgeAction);
    connect(toolbarJudgeAction, SIGNAL(triggered()), SLOT(doJudgeAction()));
    toolbar->addAction(toolbarJudgeAction);
    toolbar->addSeparator();
    toolbarPrintAction = new QAction("Print...", this);
    copyQActionPartial(printAction, toolbarPrintAction);
    connect(toolbarPrintAction, SIGNAL(triggered()), SLOT(doPrintAction()));
    toolbar->addAction(toolbarPrintAction);
    toolbar->addSeparator();
    QAction* toolbarEditPrefsAction = new QAction("Preferences", this);
    copyQActionPartial(editPrefsAction, toolbarEditPrefsAction);
    connect(toolbarEditPrefsAction, SIGNAL(triggered()), SLOT(editSettings()));
    toolbar->addAction(toolbarEditPrefsAction);
    toolbar->addSeparator();
    QAction* toolbarHelpAction = new QAction("Help", this);
    copyQActionPartial(helpAction, toolbarHelpAction);
    // Start using "Welcome" dialog solely for providing help!
    connect(toolbarHelpAction, SIGNAL(triggered()), SLOT(newIntroForm()));
    toolbar->addAction(toolbarHelpAction);
    addToolBar(toolbar);

    tabStack = new QTabWidget(this);
    Q_CHECK_PTR(tabStack);
    connect(tabStack, SIGNAL(currentChanged(int)),
             SLOT(currentTabChanged(int)));

    closeButton = new QToolButton(tabStack);
    Q_CHECK_PTR(closeButton);
    closeButton->setIcon(QIcon(":/close-tab-icon"));
    tabStack->setCornerWidget(closeButton);
    closeButton->hide();
    connect(closeButton, SIGNAL(clicked()), SLOT(closeCurrentTab()));

    setCentralWidget(tabStack);

    messageLabel = new QLabel;
    Q_CHECK_PTR(messageLabel);
    statusBar()->addWidget(messageLabel, 1);

    QFont detailsFont = qApp->font();
    int detailsPoints = detailsFont.pointSize() - 1;
    if (detailsPoints < DETAILS_FONT_MIN_POINTS)
        detailsPoints = DETAILS_FONT_MIN_POINTS;
    detailsFont.setPointSize(detailsPoints);

    detailsLabel = new QLabel;
    Q_CHECK_PTR(detailsLabel);
    detailsLabel->setFont(detailsFont);
    detailsLabel->setTextFormat(Qt::RichText);
    detailsLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    detailsLabel->setOpenExternalLinks(true);
    statusBar()->addWidget(detailsLabel);

    fixTrolltechConfig();

    setSplashMessage("Reading settings...", SPLASH_MESSAGE_COLOR);
    readSettings(true);
    updateSettings();

    setSplashMessage("Creating data files...", SPLASH_MESSAGE_COLOR);
    makeUserDirs();

    setWindowTitle(APPLICATION_TITLE);
    setWindowIcon(QIcon(":/zyzzyva-128x128"));

    if (!instance)
        instance = this;

    if (MainSettings::getDisplayWelcome())
        newIntroForm();

    splashScreen = 0;
    QTimer::singleShot(0, this, SLOT(displayLexiconError()));
}

//---------------------------------------------------------------------------
//  fileOpenRequested
//
//! Called when a request is made to open a file.  Open the file and start
//! a quiz from it, if possible.
//
//! @param filename the filename
//---------------------------------------------------------------------------
void
MainWindow::fileOpenRequested(const QString& filename)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    if (filename.endsWith(".zzq"))
        newQuizFromQuizFile(filename);
    else
        newQuizFromWordFile(filename);

    QApplication::restoreOverrideCursor();
}

//---------------------------------------------------------------------------
//  processArguments
//
//! Process command-line arguments.
//
//! @param args a list of arguments
//---------------------------------------------------------------------------
void
MainWindow::processArguments(const QStringList& args)
{
    foreach (const QString& arg, args)
        fileOpenRequested(arg);
}

//---------------------------------------------------------------------------
//  tryAutoImport
//
//! Try automatically importing a lexicon, if the user has enabled it in
//! preferences.
//---------------------------------------------------------------------------
void
MainWindow::tryAutoImport()
{
    if (!MainSettings::getUseAutoImport())
        return;

    setSplashMessage("Loading lexicons...", SPLASH_MESSAGE_COLOR);

    // If no auto import lexicons are set, prompt the user with a lexicon
    // selection dialog
    QStringList lexicons = MainSettings::getAutoImportLexicons();
    if (lexicons.isEmpty()) {
        LexiconSelectDialog* dialog = new LexiconSelectDialog;
        Q_CHECK_PTR(dialog);

        QString defaultLexicon = MainSettings::getDefaultLexicon();
        lexicons << defaultLexicon;

        dialog->setImportLexicons(lexicons);
        dialog->setDefaultLexicon(defaultLexicon);

        int code = dialog->exec();
        if (code == QDialog::Accepted) {
            lexicons = dialog->getImportLexicons();
            MainSettings::setAutoImportLexicons(lexicons);
            MainSettings::setDefaultLexicon(dialog->getDefaultLexicon());
            writeSettings();
            settingsDialog->readSettings();
        }

        delete dialog;
    }

    // FIXME: This should not be part of the MainWindow class.  Lexicons (and
    // mapping lexicons to actual files) should be handled by someone else.
    QStringListIterator it (lexicons);
    while (it.hasNext()) {
        const QString& lexicon = it.next();
        importLexicon(lexicon);
    }
}

//---------------------------------------------------------------------------
//  tryConnectToDatabases
//
//! Try to connect to databases, but do not prompt the user to create ones
//! that do not exist or otherwise cannot be opened.
//---------------------------------------------------------------------------
void
MainWindow::tryConnectToDatabases()
{
    if (!MainSettings::getUseAutoImport())
        return;

    QStringList lexicons = MainSettings::getAutoImportLexicons();
    QStringListIterator it (lexicons);
    while (it.hasNext()) {
        const QString& lexicon = it.next();
        int error = tryConnectToDatabase(lexicon);
        if (error != DbNoError)
            dbErrors.insert(lexicon, error);
    }
}

//---------------------------------------------------------------------------
//  processDatabaseErrors
//
//! Try to connect to databases, but do not prompt the user to create ones
//! that do not exist.
//---------------------------------------------------------------------------
void
MainWindow::processDatabaseErrors()
{
    if (dbErrors.isEmpty())
        return;

    QString caption = "Build databases?";
    QString message =
        "For certain searches and quizzes to work correctly, Collins Zyzzyva "
        "must have a database of information about each lexicon in use.  "
        "Creating or updating these databases may take several minutes, "
        "but it will only need to be done once.  Here are the steps that "
        "Zyzzyva will need to take:\n\n%1\n"
        "Perform these actions now?";

    QMap<int, QString> errorActions;
    errorActions.insert(DbOutOfDate, "Database needs to be updated");
    errorActions.insert(DbOpenError, "Database needs to be rebuilt");
    errorActions.insert(DbDoesNotExist, "Database needs to be created");
    errorActions.insert(DbSymbolsOutOfDate,
                        "Lexicon symbols need to be updated");

    QString actionText;
    QMapIterator<QString, int> it (dbErrors);
    while (it.hasNext()) {
        it.next();
        const QString& lexicon = it.key();
        int error = it.value();
        actionText += lexicon + " - " + errorActions.value(error) + "\n";
    }

    message = Auxil::dialogWordWrap(message.arg(actionText));

    int code = QMessageBox::question(this, caption, message,
                                     QMessageBox::Yes | QMessageBox::No,
                                     QMessageBox::Yes);
    if (code != QMessageBox::Yes)
        return;

    rebuildDatabases(dbErrors.keys());
}

//---------------------------------------------------------------------------
//  importInteractive
//
//! Allow the user to import a word list from a file.
//---------------------------------------------------------------------------
void
MainWindow::importInteractive()
{
    QString file = QFileDialog::getOpenFileName(this, IMPORT_CHOOSER_TITLE,
        QDir::current().path(), "All Files (*.*)");

    if (file.isNull())
        return;
    int imported = importText(LEXICON_CUSTOM, file);
    if (imported < 0)
        return;
    QString caption = IMPORT_COMPLETE_TITLE;
    QString message = "Loaded " + QString::number(imported) + " words.";
    message = Auxil::dialogWordWrap(message);
    QMessageBox::information(this, caption, message, QMessageBox::Ok);
}

//---------------------------------------------------------------------------
//  newQuizFormInteractive
//
//! Create a new quiz form interactively.
//---------------------------------------------------------------------------
void
MainWindow::newQuizFormInteractive()
{
    NewQuizDialog* dialog = new NewQuizDialog(this);
    Q_CHECK_PTR(dialog);
    int code = dialog->exec();
    if (code == QDialog::Accepted) {
        newQuizForm(dialog->getQuizSpec());
    }
    delete dialog;
}

//---------------------------------------------------------------------------
//  newQuizFormInteractive
//
//! Create a new quiz form interactively, with the new quiz dialog initialized
//! from a quiz specification.
//---------------------------------------------------------------------------
void
MainWindow::newQuizFormInteractive(const QuizSpec& quizSpec)
{
    NewQuizDialog* dialog = new NewQuizDialog(this);
    Q_CHECK_PTR(dialog);
    dialog->setQuizSpec(quizSpec);
    int code = dialog->exec();
    if (code == QDialog::Accepted) {
        newQuizForm(dialog->getQuizSpec());
    }
    delete dialog;
}

//---------------------------------------------------------------------------
//  newQuizForm
//
//! Create a new quiz form directly from a quiz specification without
//! presenting the user with a quiz spec dialog.
//
//! @param quizSpec the quiz specification
//---------------------------------------------------------------------------
void
MainWindow::newQuizForm(const QuizSpec& quizSpec)
{
    QuizForm* form = new QuizForm(wordEngine);
    Q_CHECK_PTR(form);
    form->setTileTheme(MainSettings::getTileTheme());
    bool ok = form->newQuiz(quizSpec);
    if (!ok) {
        delete form;
        return;
    }
    newTab(form);
}

//---------------------------------------------------------------------------
//  newSearchForm
//
//! Create a new search form.
//---------------------------------------------------------------------------
void
MainWindow::newSearchForm()
{
    SearchForm* form = new SearchForm(wordEngine);
    Q_CHECK_PTR(form);
    newTab(form);
}

//---------------------------------------------------------------------------
//  newCrosswordGameForm
//
//! Create a crossword game form.
//---------------------------------------------------------------------------
void
MainWindow::newCrosswordGameForm()
{
    CrosswordGameForm* form = new CrosswordGameForm;
    Q_CHECK_PTR(form);
    newTab(form);
}

//---------------------------------------------------------------------------
//  newDefineForm
//
//! Create a new word definition form.
//---------------------------------------------------------------------------
void
MainWindow::newDefineForm()
{
    DefineForm* form = new DefineForm(wordEngine);
    Q_CHECK_PTR(form);
    newTab(form);
}

//---------------------------------------------------------------------------
//  newIntroForm
//
//! Create a new introduction form.
//---------------------------------------------------------------------------
void
MainWindow::newIntroForm()
{
    IntroForm* form = new IntroForm;
    Q_CHECK_PTR(form);
    newTab(form);
}

//---------------------------------------------------------------------------
//  newCardboxForm
//
//! Create a new cardbox form.
//---------------------------------------------------------------------------
void
MainWindow::newCardboxForm()
{
    CardboxForm* form = new CardboxForm(wordEngine);
    Q_CHECK_PTR(form);
    newTab(form);
}

//---------------------------------------------------------------------------
//  doSaveAction
//
//! Open a save dialog for the current tab.
//---------------------------------------------------------------------------
void
MainWindow::doSaveAction()
{
    QWidget* w = tabStack->currentWidget();
    if (!w)
        return;

    // Prompt to save changes if this is a Quiz tab
    ActionForm* form = static_cast<ActionForm*>(w);
    form->saveRequested(false);
}

//---------------------------------------------------------------------------
//  doSaveAsAction
//
//! Open a save dialog for the current tab.
//---------------------------------------------------------------------------
void
MainWindow::doSaveAsAction()
{
    QWidget* w = tabStack->currentWidget();
    if (!w)
        return;

    // Prompt to save changes if this is a Quiz tab
    ActionForm* form = static_cast<ActionForm*>(w);
    form->saveRequested(true);
}

//---------------------------------------------------------------------------
//  doPrintAction
//
//! Open a print dialog for the current tab.
//---------------------------------------------------------------------------
void
MainWindow::doPrintAction()
{
    QWidget* w = tabStack->currentWidget();
    if (!w)
        return;

    ActionForm* form = static_cast<ActionForm*>(w);
    form->printRequested();
}

//---------------------------------------------------------------------------
//  doJudgeAction
//
//! Open a word judge window.
//---------------------------------------------------------------------------
void
MainWindow::doJudgeAction()
{
    JudgeSelectDialog* selectDialog = new JudgeSelectDialog(this);
    Q_CHECK_PTR(selectDialog);
    int code = selectDialog->exec();
    if (code != QDialog::Accepted) {
        delete selectDialog;
        return;
    }

    QString lexicon = selectDialog->getLexicon();
    QString password = selectDialog->getPassword();
    delete selectDialog;

    QApplication::setOverrideCursor(Qt::BlankCursor);
    JudgeDialog* dialog = new JudgeDialog(wordEngine, lexicon, password, this);
    dialog->exec();
    delete dialog;
    QApplication::restoreOverrideCursor();
}

//---------------------------------------------------------------------------
//  editSettings
//
//! Allow the user to edit application settings.  If the user makes changes
//! and accepts the dialog, write the settings.  If the user rejects the
//! dialog, restore the settings after the dialog is closed.
//---------------------------------------------------------------------------
void
MainWindow::editSettings()
{
    bool settingsChanged = false;
    bool oldAutoImport = false;
    QSet<QString> oldLexicons;
    QList<LexiconStyle> oldStyles;
    QString oldCustomFile;
    if (settingsDialog->exec() == QDialog::Accepted) {
        settingsChanged = true;
        oldAutoImport = MainSettings::getUseAutoImport();
        oldLexicons = MainSettings::getAutoImportLexicons().toSet();
        oldCustomFile = MainSettings::getAutoImportFile();
        oldStyles = MainSettings::getWordListLexiconStyles();
        settingsDialog->writeSettings();
    }
    else {
        settingsDialog->readSettings();
    }
    readSettings(false);

    bool newAutoImport = MainSettings::getUseAutoImport();
    if (!settingsChanged || !newAutoImport)
        return;

    // Load any new lexicons that have been selected, but do not unload any
    // that have been deselected, because they might be in use (by a quiz, for
    // example).  If auto import was turned on, then all lexicons are new.
    QSet<QString> newLexicons = MainSettings::getAutoImportLexicons().toSet();
    QSet<QString> addedLexicons = (newAutoImport && !oldAutoImport) ?
        newLexicons : newLexicons - oldLexicons;

    // Custom database needs to be rebuilt if custom lexicon file changed
    QString newCustomFile = MainSettings::getAutoImportFile();
    if ((oldCustomFile != newCustomFile) && newLexicons.contains(LEXICON_CUSTOM))
        addedLexicons.insert(LEXICON_CUSTOM);

    // Determine lexicons that need to be rebuilt based on lexicon style
    // (symbol) changes - also update lexicons that compare against a lexicon
    // that was newly added
    QSet<QString> symbolLexicons;
    QList<LexiconStyle> newStyles = MainSettings::getWordListLexiconStyles();
    if (!oldStyles.isEmpty()) {
        QMutableListIterator<LexiconStyle> it (oldStyles);
        while (it.hasNext()) {
            const LexiconStyle& style = it.next();
            int removed = newStyles.removeAll(style);
            if (!removed || addedLexicons.contains(style.compareLexicon))
                symbolLexicons.insert(style.lexicon);
            else
                it.remove();
        }
    }
    if (!newStyles.isEmpty()) {
        QListIterator<LexiconStyle> it (newStyles);
        while (it.hasNext()) {
            const LexiconStyle& style = it.next();
            symbolLexicons.insert(style.lexicon);
        }
    }
    symbolLexicons -= addedLexicons;
    addedLexicons += symbolLexicons;

    // Try to import added lexicons and symbol-changed lexicons
    bool processNeeded = false;
    if (!addedLexicons.isEmpty()) {
        QSetIterator<QString> it (addedLexicons);
        while (it.hasNext()) {
            const QString& lexicon = it.next();
            importLexicon(lexicon);
        }
        tryConnectToDatabases();
    }

    // Add DB errors for database whose symbols need to be updated
    if (!symbolLexicons.isEmpty()) {
        processNeeded = true;
        foreach (const QString& lexicon, symbolLexicons) {
            dbErrors.insert(lexicon, DbSymbolsOutOfDate);
        }
    }

    // Process all DB errors (connection errors and symbol updates)
    if (processNeeded)
        processDatabaseErrors();

    // Call this to get original tab status message back
    currentTabChanged(0);
}

//---------------------------------------------------------------------------
//  viewDefinition
//
//! Allow the user to view the definition of a word.  Display a dialog asking
//! the user for the word.
//---------------------------------------------------------------------------
void
MainWindow::viewDefinition()
{
    WordEntryDialog* entryDialog = new WordEntryDialog(this);
    Q_CHECK_PTR(entryDialog);
    entryDialog->setWindowTitle("Word Definition");
    entryDialog->resize(entryDialog->minimumSizeHint().width() * 2,
                        entryDialog->minimumSizeHint().height());
    int code = entryDialog->exec();
    QString word = entryDialog->getWord();
    QString lexicon = entryDialog->getLexicon();
    delete entryDialog;
    if ((code != QDialog::Accepted) || word.isEmpty())
        return;

    DefinitionDialog* dialog = new DefinitionDialog(wordEngine, lexicon, word,
                                                    this);
    Q_CHECK_PTR(dialog);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

//---------------------------------------------------------------------------
//  viewVariation
//
//! Allow the user to view variations of a word.  Display a dialog asking the
//! user for the word.
//---------------------------------------------------------------------------
void
MainWindow::viewVariation(int variation)
{
    QString caption;
    switch (variation) {
        case VariationAnagrams: caption = "Word Anagrams"; break;
        case VariationSubanagrams: caption = "Word Subanagrams"; break;
        case VariationHooks: caption = "Word Hooks"; break;
        case VariationExtensions: caption = "Word Extensions"; break;
        case VariationAnagramHooks: caption = "Word Anagram Hooks"; break;
        case VariationBlankAnagrams: caption = "Word Blank Anagrams"; break;
        case VariationBlankMatches: caption = "Word Blank Matches"; break;
        case VariationTranspositions: caption = "Word Transpositions"; break;
        default: break;
    }

    WordEntryDialog* entryDialog = new WordEntryDialog(this);
    Q_CHECK_PTR(entryDialog);
    entryDialog->setWindowTitle(caption);
    entryDialog->resize(entryDialog->minimumSizeHint().width() * 2,
                        entryDialog->minimumSizeHint().height());
    int code = entryDialog->exec();
    QString word = entryDialog->getWord();
    QString lexicon = entryDialog->getLexicon();
    delete entryDialog;
    if ((code != QDialog::Accepted) || word.isEmpty())
        return;

    WordVariationType type = static_cast<WordVariationType>(variation);
    WordVariationDialog* dialog = new WordVariationDialog(wordEngine, lexicon,
                                                          word, type, this);
    Q_CHECK_PTR(dialog);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    wordVariationDialogs.insert(wordVariationDialogs.size(), dialog);
    //NOTE (JGM) WordVariationDialog* does not work as arguments!
    connect(dialog, SIGNAL(destroyed(QObject*)), this, SLOT(clearDialogFromList(QObject*)));
    dialog->show();
}

//---------------------------------------------------------------------------
//  rebuildDatabaseRequested
//
//! Called when the user requests a database rebuild.
//---------------------------------------------------------------------------
void
MainWindow::rebuildDatabaseRequested()
{
    DatabaseRebuildDialog* dialog = new DatabaseRebuildDialog(this);
    Q_CHECK_PTR(dialog);

    int code = dialog->exec();
    if (code == QDialog::Accepted) {
        QStringList lexicons;
        if (dialog->getRebuildAll())
            lexicons = MainSettings::getAutoImportLexicons();
        else
            lexicons.append(dialog->getLexicon());

        rebuildDatabases(lexicons);
    }
    delete dialog;
}

//---------------------------------------------------------------------------
//  rescheduleCardboxRequested
//
//! Called when the user requests to reschedule cardbox items.
//---------------------------------------------------------------------------
void
MainWindow::rescheduleCardboxRequested()
{
    CardboxRescheduleDialog* dialog = new CardboxRescheduleDialog(this);
    Q_CHECK_PTR(dialog);

    int code = dialog->exec();
    if (code == QDialog::Accepted) {
        QString quizType = dialog->getQuizType();
        QString lexicon = dialog->getLexicon();

        QStringList words;
        if (!dialog->getRescheduleAll()) {
            SearchSpec searchSpec = dialog->getSearchSpec();
            words = wordEngine->search(lexicon, searchSpec, true);
            if (words.isEmpty())
                return;
        }

        CardboxRescheduleType rescheduleType = dialog->getRescheduleType();

        int rescheduleValue = 0;
        if (rescheduleType == CardboxRescheduleShiftDays)
            rescheduleValue = dialog->getNumDays();
        else if (rescheduleType == CardboxRescheduleShiftBacklog)
            rescheduleValue = dialog->getBacklogSize();

        QApplication::setOverrideCursor(Qt::WaitCursor);
        int numRescheduled = rescheduleCardbox(words, lexicon, quizType,
            rescheduleType, rescheduleValue);
        QApplication::restoreOverrideCursor();

        QString questionStr = numRescheduled == 1 ? QString("question")
                                                  : QString("questions");
        QString caption = "Cardbox Questions Rescheduled";
        QString message = QString::number(numRescheduled) + " " +
            questionStr + " rescheduled.";
        message = Auxil::dialogWordWrap(message);
        QMessageBox::information(this, caption, message);
    }

    delete dialog;
}

//---------------------------------------------------------------------------
//  displayAbout
//
//! Display an About screen.
//---------------------------------------------------------------------------
void
MainWindow::displayAbout()
{
    aboutDialog->exec();
}

//---------------------------------------------------------------------------
//  displayHelp
//
//! Display a Help screen.
//---------------------------------------------------------------------------
void
MainWindow::displayHelp()
{
//    helpDialog->showPage(Auxil::getHelpDir() + "/index.html");
    QProcess *process = new QProcess;
    QStringList args;
    args << QLatin1String("-collectionFile")
        << (Auxil::getHelpDir() + QLatin1String("/zyzzyva.qhc"))
        << QLatin1String("-showUrl")
        << QLatin1String("qthelp://twilightcenturycomputing.com/5.1.1/index.html")
        << QLatin1String("-enableRemoteControl");
    process->start(QLatin1String("assistant"), args);
    if (!process->waitForStarted())
        return;
}

//---------------------------------------------------------------------------
//  displayLexiconError
//
//! Display any lexicon errors, and ask the user whether to proceed in the
//! face of any errors.
//---------------------------------------------------------------------------
void
MainWindow::displayLexiconError()
{
    if (lexiconError.isEmpty())
        return;

    QString caption = "Lexicon Warning";
    QString message = lexiconError + "\n\nProceed anyway?";
    message = Auxil::dialogWordWrap(message);
    int code = QMessageBox::warning(this, caption, message,
                                    QMessageBox::Yes | QMessageBox::No,
                                    QMessageBox::No);
    if (code != QMessageBox::Yes)
        qApp->quit();
}

//---------------------------------------------------------------------------
//  helpDialogError
//
//! Called when an error occurs in the help dialog.
//
//! @param message the error message
//---------------------------------------------------------------------------
//void
//MainWindow::helpDialogError(const QString& message)
//{
//    QString caption = "Help Display Error";
//    QMessageBox::warning(this, caption, Auxil::dialogWordWrap(message));
//}

//---------------------------------------------------------------------------
//  closeCurrentTab
//
//! Close the currently open tab.  If no other tabs exist, hide the button
//! used for closing tabs.
//---------------------------------------------------------------------------
void
MainWindow::closeCurrentTab()
{
    QWidget* w = tabStack->currentWidget();
    if (!w)
        return;

    // Prompt to save changes if this is a Quiz tab
    ActionForm* form = static_cast<ActionForm*>(w);
    ActionForm::ActionFormType type = form->getType();
    if (type == ActionForm::QuizFormType) {
        QuizForm* quizForm = static_cast<QuizForm*> (form);
        if (quizForm->isSaveEnabled()) {
            bool ok = quizForm->promptToSaveChanges();
            if (!ok)
                return;
        }
    }

    tabStack->removeTab(tabStack->indexOf(w));
    delete w;
    if (tabStack->count() == 0) {
        messageLabel->setText(QString());
        detailsLabel->setText(QString());
        closeButton->hide();
        closeTabAction->setEnabled(false);
    }
}

//---------------------------------------------------------------------------
//  currentTabChanged
//
//! Called when the current tab changes.  Sets the contents of the form
//! used for closing tabs.
//---------------------------------------------------------------------------
void
MainWindow::currentTabChanged(int)
{
    QWidget* w = tabStack->currentWidget();
    QString status;
    QString details;
    bool saveEnabled = false;
    bool saveCapable = false;
    bool printEnabled = false;
    if (w) {
        ActionForm* form = static_cast<ActionForm*>(w);
        form->selectInputArea();
        status = form->getStatusString();
        details = form->getDetailsString();
        saveEnabled = form->isSaveEnabled();
        saveCapable = form->isSaveCapable();
        printEnabled = form->isPrintEnabled();
    }
    messageLabel->setText(status);
    detailsLabel->setText(details);
    saveAction->setEnabled(saveEnabled);
    toolbarSaveAction->setEnabled(saveEnabled);
    saveAsAction->setEnabled(saveCapable);
    toolbarSaveAsAction->setEnabled(saveCapable);
    printAction->setEnabled(printEnabled);
    toolbarPrintAction->setEnabled(printEnabled);
}

//---------------------------------------------------------------------------
//  tabTitleChanged
//
//! Called when the title string for a tab changes.
//
//! @param the new title status string
//---------------------------------------------------------------------------
void
MainWindow::tabTitleChanged(const QString& title)
{
    QObject* object = sender();
    if (!object)
        return;
    ActionForm* form = static_cast<ActionForm*>(object);
    int index = tabStack->indexOf(form);
    tabStack->setTabText(index, title);
}

//---------------------------------------------------------------------------
//  tabStatusChanged
//
//! Called when the status string for a tab changes.
//
//! @param the new status string
//---------------------------------------------------------------------------
void
MainWindow::tabStatusChanged(const QString& status)
{
    QObject* object = sender();
    if (!object)
        return;
    ActionForm* form = static_cast<ActionForm*>(object);
    int index = tabStack->indexOf(form);
    if (index == tabStack->currentIndex()) {
        messageLabel->setText(status);
        qApp->processEvents();
    }
}

//---------------------------------------------------------------------------
//  tabDetailsChanged
//
//! Called when the details string for a tab changes.
//
//! @param the new details string
//---------------------------------------------------------------------------
void
MainWindow::tabDetailsChanged(const QString& details)
{
    QObject* object = sender();
    if (!object)
        return;
    ActionForm* form = static_cast<ActionForm*>(object);
    int index = tabStack->indexOf(form);
    if (index == tabStack->currentIndex()) {
        detailsLabel->setText(details);
        qApp->processEvents();
    }
}

//---------------------------------------------------------------------------
//  tabSaveEnabledChanged
//
//! Called when the save enabled status for a tab changes.
//
//! @param the new save enabled status
//---------------------------------------------------------------------------
void
MainWindow::tabSaveEnabledChanged(bool saveEnabled)
{
    QObject* object = sender();
    if (!object)
        return;
    ActionForm* form = static_cast<ActionForm*>(object);
    int index = tabStack->indexOf(form);
    if (index == tabStack->currentIndex()) {
        saveAction->setEnabled(saveEnabled);
        toolbarSaveAction->setEnabled(saveEnabled);
    }
}

//---------------------------------------------------------------------------
//  tabPrintEnabledChanged
//
//! Called when the print enabled status for a tab changes.
//
//! @param the new print enabled status
//---------------------------------------------------------------------------
void
MainWindow::tabPrintEnabledChanged(bool printEnabled)
{
    QObject* object = sender();
    if (!object)
        return;
    ActionForm* form = static_cast<ActionForm*>(object);
    int index = tabStack->indexOf(form);
    if (index == tabStack->currentIndex()) {
        printAction->setEnabled(printEnabled);
        toolbarPrintAction->setEnabled(printEnabled);
    }
}

//---------------------------------------------------------------------------
//  tryConnectToDatabase
//
//! Try to connect to a lexicon database.
//
//! @param lexicon the lexicon name
//! @return the error code
//---------------------------------------------------------------------------
int
MainWindow::tryConnectToDatabase(const QString& lexicon)
{
    if (wordEngine->databaseIsConnected(lexicon) && (lexicon != LEXICON_CUSTOM))
        return DbNoError;

    setSplashMessage(QString("Connecting to %1 database...").arg(lexicon), SPLASH_MESSAGE_COLOR);

    QString dbFilename = Auxil::getDatabaseFilename(lexicon);
    QFile dbFile (dbFilename);
    int dbError = DbNoError;

    // Make sure DB file exists, can be opened, and is up-to-date
    if (dbFile.exists()) {
        Rand rng;
        rng.srand(QDateTime::currentDateTime().toTime_t(), Auxil::getPid());
//        rng.srand(QDateTime::currentDateTime().toTime_t());
        unsigned int r = rng.rand();
        QString dbConnectionName = "MainWindow_" + lexicon + "_" +
            QString::number(r);
        do {
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE",
                                                        dbConnectionName);
            db.setDatabaseName(dbFilename);
            bool ok = db.open();

            if (ok) {
                QString qstr = "SELECT version FROM db_version";
                QSqlQuery query (qstr, db);
                int dbVersion = 0;
                if (query.next())
                    dbVersion = query.value(0).toInt();

                if (dbVersion < CURRENT_DATABASE_VERSION) {
                    dbError = DbOutOfDate;
                    break;
                }
            }

            else {
                dbError = DbOpenError;
                break;
            }

            // For custom lexicon, check to see if lexicon file has changed -
            // if it has, the database is out of date
            if (lexicon == LEXICON_CUSTOM) {
                QString qstr = "SELECT file FROM lexicon_file";
                QSqlQuery query (qstr, db);
                QString lexiconFile;
                if (query.next())
                    lexiconFile = query.value(0).toString();

                if (lexiconFile != wordEngine->getLexiconFile(lexicon)) {
                    dbError = DbOutOfDate;
                    break;
                }
            }
        } while (false);

        QSqlDatabase::removeDatabase(dbConnectionName);
    }

    else {
        dbError = DbDoesNotExist;
    }

    if (dbError != DbNoError)
        return dbError;

    // Everything seems okay, so actually try to connect
    bool ok = connectToDatabase(lexicon);
    if (!ok)
        return DbConnectionError;

    setSplashMessage(QString());
    return DbNoError;
}

//---------------------------------------------------------------------------
//  connectToDatabase
//
//! Connect to the database for a lexicon.
//
//! @param lexicon the lexicon name
//! @return true if successful, false otherwise
//---------------------------------------------------------------------------
bool
MainWindow::connectToDatabase(const QString& lexicon)
{
    QString dbFilename = Auxil::getDatabaseFilename(lexicon);
    QString dbError;
    bool ok = wordEngine->connectToDatabase(lexicon, dbFilename, &dbError);
    if (!ok) {
        QString caption = "Database Connection Error";
        QString message = "Unable to connect to database:\n" + dbError;
        message = Auxil::dialogWordWrap(message);
        QMessageBox::warning(this, caption, message);
        return false;
    }
    return true;
}

//---------------------------------------------------------------------------
//  rebuildDatabases
//
//! Rebuild the databases for a list of lexicons.  Also display a progress
//! dialog.
//
//! @param lexicons the list of lexicons
//---------------------------------------------------------------------------
void
MainWindow::rebuildDatabases(const QStringList& lexicons)
{
    QStringList successes;
    QStringList failures;
    foreach (const QString& lexicon, lexicons) {
        bool ok = rebuildDatabase(lexicon);
        // FIXME: do something if DB creation fails!
        if (!ok) {
            failures.append(lexicon);
            continue;
        }

        ok = connectToDatabase(lexicon);
        if (ok)
            successes.append(lexicon);
        else
            failures.append(lexicon);
    }

    QString resultMessage;
    if (!successes.isEmpty()) {
        resultMessage += "These databases were successfully created: " +
            successes.join(", ") + ".";
    }
    if (!failures.isEmpty()) {
        resultMessage += "These databases encountered errors: " +
            failures.join(", ") + ".";
    }

    QMessageBox::information(this, "Database Creation Result", resultMessage);
}

//---------------------------------------------------------------------------
//  rebuildDatabase
//
//! Rebuild the database for a lexicon.  Also display a progress dialog.
//
//! @param lexicon the lexicon name
//! @return true if successful, false otherwise
//---------------------------------------------------------------------------
bool
MainWindow::rebuildDatabase(const QString& lexicon)
{
    QString dbFilename = Auxil::getDatabaseFilename(lexicon);
    QString definitionFilename;
    if (lexicon == LEXICON_CUSTOM) {
        definitionFilename = MainSettings::getAutoImportFile();
    }
    else {
        definitionFilename = Auxil::getWordsDir() +
            Auxil::getLexiconPrefix(lexicon) + ((lexicon == LEXICON_CSW15 || lexicon == LEXICON_CSW19) ? ".bin" : ".txt");
    }

    QFileInfo fileInfo (dbFilename);
    QString file = fileInfo.fileName();
    QString path = fileInfo.path();
    QString tmpDbFilename = path + "/orig-" + file;

    wordEngine->disconnectFromDatabase(lexicon);
    QFile dbFile (dbFilename);
    QFile tmpDbFile (tmpDbFilename);

    bool ok = (!tmpDbFile.exists() || tmpDbFile.remove());
    if (!ok) {
        QString caption = "Cannot remove database backup file";
        QString message = "Cannot remove database backup file: " +
            tmpDbFilename + ".\nPlease close Collins Zyzzyva, remove or rename the "
            "file, then restart Collins Zyzzyva.";
        message = Auxil::dialogWordWrap(message);
        QMessageBox::warning(this, caption, message);
        return false;
    }

    ok = (!dbFile.exists() || dbFile.rename(tmpDbFilename));
    if (!ok) {
        QString caption = "Cannot remove database file";
        QString message = "Cannot remove original database file: " +
            dbFilename + ".\nPlease close Collins Zyzzyva, remove or rename the "
            "file, then restart Collins Zyzzyva.";
        message = Auxil::dialogWordWrap(message);
        QMessageBox::warning(this, caption, message);
        tmpDbFile.rename(dbFilename);
        return false;
    }

    QProgressDialog* dialog = new QProgressDialog(this);

    QLabel* dialogLabel = new QLabel("Creating " + lexicon + " database...");
    dialog->setWindowTitle("Creating " + lexicon + " Database");
    dialog->setLabel(dialogLabel);

    CreateDatabaseThread* thread = new CreateDatabaseThread(wordEngine,
        lexicon, dbFilename, definitionFilename, this);
    connect(thread, SIGNAL(steps(int)),
            dialog, SLOT(setMaximum(int)));
    connect(thread, SIGNAL(progress(int)),
            dialog, SLOT(setValue(int)));
    connect(dialog, SIGNAL(canceled()), thread, SLOT(cancel()));

    QApplication::setOverrideCursor(Qt::WaitCursor);

    thread->start();
    dialog->exec();
    thread->quit();
    thread->wait();

    QApplication::restoreOverrideCursor();

    bool success = true;
    if (!thread->getError().isEmpty()) {
        QMessageBox::information(this, "Unable to Create Database",
            thread->getError());
        success = false;
    }

    else if (thread->getCancelled()) {
        QMessageBox::information(this, "Database Not Created",
            "Database creation cancelled.");
        success = false;
    }

    if (!success) {
        if (dbFile.exists())
            dbFile.remove();
        if (tmpDbFile.exists())
            tmpDbFile.rename(dbFilename);
    }

    delete thread;
    delete dialog;
    return success;
}

//---------------------------------------------------------------------------
//  rescheduleCardbox
//
//! Reschedule quiz questions in the cardbox system.
//
//! @param words a list of words to reschedule
//! @param lexicon the lexicon
//! @param quizType the quiz type
//! @return the number of questions rescheduled
//---------------------------------------------------------------------------
int
MainWindow::rescheduleCardbox(const QStringList& words,
    const QString& lexicon, const QString& quizType,
    CardboxRescheduleType rescheduleType, int rescheduleValue) const
{
    if (rescheduleType == CardboxRescheduleUnknown)
        return 0;

    QuizSpec::QuizType type = Auxil::stringToQuizType(quizType);
    if (type == QuizSpec::UnknownQuizType)
        return 0;

    QStringList questions;
    if (!words.isEmpty()) {
        switch (type) {
            case QuizSpec::QuizAnagrams:
            case QuizSpec::QuizAnagramsWithHooks:
            case QuizSpec::QuizSubanagrams:
            case QuizSpec::QuizAnagramJumble:
            case QuizSpec::QuizSubanagramJumble: {
                QSet<QString> alphagramSet;
                QStringListIterator it (words);
                while (it.hasNext()) {
                    alphagramSet.insert(Auxil::getAlphagram(it.next()));
                }
                questions = QStringList::fromSet(alphagramSet);
            }
            break;

            default:
            questions = words;
        }
    }

    QuizDatabase db (lexicon, quizType);
    if (!db.isValid())
        return 0;

    switch (rescheduleType) {
        case CardboxRescheduleShiftDays:
        return db.shiftCardboxByDays(questions, rescheduleValue);

        case CardboxRescheduleShiftBacklog:
        return db.shiftCardboxByBacklog(questions, rescheduleValue);

        case CardboxRescheduleByCardbox:
        return db.rescheduleCardbox(questions);

        default: return 0;
    }
}

//---------------------------------------------------------------------------
//  closeEvent
//
//! The event handler for widget close events, called when the main window is
//! closed.
//
//! @param event the close event
//---------------------------------------------------------------------------
void
MainWindow::closeEvent(QCloseEvent* event)
{
    QMessageBox* msgBox = new QMessageBox(QMessageBox::Question, QCoreApplication::applicationName(), tr("Really exit?\n"),
                                          QMessageBox::StandardButton::NoButton, this);
    Q_CHECK_PTR(msgBox);
    QCheckBox* neverShowCbox = new QCheckBox("Don't show this again");
    Q_CHECK_PTR(neverShowCbox);
    msgBox->setCheckBox(neverShowCbox);
    QPushButton* yesButton = msgBox->addButton(tr("Yes"), QMessageBox::YesRole);
    Q_CHECK_PTR(yesButton);
    QPushButton* noButton = msgBox->addButton(tr("No"), QMessageBox::NoRole);
    Q_CHECK_PTR(noButton);
    QPushButton* cancelButton = msgBox->addButton(tr("Cancel"), QMessageBox::RejectRole);
    Q_CHECK_PTR(cancelButton);
    msgBox->setDefaultButton(yesButton);
    msgBox->setEscapeButton(cancelButton);

    if (MainSettings::getConfirmExit()) {
        msgBox->exec();
        if (msgBox->clickedButton() == yesButton || msgBox->clickedButton() == noButton)
            if (neverShowCbox->isChecked())
                MainSettings::setConfirmExit(false);
        if (msgBox->clickedButton() != yesButton) {
            event->ignore();
            delete msgBox;
            return;
        }
    }

    // Look for unsaved quizzes
    int count = tabStack->count();
    for (int i = 0; i < count; ++i) {
        ActionForm* form = static_cast<ActionForm*> (tabStack->widget(i));
        ActionForm::ActionFormType type = form->getType();
        if (type == ActionForm::QuizFormType) {
            QuizForm* quizForm = static_cast<QuizForm*> (form);
            if (quizForm->isSaveEnabled()) {
                tabStack->setCurrentWidget(quizForm);
                bool ok = quizForm->promptToSaveChanges();
                if (!ok) {
                    event->ignore();
                    delete msgBox;
                    return;
                }
            }
        }
    }

    writeSettings();
}

//---------------------------------------------------------------------------
//  readSettings
//
//! Read application settings.
//---------------------------------------------------------------------------
void
MainWindow::readSettings(bool useGeometry)
{
    MainSettings::readSettings();

    if (useGeometry) {
        resize(MainSettings::getMainWindowSize());
        move(MainSettings::getMainWindowPos());
    }

    // Main font
    QFont mainFont;
    QString fontStr = MainSettings::getMainFont();
    if (fontStr.isEmpty()) {
        //(JGM) Attempt to dynamically reload fonts.
        qApp->setFont(QGuiApplication::font());
        qApp->setFont(QGuiApplication::font(), "QHeaderView");
    }
    else if (mainFont.fromString(fontStr)) {
        qApp->setFont(mainFont);
        qApp->setFont(mainFont, "QHeaderView");
    }
    else {
        //qWarning("Cannot set font: " + fontStr);
    }

    // Word list font
    QFont font;
    fontStr = MainSettings::getWordListFont();
    if (fontStr.isEmpty())
        qApp->setFont(qApp->font(), "WordTableView");
    else if (font.fromString(fontStr))
        qApp->setFont(font, "WordTableView");
    else {
        //qWarning("Cannot set font: " + fontStr);
    }

    // Quiz label font
    fontStr = MainSettings::getQuizLabelFont();
    if (fontStr.isEmpty()) {
        font = qApp->font();
        font.setPixelSize(60);
        qApp->setFont(font, "QuizQuestionLabel");
    }
    else if (font.fromString(fontStr))
        qApp->setFont(font, "QuizQuestionLabel");
    else {
        //qWarning("Cannot set font: " + fontStr);
    }

    // Word input font
    fontStr = MainSettings::getWordInputFont();
    if (fontStr.isEmpty()) {
        qApp->setFont(qApp->font(), "WordLineEdit");
        qApp->setFont(qApp->font(), "WordTextEdit");
    }
    else if (font.fromString(fontStr)) {
        qApp->setFont(font, "WordLineEdit");
        qApp->setFont(font, "WordTextEdit");
    }
    else {
        //qWarning("Cannot set font: " + fontStr);
    }

    // Definition font
    fontStr = MainSettings::getDefinitionFont();
    if (fontStr.isEmpty()) {
        qApp->setFont(qApp->font(), "DefinitionBox");
        qApp->setFont(qApp->font(), "DefinitionLabel");
        qApp->setFont(qApp->font(), "DefinitionTextEdit");
    }
    else if (font.fromString(fontStr)) {
        qApp->setFont(font, "DefinitionBox");
        qApp->setFont(font, "DefinitionLabel");
        qApp->setFont(font, "DefinitionTextEdit");
    }
    else {
        //qWarning("Cannot set font: " + fontStr);
    }

    // Printing font
    fontStr = MainSettings::getPrintingFont();
    if (fontStr.isEmpty())
        ;
    else if (font.fromString(fontStr))
        ;
    else {
        //qWarning("Cannot set font: " + fontStr);
    }

    // Set tile theme and background color for all quiz forms
    // FIXME: instead, this should simply call a settingsChanged slot on all
    // forms, and each form should handle it as they see fit
    QString tileTheme = MainSettings::getTileTheme();
    QColor backgroundColor = MainSettings::getQuizBackgroundColor();
    int count = tabStack->count();
    for (int i = 0; i < count; ++i) {
        ActionForm* form = static_cast<ActionForm*> (tabStack->widget(i));
        ActionForm::ActionFormType type = form->getType();
        if (type == ActionForm::QuizFormType) {
            QuizForm* quizForm = static_cast<QuizForm*> (form);
            quizForm->setBackgroundColor(backgroundColor);
            quizForm->updateQuestionStatus();
            quizForm->updateStatusString();
            // ### update details string here?
            //quizForm->updateDetailsString();
            quizForm->setTileTheme(tileTheme);
            quizForm->getView()->resizeItemsRecursively();
            AnalyzeQuizDialog* analyzeDialog = quizForm->getAnalyzeDialog();
            if (analyzeDialog) {
                analyzeDialog->getMissedView()->resizeItemsRecursively();
                analyzeDialog->getIncorrectView()->resizeItemsRecursively();
            }
        }
        else if (type == ActionForm::SearchFormType) {
            SearchForm* searchForm = static_cast<SearchForm*> (form);
            searchForm->getView()->resizeItemsRecursively();
        }
    }
    //! TODO (JGM) Add an interface/function to combine this with the same code in
    //! WordTableView::resizeItemsRecursively.
    QListIterator<WordVariationDialog*> it(wordVariationDialogs);
    WordVariationDialog* current;
    while (it.hasNext()) {
        current = it.next();
        if (current) {
            if (current->getTopView()) {
                current->getTopView()->resizeItemsRecursively();
            }
            if (current->getMiddleView()) {
                current->getMiddleView()->resizeItemsRecursively();
            }
            if (current->getBottomView()) {
                current->getBottomView()->resizeItemsRecursively();
            }
        }
    }
}

//---------------------------------------------------------------------------
//  writeSettings
//
//! Write application settings.
//---------------------------------------------------------------------------
void
MainWindow::writeSettings()
{
    MainSettings::setMainWindowPos(pos());
    MainSettings::setMainWindowSize(size());
    MainSettings::setProgramVersion(ZYZZYVA_VERSION);
    MainSettings::writeSettings();
}

//---------------------------------------------------------------------------
//  newTab
//
//! Create and display a new tab.
//
//! @param form the form to display
//---------------------------------------------------------------------------
void
MainWindow::newTab(ActionForm* form)
{
    connect(form, SIGNAL(titleChanged(const QString&)),
            SLOT(tabTitleChanged(const QString&)));
    connect(form, SIGNAL(statusChanged(const QString&)),
            SLOT(tabStatusChanged(const QString&)));
    connect(form, SIGNAL(detailsChanged(const QString&)),
            SLOT(tabDetailsChanged(const QString&)));
    connect(form, SIGNAL(saveEnabledChanged(bool)),
            SLOT(tabSaveEnabledChanged(bool)));
    connect(form, SIGNAL(printEnabledChanged(bool)),
            SLOT(tabPrintEnabledChanged(bool)));

    tabStack->addTab(form, form->getIcon(), form->getTitle());
    tabStack->setCurrentWidget(form);
    closeButton->show();
    currentTabChanged(0);
    if (tabStack->count() == 1)
        closeTabAction->setEnabled(true);
}

//---------------------------------------------------------------------------
//  newQuizFromQuizFile
//
//! Create a new Quiz tab and start a quiz from a saved quiz file.
//
//! @param filename the file
//---------------------------------------------------------------------------
void
MainWindow::newQuizFromQuizFile(const QString& filename)
{
    QFile file (filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QString errorMsg;
    QuizSpec quizSpec;
    if (!quizSpec.fromXmlFile(file, &errorMsg)) {
        QMessageBox::warning(this, "Cannot Create Quiz",
                             "An error occurred while creating the quiz.\n" +
                             errorMsg);
        return;
    }

    newQuizForm(quizSpec);
}

//---------------------------------------------------------------------------
//  newQuizFromWordFile
//
//! Create a new Quiz tab and start a quiz from a list of words in a plain
//! text file.
//
//! @param filename the file
//---------------------------------------------------------------------------
void
MainWindow::newQuizFromWordFile(const QString& filename)
{
    QFile file (filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QStringList words;
    char* buffer = new char[MAX_INPUT_LINE_LEN];
    while (file.readLine(buffer, MAX_INPUT_LINE_LEN) > 0) {
        QString line (buffer);
        line = line.simplified();
        if (!line.length() || (line.at(0) == '#'))
            continue;

        QString word = line.section(' ', 0, 0).toUpper();
        if (word.isEmpty())
            continue;

        words.append(word);
    }

    SearchCondition condition;
    condition.type = SearchCondition::InWordList;
    condition.stringValue = words.join(" ");

    SearchSpec searchSpec;
    searchSpec.conditions.append(condition);

    QuizSpec quizSpec;
    quizSpec.setType(QuizSpec::QuizAnagrams);
    quizSpec.setLexicon(MainSettings::getDefaultLexicon());
    quizSpec.setSearchSpec(searchSpec);

    newQuizForm(quizSpec);
}

//---------------------------------------------------------------------------
//  importDawg
//
//! Import words from a DAWG file.
//
//! @param lexicon the name of the lexicon
//! @param file the file to import words from
//! @param reverse whether the DAWG contains reversed words
//! @param errString return error string
//! @return true if successful, false otherwise
//---------------------------------------------------------------------------
bool
MainWindow::importDawg(const QString& lexicon, const QString& file, bool
                       reverse, QString* errString, quint16*
                       expectedChecksum)
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    bool ok = wordEngine->importDawgFile(lexicon, file, reverse,
                                         errString, expectedChecksum);
    QApplication::restoreOverrideCursor();

    if (!ok) {
        QString message = "Unable to load the " + lexicon + " lexicon.  "
            "The following errors occurred:\n";
        if (errString)
            message += *errString;
        message = Auxil::dialogWordWrap(message);
        QMessageBox::warning(this, "Unable to load lexicon", message);
    }

    else if (errString && !errString->isEmpty()) {
        QString message = "The '" + lexicon + "' lexicon was loaded, but "
            "the following errors occurred:\n";
        if (errString)
            message += *errString;
        message = Auxil::dialogWordWrap(message);
        QMessageBox::warning(this, "Lexicon load warning", message);
        // Ask user whether to proceed...
    }

    // If failure, notify the user and bail out

    return ok;
}

//---------------------------------------------------------------------------
//  importChecksums
//
//! Import words from a text file.
//
//! @param filename the file to import words from
//! @return the number of imported words
//---------------------------------------------------------------------------
QList<quint16>
MainWindow::importChecksums(const QString& filename)
{
    QList<quint16> checksums;
    QFile file (filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return checksums;

    char* buffer = new char[MAX_INPUT_LINE_LEN];
    while (file.readLine(buffer, MAX_INPUT_LINE_LEN) > 0) {
        QString line (buffer);
        checksums.append(line.toUShort());
    }
    delete [] buffer;
    return checksums;
}

//---------------------------------------------------------------------------
//  setSplashMessage
//
//! Display a message on the splash screen if available.
//
//! @param message the message
//---------------------------------------------------------------------------
void
MainWindow::setSplashMessage(const QString& message, const QColor& color)
{
    //qDebug("Splash message: |%s|", message.toUtf8().constData());
    if (splashScreen)
        splashScreen->showMessage(message, Qt::AlignHCenter | Qt::AlignBottom, color);
    else
        messageLabel->setText(message);

    qApp->processEvents();
}

//---------------------------------------------------------------------------
//  fixTrolltechConfig
//
//! Fix Trolltech config file to avoid a crash on certain Linux systems.
//---------------------------------------------------------------------------
void
MainWindow::fixTrolltechConfig()
{
    QFile file (Auxil::getHomeDir() + "/.config/Trolltech.conf");
    if (!file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QStringList lines;
    QTextStream inStream (&file);
    while (!inStream.atEnd()) {
        QString line = inStream.readLine();
        if (!line.startsWith("filedialog="))
            lines.append(line);
    }
    file.close();

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream outStream (&file);
    foreach (const QString& line, lines) {
        outStream << line;
        endl(outStream);
    }
}

//---------------------------------------------------------------------------
//  updateSettings
//
//! Update settings. In particular, add new default settings.
//---------------------------------------------------------------------------
void
MainWindow::updateSettings()
{
    QString prevVersion = MainSettings::getProgramVersion();
    QString currVersion = Defs::ZYZZYVA_VERSION;

    if (prevVersion == currVersion)
        return;

    // Add default CSW19 lexicon styles in 2.1.3
    if (Auxil::lessThanVersion(prevVersion, "2.1.3") &&
       ((currVersion == "2.1.3") ||
        Auxil::lessThanVersion("2.1.3", currVersion)))
    {
        QList<LexiconStyle> styles = MainSettings::getWordListLexiconStyles();
        LexiconStyle addStyle;
        QList<LexiconStyle> addStyles;

        addStyle.lexicon = Defs::LEXICON_CSW19;
        addStyle.compareLexicon = Defs::LEXICON_CSW15;
        addStyle.inCompareLexicon = false;
        addStyle.symbol = "+";
        addStyles.append(addStyle);

        // ### Create addStyleToList(bool overwrite) or something similar
        // Look for existing matching lexicon styles and skip if present
        bool added = false;
        foreach (const LexiconStyle& style, addStyles) {
            bool found = false;
            foreach (const LexiconStyle& existStyle, styles) {
                if ((style.lexicon == existStyle.lexicon) &&
                    (style.compareLexicon == existStyle.compareLexicon) &&
                    (style.inCompareLexicon == existStyle.inCompareLexicon))
                {
                    found = true;
                    break;
                }
            }
            if (!found) {
                styles.append(style);
                added = true;
            }
        }

        if (added) {
            MainSettings::setWordListLexiconStyles(styles);
            settingsDialog->refreshSettings();
        }
    }
}

//---------------------------------------------------------------------------
//  makeUserDirs
//
//! Create directories for user data.
//---------------------------------------------------------------------------
void
MainWindow::makeUserDirs()
{
    QDir dir;

    // Copy predefined quizzes and searches into user data directory
    if (!dir.exists(Auxil::getQuizDir() + "/predefined")) {
        Auxil::copyDir(Auxil::getRootDir() + "/data/quiz/predefined",
                       Auxil::getQuizDir() + "/predefined");
    }
    if (!dir.exists(Auxil::getSearchDir() + "/predefined")) {
        Auxil::copyDir(Auxil::getRootDir() + "/data/search/predefined",
                       Auxil::getSearchDir() + "/predefined");
    }

//    renameLexicon(LEXICON_OLD_OWL, LEXICON_OWL);
//    renameLexicon(LEXICON_OLD_OWL2, LEXICON_OWL2);
//    renameLexicon(LEXICON_OLD_OSPD4, LEXICON_OSPD4);
//    renameLexicon(LEXICON_OLD_SOWPODS, LEXICON_OSWI);
//    renameLexicon(LEXICON_OLD_ODS, LEXICON_ODS4);
//    renameLexicon(LEXICON_OLD_CSW, LEXICON_CSW07);

    dir.mkpath(Auxil::getQuizDir() + "/saved");
    dir.mkpath(Auxil::getSearchDir() + "/saved");
    dir.mkpath(Auxil::getUserWordsDir() + "/saved");
    dir.mkpath(Auxil::getUserDir() + "/judge");
}

//---------------------------------------------------------------------------
//  renameLexicon
//
//! Take care of things that need to be done when a lexicon has been renamed.
//
//! @param oldName the old lexicon name
//! @param newName the new lexicon name
//---------------------------------------------------------------------------
void
MainWindow::renameLexicon(const QString& oldName, const QString& newName)
{
    QDir dir;

    // Move quiz database
    QString oldDir = Auxil::getQuizDir() + "/data/" + oldName;
    QString newDir = Auxil::getQuizDir() + "/data/" + newName;
    if (dir.exists(oldDir) && !dir.exists(newDir)) {
        dir.rename(oldDir, newDir);
    }

    // Move judge records
    oldDir = Auxil::getUserDir() + "/judge/" + oldName;
    newDir = Auxil::getUserDir() + "/judge/" + newName;
    if (dir.exists(oldDir) && !dir.exists(newDir)) {
        dir.rename(oldDir, newDir);
    }

    // Move lexicon database
    QString oldDb = Auxil::getUserDir() + "/lexicons/" + oldName + ".db";
    QString newDb = Auxil::getUserDir() + "/lexicons/" + newName + ".db";
    if (QFile::exists(oldDb) && !QFile::exists(newDb)) {
        QFile::rename(oldDb, newDb);
    }

    // Move lexicon database backups
    oldDb = Auxil::getUserDir() + "/lexicons/orig-" + oldName + ".db";
    newDb = Auxil::getUserDir() + "/lexicons/orig-" + newName + ".db";
    if (QFile::exists(oldDb) && !QFile::exists(newDb)) {
        QFile::rename(oldDb, newDb);
    }

    // Change auto import preferences
    bool changed = false;
    QStringList autoImportLexicons = MainSettings::getAutoImportLexicons();
    int numAutoImportLexicons = autoImportLexicons.count();
    for (int i = 0; i < numAutoImportLexicons; ++i) {
        QString& lexicon = autoImportLexicons[i];
        if (lexicon == oldName) {
            lexicon = newName;
            changed = true;
        }
    }
    if (changed)
        MainSettings::setAutoImportLexicons(autoImportLexicons);

    // Change lexicon symbol preferences
    changed = false;
    QList<LexiconStyle> lexiconStyles = MainSettings::getWordListLexiconStyles();
    int numStyles = lexiconStyles.count();
    for (int i = 0; i < numStyles; ++i) {
        LexiconStyle& style = lexiconStyles[i];
        if (style.lexicon == oldName) {
            style.lexicon = newName;
            changed = true;
        }
        if (style.compareLexicon == oldName) {
            style.compareLexicon = newName;
            changed = true;
        }
    }
    if (changed) {
        MainSettings::setWordListLexiconStyles(lexiconStyles);
        settingsDialog->refreshSettings();
    }
}

//---------------------------------------------------------------------------
//  importLexicon
//
//! Import a lexicon.
//
//! @param lexicon the name of the lexicon
//! @return true if successful, false otherwise
//---------------------------------------------------------------------------
bool
MainWindow::importLexicon(const QString& lexicon)
{
    QString importFile;
    QString reverseImportFile;
    QString checksumFile;
    QString playabilityFile;
    bool ok = true;
    bool dawg = true;
    if (lexicon == LEXICON_CUSTOM) {
        importFile = MainSettings::getAutoImportFile();
        dawg = false;

        if (wordEngine->lexiconIsLoaded(lexicon)) {
            QString lexiconFile = wordEngine->getLexiconFile(lexicon);
            if (lexiconFile == importFile)
                return true;
        }
    }
    else {
        if (wordEngine->lexiconIsLoaded(lexicon))
            return true;

        QMap<QString, QString> prefixMap;
        prefixMap[LEXICON_VOLOST] = "/Antarctic/Volost";
        prefixMap[LEXICON_CSW12] = "/British/CSW12";
        prefixMap[LEXICON_CSW15] = "/British/CSW15";
        prefixMap[LEXICON_CSW19] = "/British/CSW19";

        if (prefixMap.contains(lexicon)) {
            QString prefix = Auxil::getWordsDir() + prefixMap.value(lexicon);
            importFile =        prefix + ".dwg";
            reverseImportFile = prefix + "-R.dwg";
            checksumFile =      prefix + "-Checksums.txt";
            playabilityFile =   prefix + ((lexicon == LEXICON_CSW15 || lexicon == LEXICON_CSW19) ? "-Playability.bin" : "-Playability.txt");
        }
    }

    if (importFile.isEmpty())
        return false;

    QString splashMessage = "Loading " + lexicon + " lexicon...";
    setSplashMessage(splashMessage, SPLASH_MESSAGE_COLOR);

    if (dawg) {
        quint16 expectedForwardChecksum = 0;
        quint16 expectedReverseChecksum = 0;

        QList<quint16> checksums = importChecksums(checksumFile);
        if (checksums.size() >= 2) {
            expectedForwardChecksum = checksums[0];
            expectedReverseChecksum = checksums[1];
        }
        else {
            QString message = "Cannot find checksum information for the '" +
                lexicon + "' lexicon.  The lexicon will be loaded, but it is "
                "possible the lexicon has been corrupted.";
            message = Auxil::dialogWordWrap(message);
            QMessageBox::warning(this, "Unable to find checksums for lexicon",
                                 message);
            // warning!
            ok = false;
        }

        lexiconError = QString();

        ok = ok && importDawg(lexicon, importFile, false, &lexiconError,
                              &expectedForwardChecksum);

        ok = ok && importDawg(lexicon, reverseImportFile, true, &lexiconError,
                              &expectedReverseChecksum);
    }
    // (JGM) ??? importFile = a custom lexicon's autoimportFile - what is that?
    else
        ok = importText(lexicon, importFile);

    importStems(lexicon);

    return ok;
}

//---------------------------------------------------------------------------
//  importText
//
//! Import words from a text file.
//
//! @param lexicon the name of the lexicon
//! @param file the file to import words from
//! @return the number of imported words
//---------------------------------------------------------------------------
int
MainWindow::importText(const QString& lexicon, const QString& file)
{
    int imported;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    if ((lexicon == LEXICON_CSW15 || lexicon == LEXICON_CSW19))
        imported = wordEngine->importBinaryFile(lexicon, file, true);
    else
        imported = wordEngine->importTextFile(lexicon, file, true);
    QApplication::restoreOverrideCursor();
    return imported;
}

//---------------------------------------------------------------------------
//  importStems
//
//! Import stems.  XXX: Right now this is hard-coded to load certain North
//! American stems.  Should be more flexible.
//
//! @param lexicon the lexicon name
//! @return the number of imported stems
//---------------------------------------------------------------------------
int
MainWindow::importStems(const QString& lexicon)
{
    QStringList stemFiles;
    stemFiles << (Auxil::getWordsDir() + "/British/6-letter-stems.txt");
    stemFiles << (Auxil::getWordsDir() + "/British/7-letter-stems.txt");

    QString err;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QStringList::iterator it;
    int totalImported = 0;
    for (it = stemFiles.begin(); it != stemFiles.end(); ++it) {
        int imported = wordEngine->importStems(lexicon, *it, &err);
        totalImported += imported;
    }
    QApplication::restoreOverrideCursor();

    return totalImported;
}

//---------------------------------------------------------------------------
//  doTest
//
//! Test something out.
//---------------------------------------------------------------------------
void
MainWindow::doTest()
{
    LetterBag letterBag;
    qDebug("*** doTest!");
}

//---------------------------------------------------------------------------
//  copyQActionPartial
//
//! Copy selected properties of a QAction (for toolbar) (JGM)
//
//! @param orig Source QAction
//! @param dest Target QAction
//---------------------------------------------------------------------------
void
MainWindow::copyQActionPartial(const QAction* orig, QAction* dest)
{
    dest->setIcon(orig->icon());
    dest->setEnabled(orig->isEnabled());
}

//---------------------------------------------------------------------------
//  clearDialogFromList
//
//! Remove pointer to this dialog from the list of child WordVariationDialogs.
//! TODO (JGM) Add an interface to combine this with the same function in
//! WordTableView.
//
//! @param obj pointer to the child WVD object.
//---------------------------------------------------------------------------
void
MainWindow::clearDialogFromList(QObject* obj)
{
    WordVariationDialog *wvd = static_cast<WordVariationDialog*>(obj);
    wordVariationDialogs.removeOne(wvd);
}
