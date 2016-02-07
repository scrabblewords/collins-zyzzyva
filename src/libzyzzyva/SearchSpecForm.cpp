//---------------------------------------------------------------------------
// SearchSpecForm.cpp
//
// A form for specifying a search specification.
//
// Copyright 2016 Twilight Century Computing.
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

#include "SearchSpecForm.h"
#include "SearchSpec.h"
#include "SearchConditionForm.h"
#include "ZPushButton.h"
#include "Auxil.h"
#include "Defs.h"
#include <QApplication>
#include <QFileDialog>
#include <QFrame>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QTextStream>
#include <QVBoxLayout>

using namespace Defs;

const int MAX_CONDITIONS = 8;

//---------------------------------------------------------------------------
//  SearchSpecForm
//
//! Constructor.
//
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
SearchSpecForm::SearchSpecForm(QWidget* parent, Qt::WindowFlags f)
    : QFrame(parent, f)
{
    QVBoxLayout* mainVlay = new QVBoxLayout(this);
    Q_CHECK_PTR(mainVlay);
    mainVlay->setMargin(0);
    mainVlay->setSpacing(SPACING);

    //QButtonGroup* radioGroup = new QButtonGroup(this);
    //Q_CHECK_PTR(radioGroup);
    //radioGroup->hide();

    //QFrame* radioFrame = new QFrame;
    //Q_CHECK_PTR(radioFrame);
    //mainVlay->addWidget(radioFrame);

    //QHBoxLayout* radioHlay = new QHBoxLayout(radioFrame, 0, SPACING);
    //Q_CHECK_PTR(radioHlay);

    //conjunctionRadio = new QRadioButton("Match all of the following");
    //Q_CHECK_PTR(conjunctionRadio);
    //conjunctionRadio->setChecked(true);
    //radioGroup->insert(conjunctionRadio, 1);
    //radioHlay->addWidget(conjunctionRadio);

    //QRadioButton* disjunctionRadio =
    //    new QRadioButton("Match any of the following");
    //Q_CHECK_PTR(disjunctionRadio);
    //radioGroup->insert(disjunctionRadio, 1);
    //radioHlay->addWidget(disjunctionRadio);

    QHBoxLayout* conditionHlay = new QHBoxLayout;
    conditionHlay->setSpacing(SPACING);
    Q_CHECK_PTR(conditionHlay);
    mainVlay->addLayout(conditionHlay);
    mainVlay->setStretchFactor(conditionHlay, 1);

    conditionVlay = new QVBoxLayout;
    conditionVlay->setSpacing(SPACING);
    Q_CHECK_PTR(conditionVlay);
    conditionHlay->addLayout(conditionVlay);

    QHBoxLayout* buttonHlay = new QHBoxLayout;
    buttonHlay->setSpacing(SPACING);
    Q_CHECK_PTR(buttonHlay);
    mainVlay->addLayout(buttonHlay);

    buttonHlay->addStretch(1);

    loadButton = new ZPushButton("L&oad Search...");
    Q_CHECK_PTR(loadButton);
    loadButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(loadButton, SIGNAL(clicked()), SLOT(loadSearch()));
    buttonHlay->addWidget(loadButton);

    saveButton = new ZPushButton("S&ave Search...");
    Q_CHECK_PTR(saveButton);
    saveButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(saveButton, SIGNAL(clicked()), SLOT(saveSearch()));
    buttonHlay->addWidget(saveButton);

    connect(this, SIGNAL(contentsChanged()), SLOT(contentsChangedSlot()));
    saveButton->setEnabled(false);

    addMapper = new QSignalMapper(this);
    Q_CHECK_PTR(addMapper);
    connect(addMapper, SIGNAL(mapped(int)), SLOT(insertConditionForm(int)));

    deleteMapper = new QSignalMapper(this);
    Q_CHECK_PTR(deleteMapper);
    connect(deleteMapper, SIGNAL(mapped(int)), SLOT(removeConditionForm(int)));

    insertConditionForm(0);
}

