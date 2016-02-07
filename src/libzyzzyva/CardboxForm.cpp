//---------------------------------------------------------------------------
// CardboxForm.cpp
//
// A form for querying and editing the contents of the cardbox system.
//
// Copyright 2016 Twilight Century Computing.
// Copyright 2007-2012 North American SCRABBLE Players Association.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//---------------------------------------------------------------------------

#include "CardboxForm.h"
#include "LexiconSelectWidget.h"
#include "QuizDatabase.h"
#include "WordEngine.h"
#include "WordValidator.h"
#include "ZPushButton.h"
#include "Auxil.h"
#include "Defs.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>

using namespace Defs;

const QString TITLE_PREFIX = "Cardbox";
const int REFRESH_MSECS = 120000;

//---------------------------------------------------------------------------
//  CardboxForm
//
//! Constructor.
//
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
CardboxForm::CardboxForm(WordEngine* e, QWidget* parent, Qt::WindowFlags f)
    : ActionForm(CardboxFormType, parent, f), wordEngine(e)
    //cardboxCountModel(0), cardboxDaysModel(0), cardboxContentsModel(0)
{
    QVBoxLayout* mainVlay = new QVBoxLayout(this);
    Q_CHECK_PTR(mainVlay);
    mainVlay->setMargin(MARGIN);
    mainVlay->setSpacing(SPACING);

    QHBoxLayout* controlHlay = new QHBoxLayout;
    controlHlay->setMargin(0);
    controlHlay->setSpacing(SPACING);
    mainVlay->addLayout(controlHlay);

    controlHlay->addStretch(1);

    QGridLayout* controlGlay = new QGridLayout;
    controlGlay->setMargin(0);
    controlGlay->setSpacing(SPACING);
    controlHlay->addLayout(controlGlay);

    QLabel* lexiconLabel = new QLabel;
    lexiconLabel->setText("Lexicon:");
    controlGlay->addWidget(lexiconLabel, 0, 0);

    lexiconWidget = new LexiconSelectWidget;
    lexiconWidget->setLabelVisible(false);
    connect(lexiconWidget->getComboBox(), SIGNAL(activated(const QString&)),
        SLOT(lexiconActivated(const QString&)));
    controlGlay->addWidget(lexiconWidget, 0, 1);

    QLabel* quizTypeLabel = new QLabel;
    Q_CHECK_PTR(quizTypeLabel);
    quizTypeLabel->setText("Quiz Type:");
    controlGlay->addWidget(quizTypeLabel, 1, 0);

    quizTypeCombo = new QComboBox;
    Q_CHECK_PTR(quizTypeCombo);
    quizTypeCombo->addItem(Auxil::quizTypeToString(QuizSpec::QuizAnagrams));
    quizTypeCombo->addItem(
        Auxil::quizTypeToString(QuizSpec::QuizAnagramsWithHooks));
    quizTypeCombo->addItem(Auxil::quizTypeToString(QuizSpec::QuizHooks));
    controlGlay->addWidget(quizTypeCombo, 1, 1);

    ZPushButton* refreshButton = new ZPushButton;
    refreshButton->setText("&Refresh");
    refreshButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(refreshButton, SIGNAL(clicked()), SLOT(refreshClicked()));
    controlGlay->addWidget(refreshButton, 2, 0, 1, 2, Qt::AlignHCenter);

    controlHlay->addStretch(1);

    QFrame* topSepFrame = new QFrame;
    Q_CHECK_PTR(topSepFrame);
    topSepFrame->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    mainVlay->addWidget(topSepFrame);

    QGridLayout* cardboxGlay = new QGridLayout;
    Q_CHECK_PTR(cardboxGlay);
    cardboxGlay->setSpacing(SPACING);
    mainVlay->addLayout(cardboxGlay);

    QLabel* cardboxCountLabel = new QLabel;
    Q_CHECK_PTR(cardboxCountLabel);
    cardboxCountLabel->setText("Questions in each cardbox:");
    cardboxGlay->addWidget(cardboxCountLabel, 0, 0);

    cardboxCountTree = new QTreeWidget;
    Q_CHECK_PTR(cardboxCountTree);
    QStringList cardboxCountTreeHeaders;
    cardboxCountTreeHeaders.append("Cardbox");
    cardboxCountTreeHeaders.append("Count");
    cardboxCountTreeHeaders.append("Due");
    cardboxCountTree->setHeaderLabels(cardboxCountTreeHeaders);
    cardboxCountTree->setRootIsDecorated(false);
    cardboxGlay->addWidget(cardboxCountTree, 1, 0);

    QLabel* cardboxDaysLabel = new QLabel;
    cardboxDaysLabel->setText("Questions scheduled:");
    cardboxGlay->addWidget(cardboxDaysLabel, 0, 1);

    cardboxDaysTree = new QTreeWidget;
    Q_CHECK_PTR(cardboxDaysTree);
    QStringList cardboxDaysTreeHeaders;
    cardboxDaysTreeHeaders.append("Due");
    cardboxDaysTreeHeaders.append("Count");
    cardboxDaysTree->setHeaderLabels(cardboxDaysTreeHeaders);
    cardboxDaysTree->setRootIsDecorated(false);
    cardboxGlay->addWidget(cardboxDaysTree, 1, 1);

    //cardboxContentsView = new QTreeView;
    //Q_CHECK_PTR(cardboxContentsView);
    //mainVlay->addWidget(cardboxContentsView);

    QFrame* bottomSepFrame = new QFrame;
    Q_CHECK_PTR(bottomSepFrame);
    bottomSepFrame->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    mainVlay->addWidget(bottomSepFrame);

    QHBoxLayout* questionHlay = new QHBoxLayout;
    Q_CHECK_PTR(questionHlay);
    questionHlay->setMargin(0);
    questionHlay->setSpacing(SPACING);
    mainVlay->addLayout(questionHlay);

    QLabel* questionLabel = new QLabel;
    Q_CHECK_PTR(questionLabel);
    questionLabel->setText("Get data for question:");
    questionHlay->addWidget(questionLabel);

    questionLine = new QLineEdit;
    Q_CHECK_PTR(questionLine);
    WordValidator* validator = new WordValidator(questionLine);
    validator->setOptions(WordValidator::AllowQuestionMarks);
    questionLine->setValidator(validator);
    connect(questionLine, SIGNAL(returnPressed()), SLOT(questionDataClicked()));
    questionHlay->addWidget(questionLine);

    QPushButton* questionButton = new QPushButton;
    Q_CHECK_PTR(questionButton);
    questionButton->setText("Get Info");
    questionButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(questionButton, SIGNAL(clicked()), SLOT(questionDataClicked()));
    questionHlay->addWidget(questionButton);

    questionDataLabel = new QLabel;
    Q_CHECK_PTR(questionDataLabel);
    questionDataLabel->setAlignment(Qt::AlignTop);
    mainVlay->addWidget(questionDataLabel, 1);

    //mainVlay->addStretch(0);

    //connect(&refreshTimer, SIGNAL(timeout()), SLOT(refreshClicked()));
    //refreshTimer.start(REFRESH_MSECS);

    lexiconActivated(lexiconWidget->getCurrentLexicon());
    refreshClicked();
}

