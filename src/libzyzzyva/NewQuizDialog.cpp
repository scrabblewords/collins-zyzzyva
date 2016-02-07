//---------------------------------------------------------------------------
// NewQuizDialog.cpp
//
// A dialog for prompting the user for a quiz.
//
// Copyright 2016 Twilight Century Computing.
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

#include "NewQuizDialog.h"
#include "LexiconSelectWidget.h"
#include "MainSettings.h"
#include "QuizSpec.h"
#include "SearchSpec.h"
#include "SearchSpecForm.h"
#include "ZPushButton.h"
#include "Auxil.h"
#include "Defs.h"
#include <QApplication>
#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QTextStream>
#include <QVBoxLayout>

const QString DIALOG_CAPTION = "New Quiz";
const QString TIMER_PER_QUESTION = "per question";
const QString TIMER_PER_RESPONSE = "per response";

using namespace Defs;

//---------------------------------------------------------------------------
//  NewQuizDialog
//
//! Constructor.
//
//! @param e a word engine
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
NewQuizDialog::NewQuizDialog(QWidget* parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    QVBoxLayout* mainVlay = new QVBoxLayout(this);
    Q_CHECK_PTR(mainVlay);

    lexiconWidget = new LexiconSelectWidget;
    Q_CHECK_PTR(lexiconWidget);
    mainVlay->addWidget(lexiconWidget);

    QGridLayout* quizGlay = new QGridLayout;
    Q_CHECK_PTR(quizGlay);
    mainVlay->addLayout(quizGlay);

    int row = 0;
    QLabel* typeLabel = new QLabel("Quiz Type:");
    Q_CHECK_PTR(typeLabel);
    quizGlay->addWidget(typeLabel, row, 0);

    typeCombo = new QComboBox;
    typeCombo->addItem(Auxil::quizTypeToString(QuizSpec::QuizAnagrams));
    typeCombo->addItem(
        Auxil::quizTypeToString(QuizSpec::QuizAnagramsWithHooks));
    // Commented out until it can be fully implemented
    //typeCombo->addItem(Auxil::quizTypeToString(QuizSpec::QuizBuild));
    typeCombo->addItem(Auxil::quizTypeToString(QuizSpec::QuizWordListRecall));
    typeCombo->addItem(Auxil::quizTypeToString(QuizSpec::QuizHooks));
    typeCombo->setCurrentIndex(typeCombo->findText(
        Auxil::quizTypeToString(QuizSpec::QuizAnagrams)));
    connect(typeCombo, SIGNAL(activated(const QString&)),
            SLOT(typeActivated(const QString&)));
    quizGlay->addWidget(typeCombo, row, 1);

    ++row;
    QLabel* methodLabel = new QLabel("Quiz Method:");
    Q_CHECK_PTR(methodLabel);
    quizGlay->addWidget(methodLabel, row, 0);

    methodCombo = new QComboBox;
    methodCombo->addItem(
        Auxil::quizMethodToString(QuizSpec::StandardQuizMethod));
    methodCombo->addItem(
        Auxil::quizMethodToString(QuizSpec::CardboxQuizMethod));
    methodCombo->setCurrentIndex(methodCombo->findText(
        Auxil::quizMethodToString(QuizSpec::StandardQuizMethod)));
    connect(methodCombo, SIGNAL(activated(const QString&)),
            SLOT(methodActivated(const QString&)));
    quizGlay->addWidget(methodCombo, row, 1);

    ++row;
    QLabel* questionOrderLabel = new QLabel("Question Order:");
    Q_CHECK_PTR(questionOrderLabel);
    quizGlay->addWidget(questionOrderLabel, row, 0);

    QHBoxLayout* questionOrderHlay = new QHBoxLayout;
    Q_CHECK_PTR(questionOrderHlay);
    questionOrderHlay->setMargin(0);
    quizGlay->addLayout(questionOrderHlay, row, 1);

    questionOrderCombo = new QComboBox;
    Q_CHECK_PTR(questionOrderCombo);
    fillQuestionOrderCombo(methodCombo->currentText());
    questionOrderCombo->setCurrentIndex(questionOrderCombo->findText(
        Auxil::quizQuestionOrderToString(QuizSpec::RandomOrder)));
    connect(questionOrderCombo, SIGNAL(activated(const QString&)),
            SLOT(questionOrderActivated(const QString&)));
    questionOrderHlay->addWidget(questionOrderCombo);

    probNumBlanksLabel = new QLabel("Blanks:");;
    Q_CHECK_PTR(probNumBlanksLabel);
    probNumBlanksLabel->hide();
    questionOrderHlay->addWidget(probNumBlanksLabel);

    probNumBlanksSbox = new QSpinBox;
    Q_CHECK_PTR(probNumBlanksSbox);
    probNumBlanksSbox->setMinimum(0);
    probNumBlanksSbox->setMaximum(Defs::MAX_BLANKS);
    probNumBlanksSbox->setValue(MainSettings::getProbabilityNumBlanks());
    connect(probNumBlanksSbox, SIGNAL(valueChanged(int)),
            SLOT(probNumBlanksValueChanged(int)));
    probNumBlanksSbox->hide();
    questionOrderHlay->addWidget(probNumBlanksSbox);

    sourceStack = new QStackedWidget;
    Q_CHECK_PTR(sourceStack);
    mainVlay->addWidget(sourceStack);

    buildWidget = new QWidget;
    Q_CHECK_PTR(buildWidget);
    sourceStack->addWidget(buildWidget);

    QHBoxLayout* responseHlay = new QHBoxLayout(buildWidget);
    Q_CHECK_PTR(responseHlay);
    responseHlay->setMargin(0);

    QLabel* responseLabel = new QLabel("Response length:");
    Q_CHECK_PTR(responseLabel);
    responseHlay->addWidget(responseLabel);

    QLabel* responseMinLabel = new QLabel("Min:");
    Q_CHECK_PTR(responseMinLabel);
    responseHlay->addWidget(responseMinLabel);

    responseMinSbox = new QSpinBox;
    Q_CHECK_PTR(responseMinSbox);
    responseMinSbox->setMinimum(0);
    responseMinSbox->setMaximum(MAX_WORD_LEN);
    responseHlay->addWidget(responseMinSbox);

    QLabel* responseMaxLabel = new QLabel("Max:");
    Q_CHECK_PTR(responseMaxLabel);
    responseHlay->addWidget(responseMaxLabel);

    responseMaxSbox = new QSpinBox;
    Q_CHECK_PTR(responseMaxSbox);
    responseMaxSbox->setMinimum(0);
    responseMaxSbox->setMaximum(MAX_WORD_LEN);
    responseHlay->addWidget(responseMaxSbox);

    searchWidget = new QWidget;
    Q_CHECK_PTR(searchWidget);
    sourceStack->addWidget(searchWidget);

    QVBoxLayout* searchVlay = new QVBoxLayout(searchWidget);
    Q_CHECK_PTR(searchVlay);
    searchVlay->setMargin(0);

    allCardboxButton = new QRadioButton;
    Q_CHECK_PTR(allCardboxButton);
    allCardboxButton->setText("Use all available words");
    allCardboxButton->setChecked(true);
    searchVlay->addWidget(allCardboxButton);

    useSearchButton = new QRadioButton;
    Q_CHECK_PTR(useSearchButton);
    useSearchButton->setText("Use only words matching search specification");
    connect(useSearchButton, SIGNAL(toggled(bool)),
            SLOT(useSearchButtonToggled(bool)));
    searchVlay->addWidget(useSearchButton);

    searchSpecGbox = new QGroupBox("Search Specification");
    Q_CHECK_PTR(searchSpecGbox);
    searchVlay->addWidget(searchSpecGbox);

    QHBoxLayout* specHlay = new QHBoxLayout(searchSpecGbox);
    Q_CHECK_PTR(specHlay);

    searchSpecForm = new SearchSpecForm;
    Q_CHECK_PTR(searchSpecForm);
    connect(searchSpecForm, SIGNAL(contentsChanged()),
            SLOT(searchContentsChanged()));
    connect(searchSpecForm, SIGNAL(returnPressed()), SLOT(accept()));
    specHlay->addWidget(searchSpecForm);

    QHBoxLayout* progressHlay = new QHBoxLayout;
    Q_CHECK_PTR(progressHlay);
    mainVlay->addLayout(progressHlay);

    progressCbox = new QCheckBox("Restore &progress");
    Q_CHECK_PTR(progressCbox);
    progressCbox->setEnabled(false);
    progressHlay->addWidget(progressCbox);

    progressLabel = new QLabel;
    Q_CHECK_PTR(progressLabel);
    progressLabel->setEnabled(false);
    progressHlay->addWidget(progressLabel);

    progressHlay->addStretch(1);

    QHBoxLayout* timerHlay = new QHBoxLayout;
    Q_CHECK_PTR(timerHlay);
    mainVlay->addLayout(timerHlay);

    timerCbox = new QCheckBox("&Timer:");
    Q_CHECK_PTR(timerCbox);
    connect(timerCbox, SIGNAL(toggled(bool)), SLOT(timerToggled(bool)));
    timerHlay->addWidget(timerCbox);

    timerWidget = new QWidget;
    Q_CHECK_PTR(timerWidget);
    timerWidget->setEnabled(false);
    timerHlay->addWidget(timerWidget);

    QHBoxLayout* timerWidgetHlay = new QHBoxLayout(timerWidget);
    Q_CHECK_PTR(timerWidgetHlay);
    timerWidgetHlay->setMargin(0);

    timerSbox = new QSpinBox;
    Q_CHECK_PTR(timerSbox);
    timerSbox->setMinimum(1);
    timerSbox->setMaximum(9999);
    timerSbox->setValue(10);
    timerWidgetHlay->addWidget(timerSbox);

    QLabel* timerLabel = new QLabel("seconds");
    Q_CHECK_PTR(timerLabel);
    timerWidgetHlay->addWidget(timerLabel);

    timerCombo = new QComboBox;
    timerCombo->addItem(TIMER_PER_QUESTION);
    timerCombo->addItem(TIMER_PER_RESPONSE);
    timerCombo->setCurrentIndex(timerCombo->findText(TIMER_PER_RESPONSE));
    timerWidgetHlay->addWidget(timerCombo);

    // OK/Cancel buttons
    QHBoxLayout* buttonHlay = new QHBoxLayout;
    Q_CHECK_PTR(buttonHlay);
    mainVlay->addLayout(buttonHlay);

    ZPushButton* loadQuizButton = new ZPushButton("&Load Quiz...");
    Q_CHECK_PTR(loadQuizButton);
    connect(loadQuizButton, SIGNAL(clicked()), SLOT(loadQuiz()));
    buttonHlay->addWidget(loadQuizButton);

    saveQuizButton = new ZPushButton("&Save Quiz...");
    Q_CHECK_PTR(saveQuizButton);
    connect(saveQuizButton, SIGNAL(clicked()), SLOT(saveQuiz()));
    buttonHlay->addWidget(saveQuizButton);

    buttonHlay->addStretch(1);

    okButton = new ZPushButton("OK");
    Q_CHECK_PTR(okButton);
    okButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    okButton->setDefault(true);
    connect(okButton, SIGNAL(clicked()), SLOT(accept()));
    buttonHlay->addWidget(okButton);

    ZPushButton* cancelButton = new ZPushButton("Cancel");
    Q_CHECK_PTR(cancelButton);
    cancelButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(cancelButton, SIGNAL(clicked()), SLOT(reject()));
    buttonHlay->addWidget(cancelButton);

    updateForm();

    setWindowTitle(DIALOG_CAPTION);
    searchSpecForm->selectInputArea();
}