//---------------------------------------------------------------------------
//  getSearchSpec
//
//! Get a SearchSpec object corresponding to the values in the form.
//
//! @return the search spec
//---------------------------------------------------------------------------
SearchSpec
SearchSpecForm::getSearchSpec() const
{
    SearchSpec spec;
    spec.conjunction = true;
    //spec.conjunction = conjunctionRadio->isChecked();
    QListIterator<SearchConditionForm*> it (conditionForms);
    while (it.hasNext()) {
        const SearchConditionForm* form = it.next();
        if (form->isValid())
            spec.conditions.append(form->getSearchCondition());
    }
    return spec;
}

//---------------------------------------------------------------------------
//  setSearchSpec
//
//! Set the contents of the form according to the contents of a search spec.
//
//! @param spec the search spec
//---------------------------------------------------------------------------
void
SearchSpecForm::setSearchSpec(const SearchSpec& spec)
{
    if (spec.conditions.empty())
        return;

    qDeleteAll(conditionForms);
    conditionForms.clear();

    //conjunctionRadio->setChecked(spec->conjunction);
    QListIterator<SearchCondition> cit (spec.conditions);
    while (cit.hasNext()) {
        insertConditionForm(conditionForms.size());
        SearchConditionForm* conditionForm = conditionForms.last();
        conditionForm->setSearchCondition(cit.next());
        //conditionForm->setAddEnabled(spec.conditions.size() < MAX_CONDITIONS);
        //conditionForm->setDeleteEnabled(spec.conditions.size() > 1);
    }
}

//---------------------------------------------------------------------------
//  isValid
//
//! Determine whether the input in all visible search condition forms is
//! valid.
//
//! @return true if valid, false otherwise
//---------------------------------------------------------------------------
bool
SearchSpecForm::isValid() const
{
    foreach (SearchConditionForm* form, conditionForms) {
        if (!form->isValid())
            return false;
    }
    return true;
}

//---------------------------------------------------------------------------
//  contentsChangedSlot
//
//! Called when the contents of the form change.  Enables or disables the Save
//! button appropriately.
//---------------------------------------------------------------------------
void
SearchSpecForm::contentsChangedSlot()
{
    saveButton->setEnabled(isValid());
}

//---------------------------------------------------------------------------
//  insertConditionForm
//
//! Insert a condition form into the layout.
//
//! @param index the index where the form should be inserted
//---------------------------------------------------------------------------
void
SearchSpecForm::insertConditionForm(int index)
{
    if (conditionForms.size() == MAX_CONDITIONS)
        return;

    // Remap signals for any search condition forms after the one to be
    // inserted
    for (int i = index; i < conditionForms.size(); ++i) {
        SearchConditionForm* form = conditionForms[i];
        addMapper->setMapping(form, i + 2);
        deleteMapper->setMapping(form, i + 1);
    }

    SearchConditionForm* form = new SearchConditionForm(this);
    Q_CHECK_PTR(form);
    connect(form, SIGNAL(returnPressed()), SIGNAL(returnPressed()));
    connect(form, SIGNAL(contentsChanged()), SIGNAL(contentsChanged()));
    connect(form, SIGNAL(addClicked()), addMapper, SLOT(map()));
    addMapper->setMapping(form, index + 1);
    connect(form, SIGNAL(deleteClicked()), deleteMapper, SLOT(map()));
    deleteMapper->setMapping(form, index);

    conditionVlay->insertWidget(index, form);
    conditionForms.insert(index, form);

    if (conditionForms.size() == 1) {
        form->setDeleteEnabled(false);
    }
    else {
        QListIterator<SearchConditionForm*> it (conditionForms);
        while (it.hasNext()) {
            SearchConditionForm* conditionForm = it.next();
            conditionForm->setDeleteEnabled(true);
            if (conditionForms.size() == MAX_CONDITIONS)
                conditionForm->setAddEnabled(false);
        }
    }

    contentsChanged();
}

//---------------------------------------------------------------------------
//  removeConditionForm
//
//! Remove a condition form from the layout.
//
//! @param index the index of the form to be removed
//---------------------------------------------------------------------------
void
SearchSpecForm::removeConditionForm(int index)
{
    if ((index < 0) || (index >= conditionForms.size()))
        return;

    // Remap signals for any search condition forms after the one to be
    // inserted
    for (int i = index + 1; i < conditionForms.size(); ++i) {
        SearchConditionForm* form = conditionForms[i];
        addMapper->setMapping(form, i);
        deleteMapper->setMapping(form, i - 1);
    }

    SearchConditionForm* form = conditionForms.value(index);
    delete form;
    conditionForms.removeAt(index);

    if (conditionForms.size() == 1) {
        conditionForms.first()->setDeleteEnabled(false);
    }
    else {
        QListIterator<SearchConditionForm*> it (conditionForms);
        while (it.hasNext()) {
            SearchConditionForm* conditionForm = it.next();
            conditionForm->setAddEnabled(true);
        }
    }

    contentsChanged();
}