//---------------------------------------------------------------------------
//  getIcon
//
//! Returns the current icon.
//
//! @return the current icon
//---------------------------------------------------------------------------
QIcon
CardboxForm::getIcon() const
{
    return QIcon(":/cardbox-icon");
}

//---------------------------------------------------------------------------
//  getTitle
//
//! Returns the current title string.
//
//! @return the current title string
//---------------------------------------------------------------------------
QString
CardboxForm::getTitle() const
{
    return TITLE_PREFIX;
}

//---------------------------------------------------------------------------
//  getStatusString
//
//! Returns the current status string.
//
//! @return the current status string
//---------------------------------------------------------------------------
QString
CardboxForm::getStatusString() const
{
    return QString();
}

//---------------------------------------------------------------------------
//  getDetailsString
//
//! Returns the current details string.
//
//! @return the current details string
//---------------------------------------------------------------------------
QString
CardboxForm::getDetailsString() const
{
    return detailsString;
}

//---------------------------------------------------------------------------
//  lexiconActivated
//
//! Called when the lexicon combo box is activated
//! @param lexicon the activated lexicon
//---------------------------------------------------------------------------
void
CardboxForm::lexiconActivated(const QString& lexicon)
{
    detailsString = Auxil::lexiconToDetails(lexicon);
    emit detailsChanged(detailsString);
}