//---------------------------------------------------------------------------
//  getQuizSpec
//
//! Get a QuizSpec corresponding to the state of the form.
//
//! @return the quiz spec
//---------------------------------------------------------------------------
QuizSpec
NewQuizDialog::getQuizSpec()
{
    QuizSpec::QuizMethod quizMethod =
        Auxil::stringToQuizMethod(methodCombo->currentText());
    QuizSpec::QuizType quizType =
        Auxil::stringToQuizType(typeCombo->currentText());

    quizSpec.setLexicon(lexiconWidget->getCurrentLexicon());
    quizSpec.setType(quizType);
    quizSpec.setMethod(quizMethod);
    quizSpec.setQuestionOrder(
        Auxil::stringToQuizQuestionOrder(questionOrderCombo->currentText()));
    quizSpec.setProbabilityNumBlanks(probNumBlanksSbox->value());

    if (quizType == QuizSpec::QuizBuild) {
        quizSpec.setQuizSourceType(QuizSpec::RandomLettersSource);
    }
    else if ((quizMethod == QuizSpec::CardboxQuizMethod) &&
              allCardboxButton->isChecked())
    {
        quizSpec.setQuizSourceType(QuizSpec::CardboxReadySource);
    }
    else {
        quizSpec.setQuizSourceType(QuizSpec::SearchSource);
        quizSpec.setSearchSpec(searchSpecForm->getSearchSpec());
    }

    QWidget* widget = sourceStack->currentWidget();
    if (widget == buildWidget) {
        quizSpec.setResponseMinLength(responseMinSbox->value());
        quizSpec.setResponseMaxLength(responseMaxSbox->value());
    }

    QuizTimerSpec timerSpec;
    if (timerCbox->isChecked()) {
        timerSpec.setDuration(timerSbox->value());
        QString timerType = timerCombo->currentText();
        if (timerType == TIMER_PER_QUESTION)
            timerSpec.setType(PerQuestion);
        else if (timerType == TIMER_PER_RESPONSE)
            timerSpec.setType(PerResponse);
    }
    quizSpec.setTimerSpec(timerSpec);

    if (!progressCbox->isChecked() &&
        (quizMethod != QuizSpec::CardboxQuizMethod))
    {
        quizSpec.setProgress(QuizProgress());
        quizSpec.setFilename(QString());
        quizSpec.setRandomSeed(0);
        quizSpec.setRandomSeed2(0);
    }

    return quizSpec;
}