//---------------------------------------------------------------------------
//  loadSearch
//
//! Load a search spec from a file.
//---------------------------------------------------------------------------
void
SearchSpecForm::loadSearch()
{
    QString filename = QFileDialog::getOpenFileName(this, "Load Search",
        Auxil::getSearchDir() + "/saved", "Zyzzyva Search Files (*.zzs)");

    if (filename.isEmpty())
        return;

    QFile file (filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString caption = "Error Opening Search File";
        QString message = "Cannot open file '" + filename + "': " +
            file.errorString();
        message = Auxil::dialogWordWrap(message);
        QMessageBox::warning(this, caption, message);
        return;
    }

    QString errorMsg;
    int errorLine = 0;
    int errorColumn = 0;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QDomDocument document;
    bool success = document.setContent(&file, false, &errorMsg, &errorLine,
                                       &errorColumn);
    QApplication::restoreOverrideCursor();

    if (!success) {
        QString caption = "Error in Search File";
        QString message = "Error in search file, line " +
            QString::number(errorLine) + ", column " +
            QString::number(errorColumn) + ": " + errorMsg;
        message = Auxil::dialogWordWrap(message);
        QMessageBox::warning(this, caption, message);
        return;
    }

    SearchSpec spec;
    if (!spec.fromDomElement(document.documentElement())) {
        QString caption = "Error in Search File";
        QString message = "Error in search file.";
        message = Auxil::dialogWordWrap(message);
        QMessageBox::warning(this, caption, message);
        return;
    }

    setSearchSpec(spec);
    contentsChanged();
}

//---------------------------------------------------------------------------
//  saveSearch
//
//! Save the current search spec to a file.
//---------------------------------------------------------------------------
void
SearchSpecForm::saveSearch()
{
    QString filename = QFileDialog::getSaveFileName(this, "Save Search",
        Auxil::getSearchDir() + "/saved", "Zyzzyva Search Files (*.zzs)");

    if (filename.isEmpty())
        return;

    bool filenameEdited = false;
    if (!filename.endsWith(".zzs", Qt::CaseInsensitive)) {
        filename += ".zzs";
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
        QString caption = "Error Saving Search";
        QString message = "Cannot save search:\n" + file.errorString() + ".";
        message = Auxil::dialogWordWrap(message);
        QMessageBox::warning(this, caption, message);
        return;
    }

    QDomImplementation implementation;
    QDomDocument document (implementation.createDocumentType("zyzzyva-search",
        QString(), "http://boshvark.com/dtd/zyzzyva-search.dtd"));

    document.appendChild(getSearchSpec().asDomElement());

    // XXX: There should be a programmatic way to write the <?xml?> header
    // based on the QDomImplementation, shouldn't there?
    QTextStream stream (&file);
    stream << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
        << document.toString();
}

//---------------------------------------------------------------------------
//  selectInputArea
//
//! Give focus to a text input area if possible.  Otherwise give focus to the
//! first input area in the form.
//---------------------------------------------------------------------------
void
SearchSpecForm::selectInputArea()
{
    // Look for an input area to select
    bool focusSet = false;
    QListIterator<SearchConditionForm*> it (conditionForms);
    while (!focusSet && it.hasNext()) {
        SearchConditionForm* form = it.next();
        SearchCondition condition = form->getSearchCondition();
        switch (condition.type) {
            case SearchCondition::PatternMatch:
            case SearchCondition::AnagramMatch:
            case SearchCondition::SubanagramMatch:
            case SearchCondition::Prefix:
            case SearchCondition::Suffix:
            case SearchCondition::IncludeLetters:
            case SearchCondition::ConsistOf:
            form->selectInputArea();
            focusSet = true;
            break;

            default: break;
        }
    }

    // If no text input area was found, give focus to the first input area
    if (!focusSet)
        conditionForms[0]->selectInputArea();
}