//---------------------------------------------------------------------------
//  refreshClicked
//
//! Called when the Refresh button is clicked.
//---------------------------------------------------------------------------
void
CardboxForm::refreshClicked()
{
    QString lexicon = lexiconWidget->getCurrentLexicon();
    QString quizType = quizTypeCombo->currentText();
    QuizDatabase db (lexicon, quizType);
    if (!db.isValid()) {
        // FIXME: pop up a warning
        return;
    }

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    cardboxCountTree->clear();
    QMap<int, int> cardboxCounts = db.getCardboxCounts();
    QMap<int, int> cardboxDueCounts = db.getCardboxDueCounts();
    QMapIterator<int, int> it (cardboxCounts);
    while (it.hasNext()) {
        it.next();
        int cardbox = it.key();

        QStringList strings;
        strings.append(QString("Cardbox %1").arg(QString::number(cardbox)));
        strings.append(QString::number(it.value()));
        strings.append(QString::number(cardboxDueCounts.value(cardbox)));
        cardboxCountTree->addTopLevelItem(new QTreeWidgetItem(strings));
    }

    cardboxDaysTree->clear();
    QMap<int, int> dayCounts = db.getScheduleDayCounts();
    QMapIterator<int, int> jt (dayCounts);
    while (jt.hasNext()) {
        jt.next();

        QColor color;
        int dueDays = jt.key();
        QString dueDaysStr;
        if (dueDays == -1) {
            dueDaysStr = "Due Now";
            color = Qt::magenta;
        }
        else if (dueDays == 0) {
            dueDaysStr = "Due Soon";
            color = Qt::blue;
        }
        else if (dueDays < 0) {
            dueDays = abs(dueDays) - 1;
            dueDaysStr = QString("Due %1 day%2 ago").arg(dueDays).arg(
                dueDays == 1 ? QString() : QString("s"));
            color = Qt::red;
        }
        else {
            dueDaysStr = QString("Due in %1 day%2").arg(dueDays).arg(
                dueDays == 1 ? QString() : QString("s"));
            color = Qt::black;
        }

        QStringList strings;
        strings.append(dueDaysStr);
        strings.append(QString::number(jt.value()));
        QTreeWidgetItem* item = new QTreeWidgetItem(strings);
        item->setForeground(0, QBrush(color));
        cardboxDaysTree->addTopLevelItem(item);
    }

    cardboxDaysTree->resizeColumnToContents(0);

    // Refresh cardbox contents

    // Attempt using QSqlTableModel:
    // XXX: This would be lovely except that SQLite locks the database and
    // doesn't let go, thus preventing quiz responses from being counted
    //if (!cardboxContentsModel) {
    //    cardboxContentsModel = new QSqlTableModel(this, *sqlDb);
    //    Q_CHECK_PTR(cardboxContentsModel);
    //    cardboxContentsView->setModel(cardboxContentsModel);
    //    cardboxContentsView->setSortingEnabled(true);
    //    cardboxContentsModel->setTable("questions");
    //    cardboxContentsModel->select();
    //    while (cardboxContentsModel->canFetchMore()) {
    //        qDebug("Fetching more for cardbox contents model");
    //        cardboxContentsModel->fetchMore();
    //    }
    //}

    // Attempt using QSqlQueryModel:
    //if (!cardboxContentsModel) {
    //    cardboxContentsModel = new QSqlQueryModel;
    //    Q_CHECK_PTR(cardboxContentsModel);
    //    cardboxContentsView->setModel(cardboxContentsModel);
    //}
    //queryStr =
    //    "SELECT question, cardbox, correct, incorrect, streak, "
    //    "next_scheduled FROM questions WHERE cardbox NOT NULL "
    //    "ORDER BY next_scheduled";
    //
    //cardboxContentsModel->setQuery(queryStr, *sqlDb);
    //while (cardboxContentsModel->canFetchMore()) {
    //    qDebug("Fetching more for cardbox contents model");
    //    cardboxContentsModel->fetchMore();
    //}
    //
    //cardboxContentsModel->setHeaderData(0, Qt::Horizontal, "Question");
    //cardboxContentsModel->setHeaderData(1, Qt::Horizontal, "Cardbox");
    //cardboxContentsModel->setHeaderData(2, Qt::Horizontal, "Correct");
    //cardboxContentsModel->setHeaderData(3, Qt::Horizontal, "Incorrect");
    //cardboxContentsModel->setHeaderData(4, Qt::Horizontal, "Streak");
    //cardboxContentsModel->setHeaderData(5, Qt::Horizontal, "Next Scheduled");

    QApplication::restoreOverrideCursor();
}