//---------------------------------------------------------------------------
//  setQuizSpec
//
//! Set the contents of the dialog form based on the contents of a quiz spec.
//
//! @param spec the quiz spec
//---------------------------------------------------------------------------
void
NewQuizDialog::setQuizSpec(const QuizSpec& spec)
{
    bool lexiconOk = lexiconWidget->setCurrentLexicon(spec.getLexicon());

    // Set method before type, because type may end up changing the method
    methodCombo->setCurrentIndex(methodCombo->findText(
        Auxil::quizMethodToString(spec.getMethod())));
    methodActivated(methodCombo->currentText());
    typeCombo->setCurrentIndex(typeCombo->findText(
        Auxil::quizTypeToString(spec.getType())));
    typeActivated(typeCombo->currentText());
    questionOrderCombo->setCurrentIndex(questionOrderCombo->findText(
        Auxil::quizQuestionOrderToString(spec.getQuestionOrder())));
    questionOrderActivated(questionOrderCombo->currentText());
    probNumBlanksSbox->setValue(spec.getProbabilityNumBlanks());

    QuizSpec::QuizSourceType sourceType = spec.getQuizSourceType();
    if (sourceType == QuizSpec::SearchSource) {
        useSearchButton->setChecked(true);
        searchSpecForm->setSearchSpec(spec.getSearchSpec());
    }
    else if (sourceType == QuizSpec::CardboxReadySource) {
        allCardboxButton->setChecked(true);
        searchSpecForm->setSearchSpec(SearchSpec());
    }

    int responseMin = spec.getResponseMinLength();
    if (responseMin)
        responseMinSbox->setValue(responseMin);

    int responseMax = spec.getResponseMaxLength();
    if (responseMax)
        responseMaxSbox->setValue(responseMax);

    timerCbox->setChecked(false);
    timerSbox->setValue(0);
    timerCombo->setCurrentIndex(timerCombo->findText(TIMER_PER_RESPONSE));
    QuizTimerSpec timerSpec = spec.getTimerSpec();
    if (timerSpec.getType() != NoTimer) {
        timerCbox->setChecked(true);
        timerSbox->setValue(timerSpec.getDuration());
        switch (timerSpec.getType()) {
            case PerQuestion:
            timerCombo->setCurrentIndex(
                timerCombo->findText(TIMER_PER_QUESTION));
            break;
            case PerResponse:
            timerCombo->setCurrentIndex(
                timerCombo->findText(TIMER_PER_RESPONSE));
            break;
            default: break;
        }
    }
    if (spec.getMethod() == QuizSpec::StandardQuizMethod) {
        progressCbox->setEnabled(true);
        progressCbox->setChecked(true);
        QuizProgress progress = spec.getProgress();
        int questionNum = progress.isEmpty() ? 1 : progress.getQuestion() + 1;
        progressLabel->setEnabled(true);
        progressLabel->setText(QString("(Question %1)").arg(questionNum));
    }
    quizSpec = spec;

    // Display warning of lexicon could not be selected
    if (!lexiconOk) {
        QString caption = "Error Setting Lexicon";
        QString message = "Cannot open lexicon '" + spec.getLexicon() + "'";
        message = Auxil::dialogWordWrap(message);
        QMessageBox::warning(this, caption, message);
    }
}

//---------------------------------------------------------------------------
//  typeActivated
//
//! Called when the contents of the Quiz Type combo box are changed.  Disable
//! the Random checkbox if the Word List Recall type is selected.  Also change
//! the method to Standard if the Build or Word List Recall type is selected.
//
//! @param text the text in the combo box
//---------------------------------------------------------------------------
void
NewQuizDialog::typeActivated(const QString& text)
{
    QuizSpec::QuizType type = Auxil::stringToQuizType(text);
    if ((type == QuizSpec::QuizWordListRecall) || (type == QuizSpec::QuizBuild))
    {
        methodCombo->setEnabled(false);
        methodCombo->setCurrentIndex(methodCombo->findText(
            Auxil::quizMethodToString(QuizSpec::StandardQuizMethod)));
    }
    else {
        methodCombo->setEnabled(true);
    }
    updateForm();
    disableProgress();
    clearFilename();
}

//---------------------------------------------------------------------------
//  methodActivated
//
//! Called when the contents of the Quiz Method combo box are changed.
//! Disable the Restore Progress and Random checkboxes if the Cardbox method
//! is selected.
//
//! @param text the text in the combo box
//---------------------------------------------------------------------------
void
NewQuizDialog::methodActivated(const QString& text)
{
    fillQuestionOrderCombo(text);
    updateForm();
    disableProgress();
    clearFilename();
}