//---------------------------------------------------------------------------
//  questionDataClicked
//
//! Called when the Get Info button is clicked.  Display information about the
//! question being queried.
//---------------------------------------------------------------------------
void
CardboxForm::questionDataClicked()
{
    QString question = Auxil::getCanonicalSearchString(questionLine->text());
    if (question.isEmpty())
        return;

    QString lexicon = lexiconWidget->getCurrentLexicon();
    QString quizType = quizTypeCombo->currentText();
    QuizDatabase db (lexicon, quizType);
    if (!db.isValid()) {
        // FIXME: pop up a warning
        return;
    }

    QuizSpec::QuizType qtype = Auxil::stringToQuizType(quizType);
    if ((qtype == QuizSpec::QuizAnagrams) ||
        (qtype == QuizSpec::QuizAnagramsWithHooks))
    {
        question = Auxil::getAlphagram(question);
    }

    QString resultStr;

    QuizDatabase::QuestionData data = db.getQuestionData(question);
    if (!data.valid) {
        resultStr = QString("<font color=\"red\">Not in Database</font>");
    }
    else {
        // XXX: This code was basically stolen from QuizForm::setQuestionStats
        // - should be consolidated somewhere
        QString streak;
        if (data.streak == 0)
            streak = "none";
        else if (data.streak > 0)
            streak = QString::number(data.streak) + " correct";
        else if (data.streak < 0)
            streak = QString::number(-data.streak) + " incorrect";

        QString lastCorrect = "never";
        if (data.lastCorrect) {

            QDateTime lastCorrectDate;
            lastCorrectDate.setTime_t(data.lastCorrect);
            QDateTime now = QDateTime::currentDateTime();
            int numDays = now.daysTo(lastCorrectDate);

            QString format = "yyyy-MM-dd hh:mm:ss";
            QString delta;
            QString daysStr = (abs(numDays) == 1 ? QString("day")
                                                 : QString("days"));
            if (numDays == 0)
                delta = "today";
            else if (numDays < 0)
                delta = QString::number(-numDays) + " " + daysStr + " ago";
            else if (numDays > 0)
                delta = QString::number(numDays) + " " + daysStr + " from now";

            lastCorrect = lastCorrectDate.toString(format) + " (" + delta + ")";
        }

        int correct = data.numCorrect;
        int incorrect = data.numIncorrect;
        int total = correct + incorrect;
        double pct = total ? (correct * 100.0) / total : 0;

        resultStr += question + "<br><b>Overall Record:</b> " +
            QString::number(correct) + "/" + QString::number(total) +
            " (" + QString::number(pct, 'f', 1) + "%)<br><b>Streak:</b> " +
            streak + "<br><b>Last Correct:</b> " + lastCorrect;


        if (data.cardbox >= 0) {
            QDateTime nextDate;
            nextDate.setTime_t(data.nextScheduled);
            QDateTime now = QDateTime::currentDateTime();
            int numDays = now.daysTo(nextDate);

            QString format = "yyyy-MM-dd hh:mm:ss";

            resultStr += "<br><b>Cardbox:</b> " +
                QString::number(data.cardbox) +
                "<br><b>Next Scheduled:</b> " + nextDate.toString(format) +
                " (" + QString::number(numDays) + " day" +
                (numDays == 1 ? QString() : QString("s")) + ")";
        }
    }

    questionDataLabel->setText(resultStr);

    // Select the question input area
    questionLine->setFocus();
    questionLine->setSelection(0, questionLine->text().length());
}