//---------------------------------------------------------------------------
//  useSearchButtonToggled
//
//! Called when the Use Search Specification button is toggled.  Enable or
//! disable the search specification area.
//
//! @param on whether the search button is active
//---------------------------------------------------------------------------
void
NewQuizDialog::useSearchButtonToggled(bool)
{
    updateForm();
}

//---------------------------------------------------------------------------
//  timerToggled
//
//! Called when the Timer checkbox is toggled.  Disable the timer
//! configuration unless the Timer checkbox is checked.
//
//! @param on whether the checkbox is checked
//---------------------------------------------------------------------------
void
NewQuizDialog::timerToggled(bool on)
{
    timerWidget->setEnabled(on);
}

//---------------------------------------------------------------------------
//  searchContentsChanged
//
//! Called when the contents of the search form change.
//---------------------------------------------------------------------------
void
NewQuizDialog::searchContentsChanged()
{
    disableProgress();
    clearFilename();
}

//---------------------------------------------------------------------------
//  questionOrderActivated
//
//! Called when the contents of the Question Order combo box are changed.
//
//! @param text the new contents of the combo box
//---------------------------------------------------------------------------
void
NewQuizDialog::questionOrderActivated(const QString& str)
{
    disableProgress();
    clearFilename();

    QuizSpec::QuestionOrder order = Auxil::stringToQuizQuestionOrder(str);
    switch (order) {
        case QuizSpec::ProbabilityOrder:
        probNumBlanksLabel->show();
        probNumBlanksSbox->show();
        break;
        default:
        probNumBlanksLabel->hide();
        probNumBlanksSbox->hide();
        break;
    }
}

//---------------------------------------------------------------------------
//  probNumBlanksValueChanged
//
//! Called when the value of the Blanks spin box is changed.
//
//! @param text the new value of the spin box
//---------------------------------------------------------------------------
void
NewQuizDialog::probNumBlanksValueChanged(int)
{
    disableProgress();
    clearFilename();
}

//---------------------------------------------------------------------------
//  loadQuiz
//
//! Load a quiz spec from a file.
//---------------------------------------------------------------------------
void
NewQuizDialog::loadQuiz()
{
    QString filename = QFileDialog::getOpenFileName(this, "Load Quiz",
        Auxil::getQuizDir() + "/saved", "Zyzzyva Quiz Files (*.zzq)");

    if (filename.isEmpty())
        return;

    QFile file (filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString caption = "Error Opening Quiz File";
        QString message = "Cannot open file '" + filename + "': " +
            file.errorString();
        message = Auxil::dialogWordWrap(message);
        QMessageBox::warning(this, caption, message);
        return;
    }

    QString errorMsg;
    QuizSpec spec;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    bool ok = spec.fromXmlFile(file, &errorMsg);
    QApplication::restoreOverrideCursor();

    if (!ok) {
        QString caption = "Error in Quiz File";
        QString message = "Error in quiz file.\n" + errorMsg;
        message = Auxil::dialogWordWrap(message);
        QMessageBox::warning(this, caption, message);
        return;
    }

    setQuizSpec(spec);
}

//---------------------------------------------------------------------------
//  saveQuiz
//
//! Save the current quiz spec to a file.
//---------------------------------------------------------------------------
void
NewQuizDialog::saveQuiz()
{
    // Try saving in the same location as the spec's current filename
    QString startDir = Auxil::getQuizDir() + "/saved";
    QString currentFilename = quizSpec.getFilename();
    if (!currentFilename.isEmpty()) {
        currentFilename.remove("/[^/]+$");
        startDir = currentFilename;
    }

    QString filename = QFileDialog::getSaveFileName(this, "Save Quiz",
        startDir, "Zyzzyva Quiz Files (*.zzq)");

    if (filename.isEmpty())
        return;

    bool filenameEdited = false;
    if (!filename.endsWith(".zzq", Qt::CaseInsensitive)) {
        filename += ".zzq";
        filenameEdited = true;
    }

    QFile file (filename);
    if (filenameEdited && file.exists()) {
        QString caption = "Overwrite Existing File?";
        QString message = "The file already exists.  Overwrite it?";
        message = Auxil::dialogWordWrap(message);
        int code = QMessageBox::warning(0, caption, message,
                                        QMessageBox::Yes | QMessageBox::No,
                                        QMessageBox::No);
        if (code != QMessageBox::Yes)
            return;
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QString caption = "Error Saving Quiz";
        QString message = "Cannot save quiz:\n" + file.errorString() + ".";
        message = Auxil::dialogWordWrap(message);
        QMessageBox::warning(this, caption, message);
        return;
    }

    QTextStream stream (&file);
    stream << getQuizSpec().asXml();
}

//---------------------------------------------------------------------------
//  disableProgress
//
//! Disable the progress checkbox and set its state to unchecked.
//---------------------------------------------------------------------------
void
NewQuizDialog::disableProgress()
{
    progressCbox->setChecked(false);
    progressCbox->setEnabled(false);
    progressLabel->setEnabled(false);
    progressLabel->setText(QString());
}

//---------------------------------------------------------------------------
//  clearFilename
//
//! Remove the filename associated with the quiz specification.
//---------------------------------------------------------------------------
void
NewQuizDialog::clearFilename()
{
    quizSpec.setFilename(QString());
}

//---------------------------------------------------------------------------
//  updateForm
//
//! Update the state of the form based on the contents of the Quiz Type and
//! Quiz Method combo boxes.
//---------------------------------------------------------------------------
void
NewQuizDialog::updateForm()
{
    QuizSpec::QuizType type =
        Auxil::stringToQuizType(typeCombo->currentText());
    QuizSpec::QuizMethod method =
        Auxil::stringToQuizMethod(methodCombo->currentText());

    if (method == QuizSpec::CardboxQuizMethod) {
        disableProgress();
        questionOrderCombo->setEnabled(true);
        questionOrderCombo->setCurrentIndex(questionOrderCombo->findText(
            Auxil::quizQuestionOrderToString(QuizSpec::ScheduleOrder)));
        questionOrderActivated(questionOrderCombo->currentText());

        sourceStack->setCurrentWidget(searchWidget);
        allCardboxButton->show();
        useSearchButton->show();
        searchSpecGbox->setEnabled(useSearchButton->isChecked());
    }

    else {
        if ((type == QuizSpec::QuizWordListRecall) ||
            (type == QuizSpec::QuizBuild))
        {
            questionOrderCombo->setEnabled(false);
            questionOrderCombo->setCurrentIndex(questionOrderCombo->findText(
                Auxil::quizQuestionOrderToString(QuizSpec::RandomOrder)));
            questionOrderActivated(questionOrderCombo->currentText());
        }

        else {
            progressCbox->setEnabled(true);
            questionOrderCombo->setEnabled(true);
        }

        allCardboxButton->hide();
        useSearchButton->hide();

        if (type == QuizSpec::QuizBuild) {
            sourceStack->setCurrentWidget(buildWidget);
        }
        else {
            sourceStack->setCurrentWidget(searchWidget);
            searchSpecGbox->setEnabled(true);
        }
    }
}

//---------------------------------------------------------------------------
//  fillQuestionOrderCombo
//
//! Fill the Question Order combo box with allowed values associated with a
//! quiz method.  For example, the Schedule question order is only valid with
//! in association with the Cardbox quiz method.
//
//! @param method the quiz method string
//---------------------------------------------------------------------------
void
NewQuizDialog::fillQuestionOrderCombo(const QString& method)
{
    QString prevText = questionOrderCombo->currentText();
    QuizSpec::QuizMethod meth = Auxil::stringToQuizMethod(method);

    switch (meth) {
        case QuizSpec::StandardQuizMethod:
        questionOrderCombo->clear();
        questionOrderCombo->addItem(
            Auxil::quizQuestionOrderToString(QuizSpec::RandomOrder));
        questionOrderCombo->addItem(
            Auxil::quizQuestionOrderToString(QuizSpec::AlphabeticalOrder));
        questionOrderCombo->addItem(
            Auxil::quizQuestionOrderToString(QuizSpec::ProbabilityOrder));
        questionOrderCombo->addItem(
            Auxil::quizQuestionOrderToString(QuizSpec::PlayabilityOrder));
        break;

        case QuizSpec::CardboxQuizMethod:
        questionOrderCombo->clear();
        questionOrderCombo->addItem(
            Auxil::quizQuestionOrderToString(QuizSpec::ScheduleOrder));
        questionOrderCombo->addItem(
            Auxil::quizQuestionOrderToString(QuizSpec::ScheduleZeroFirstOrder));
        break;

        default: break;
    }

    int index = questionOrderCombo->findText(prevText);
    questionOrderCombo->setCurrentIndex(index >= 0 ? index : 0);
}
