//---------------------------------------------------------------------------
// WordTableView.cpp
//
// A class derived from QTableView, used to display word lists.
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

#include "WordTableView.h"
#include "CardboxAddDialog.h"
#include "CardboxRemoveDialog.h"
#include "DefinitionDialog.h"
#include "MainSettings.h"
#include "MainWindow.h"
#include "QuizDatabase.h"
#include "QuizSpec.h"
#include "SearchSpec.h"
#include "WordEngine.h"
#include "WordListSaveDialog.h"
#include "WordTableModel.h"
#include "WordVariationDialog.h"
#include "WordVariationType.h"
#include "Auxil.h"
#include <QAction>
#include <QApplication>
#include <QContextMenuEvent>
#include <QFileDialog>
#include <QHeaderView>
#include <QMessageBox>
#include <QMenu>
#include <QPrintDialog>
#include <QPrinter>
#include <QPushButton>
#include <QSignalMapper>
#include <QTextDocument>
#include <QTextStream>
#include <QToolTip>
#include <QDebug>
#include <QFont>

using namespace std;

//---------------------------------------------------------------------------
//  WordTableView
//
//! Constructor.
//
//! @param parent the parent object
//---------------------------------------------------------------------------
WordTableView::WordTableView(WordEngine* e, QWidget* parent)
    : QTreeView(parent), wordEngine(e)
{
    setFocusPolicy(Qt::NoFocus);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setRootIsDecorated(false);

    // TODO (JGM): Revisit this code in order to completely hide desired columns.
    // Existing view display problems:  Wildcard column in child windows only hiding/unhiding
    // on hover or other action; and inner hook symbol likewise not changing automatically.
    //
    // FIXME: Once Trolltech fixes the assert in QHeaderView, continue with
    // statements like these
    //header()->setSectionResizeMode(WordTableModel::FRONT_HOOK_COLUMN, QHeaderView::Interactive);
    //header()->setSectionResizeMode(WordTableModel::BACK_HOOK_COLUMN, QHeaderView::Interactive);
    header()->setSectionResizeMode(QHeaderView::Interactive);
    //header()->setMinimumSectionSize(0);
    //header()->setCascadingSectionResizes(true);
    //header()->setDefaultSectionSize(0);
    //header()->setStyleSheet("QHeaderView::down-arrow {image: none;}");
    //header()->setStyleSheet("QHeaderView::up-arrow {image: none;}");
//    if (!MainSettings::getWordListShowHooks()) {

//        hideColumn(WordTableModel::FRONT_HOOK_COLUMN);
//        columnCountChanged()
//        header()->resizeSection(WordTableModel::FRONT_HOOK_COLUMN, 0);
//        setColumnWidth(WordTableModel::FRONT_HOOK_COLUMN, 0);
//        header()->hideSection(WordTableModel::FRONT_HOOK_COLUMN);
//        //model()->removeColumn(WordTableModel::FRONT_HOOK_COLUMN);

//        hideColumn(WordTableModel::BACK_HOOK_COLUMN);
//        header()->resizeSection(WordTableModel::BACK_HOOK_COLUMN, 0);
//        setColumnWidth(WordTableModel::BACK_HOOK_COLUMN, 0);
//        header()->hideSection(WordTableModel::BACK_HOOK_COLUMN);
//        //header()->setStyleSheet("QHeaderView::section:hidden {background-color: transparent;}");
//    }

    header()->setSortIndicatorShown(true);
    header()->setSortIndicator(WordTableModel::WORD_COLUMN, Qt::AscendingOrder);
    //header()->setClickable(true);
    header()->setSectionsClickable(true);
    connect(header(), SIGNAL(sectionClicked(int)),
        SLOT(headerSectionClicked(int)));
}

//---------------------------------------------------------------------------
//  resizeItemsRecursively
//
//! Resize all columns to fit the model contents, for view and all views
//! of child WordVariationDialogs.
//!
//! TODO (JGM) Add an interface to combine this with the same function in
//! MainWindow::readSettings.
//---------------------------------------------------------------------------
void
WordTableView::resizeItemsRecursively()
{
    resizeItemsToContents();
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
//  resizeItemsToContents
//
//! Resize all columns to fit the model contents.
//---------------------------------------------------------------------------
void
WordTableView::resizeItemsToContents()
{
//    for (int i = 0; i < model()->rowCount(); ++i)
//        resizeRowToContents(i);

    // TODO (JGM): See WordTableView::WordTableView big TODO.
    //header()->setMinimumSectionSize(0);
    //header()->setCascadingSectionResizes(true);
    for (int i = 0; i < model()->columnCount(); ++i) {
//      if (isColumnHidden(i)) {
          //hideColumn(i);
//          header()->resizeSection(i, 0);
//          setColumnWidth(i, 0);
//          header()->hideSection(i);
          //setColumnWidth(i, 0);
          //header()->hideSection(i);
//      }
//      else {
    //model()->removeColumn(i);
    //header()->setDefaultSectionSize(0);
        //header()->resizeSection(i, 10);
        resizeColumnToContents(i);
      }
//  }
}

//---------------------------------------------------------------------------
//  viewDefinition
//
//! Display the definition of the currently selected word.
//---------------------------------------------------------------------------
void
WordTableView::viewDefinition()
{
    QModelIndex index = currentIndex();
    index = index.sibling(index.row(), WordTableModel::WORD_COLUMN);
    WordTableModel* wordModel = static_cast<WordTableModel*>(model());
    QString word = wordModel->data(index, Qt::EditRole).toString();
    QString lexicon = wordModel->getLexicon();
    DefinitionDialog* dialog = new DefinitionDialog(wordEngine, lexicon, word,
                                                    this);
    Q_CHECK_PTR(dialog);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

//---------------------------------------------------------------------------
//  viewVariation
//
//! Display a variation of the currently selected word.
//
//! @param variation the variation to display
//---------------------------------------------------------------------------
void
WordTableView::viewVariation(int variation)
{
    QModelIndex index = currentIndex();
    index = index.sibling(index.row(), WordTableModel::WORD_COLUMN);
    WordTableModel* wordModel = static_cast<WordTableModel*>(model());
    QString word = wordModel->data(index, Qt::EditRole).toString();
    QString lexicon = wordModel->getLexicon();
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
//  headerSectionClicked
//
//! Called when the user clicks a header section. Change the sort order of the
//! items in the model.
//
//! @param section the section index
//---------------------------------------------------------------------------
void
WordTableView::headerSectionClicked(int)
{
    WordTableModel* wordModel = dynamic_cast<WordTableModel*>(model());
    if (!wordModel)
        return;

    wordModel->reverse();
}

//---------------------------------------------------------------------------
//  exportRequested
//
//! Called when the user indicates that the word list should be exported.
//! Display a dialog to let the user choose where to export the list, then
//! actually do the export.
//---------------------------------------------------------------------------
void
WordTableView::exportRequested()
{
    if (model()->rowCount() == 0) {
        QString caption = "Cannot Save Word List";
        QString message = "Cannot save word list:\nNo words to save.";
        message = Auxil::dialogWordWrap(message);
        QMessageBox::warning(this, caption, message);
        return;
    }

    WordListSaveDialog* dialog = new WordListSaveDialog(this);
    Q_CHECK_PTR(dialog);
    int code = dialog->exec();
    if (code != QDialog::Accepted) {
        delete dialog;
        return;
    }

    QList<WordAttribute> attributes = dialog->getSelectedAttributes();
    WordListFormat format = dialog->getWordListFormat();
    delete dialog;

    if (attributes.empty())
        return;

    QString filename = QFileDialog::getSaveFileName(this, "Save Word List",
        Auxil::getUserWordsDir() + "/saved", "Text Files (*.txt)", 0,
        QFileDialog::DontConfirmOverwrite);

    if (filename.isEmpty())
        return;

    if (!filename.endsWith(".txt", Qt::CaseInsensitive))
        filename += ".txt";

    bool append = false;
    if (QFile::exists(filename)) {
        QFileInfo fileInfo (filename);
        QString caption = "File Exists";
        QString message = "An item named \"" + fileInfo.fileName() +
            "\" already exists in this location.  Do you want to replace "
            "it with the one you are saving, or append words to the "
            "existing file?";
        message = Auxil::dialogWordWrap(message);

        QMessageBox messageBox(QMessageBox::Warning, caption, message,
                               QMessageBox::NoButton, this);
        QPushButton* replaceButton =
            messageBox.addButton("Replace", QMessageBox::AcceptRole);
        QPushButton* appendButton =
            messageBox.addButton("Append", QMessageBox::AcceptRole);
        QPushButton* cancelButton =
            messageBox.addButton("Cancel", QMessageBox::RejectRole);
        messageBox.setIcon(QMessageBox::Warning);
        messageBox.setDefaultButton(cancelButton);

        int code = messageBox.exec();

        // XXX: For some reason, clicking the Replace button doesn't result in
        // dialog acceptance despite adding it with AcceptRole above, so test
        // directly for replace button clicked
        if ((code != QMessageBox::Accepted) &&
            (messageBox.clickedButton() != replaceButton))
            return;

        if (messageBox.clickedButton() == appendButton)
            append = true;
    }

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QString error;
    bool ok = exportFile(filename, format, attributes, &error, append);
    QApplication::restoreOverrideCursor();
    if (!ok) {
        QString caption = "Error Saving Word List";
        QString message = "Cannot save word list:\n" + error + ".";
        message = Auxil::dialogWordWrap(message);
        QMessageBox::warning(this, caption, message);
    }
}

//---------------------------------------------------------------------------
//  printRequested
//
//! Called when the user indicates that the word list should be printed.
//! Display a print dialog.
//---------------------------------------------------------------------------
void
WordTableView::printRequested()
{
    QString html;
    QString htmlAlignment;
    QString alignment;
    int wordType;
    bool shading = false;
    //bool ZWSPs;
    bool nbHyphens;
    bool isDefCol;
    qint8 padding = 0;
    QString printingFontStr;
    QFont font;
    QString fontFamily;
    int fontSize;
    bool fontBold;
    bool fontItalic;
    bool fontUnderline;
    bool fontOverline;
    QString tdStyle = QString();
    QString unformattedData, data;

    printingFontStr = MainSettings::getPrintingFont();
    if (!printingFontStr.isEmpty()) {
        font.fromString(printingFontStr);
        fontFamily = font.family();
        fontSize = font.pointSize();
        fontBold = font.bold();
        fontItalic = font.italic();
        fontUnderline = font.underline();
        fontOverline = font.overline();
        tdStyle += (QString("<style> td {font-family:") + fontFamily + ";font-size:" + QString("%1").arg(fontSize) + "pt");
        if (fontBold)
            tdStyle += (";font-weight:bold");
        else
            tdStyle += (";font-weight:normal");
        if (fontItalic)
            tdStyle += (";font-style:italic");
        else
            tdStyle += (";font-style:normal");
        if (fontUnderline)
            tdStyle += (";text-decoration:underline");
        else if (fontOverline)
            tdStyle += (";text-decoration:overline");
        else
            tdStyle += (";text-decoration:none");
        tdStyle += "} </style>";
    }

    html = "<html><body>" + tdStyle + "<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\">";
    for (int row = 0; row < model()->rowCount(); row++) {
        html += "<tr>";
        //html += "<tr valign=\"middle\">";
        for (int column = 0; column < model()->columnCount(); column++) {
            wordType = (model()->data(model()->index(row, column), WordTableModel::WordTypeRole)).toInt();
            switch (wordType) {
                case WordTableModel::WordNormal:
                    shading = false;
                break;
                case WordTableModel::WordNormalAlternate:
                    shading = true;
            }
            //ZWSPs = false;
            nbHyphens = false;
            isDefCol = false;
            alignment = (model()->data(model()->index(row, column), Qt::TextAlignmentRole)).toString();
            switch(alignment.toInt() - 128) {
                case Qt::AlignLeft:
                    htmlAlignment = "\"left\"";
                break;
                case Qt::AlignCenter:
                    htmlAlignment = "\"center\"";
                break;
                case Qt::AlignRight:
                    htmlAlignment = "\"right\"";
            }
            unformattedData = model()->data(model()->index(row, column), Qt::DisplayRole).toString();
            if (column == WordTableModel::FRONT_HOOK_COLUMN) {
                padding = 8;
                //ZWSPs = true;
                //nbHyphens = true;
            }
            else if (column == WordTableModel::WORD_COLUMN) {
                unformattedData.replace(' ', "&nbsp;");
                padding = 15;
                //nbHyphens = true;
            }
            else if (column == WordTableModel::BACK_HOOK_COLUMN) {
                padding = 25;
                //ZWSPs = true;
                nbHyphens = true;
            }
            else if (column == WordTableModel::DEFINITION_COLUMN) {
                padding = 0;
                nbHyphens = true;
                isDefCol = true;
            }
            else
                padding = 25;
            if (!unformattedData.isNull()) {
                data = (nbHyphens ? unformattedData.replace(QRegExp("-"), "&#8209;") : unformattedData);
                html += "<td align=" + htmlAlignment + " style=\""
                    // TODO (JGM):  Trying to get long lists of hook letters not to break prematurely after '+'
                    // character, for example (see PA in list of anagrams of "A?").  But using ZWSPs causes the hook cells
                    // to be placed slightly lower!  And soft hyphen leaves a hyphen character; and valign doesn't help.
                    // Is there anything in CSS to accomplish this?
                    // REFER TO:  http://doc.qt.io/qt-5/richtext-html-subset.html
                    //
                    //+ "px 0 0 px;\">" + (ZWSPs ? data.replace(QRegExp("([ -~])"), "\\1&#8203;") : data) + "</td>";
                    //+ (ZWSPs ? ("valign=\"top\">" + data.replace(QRegExp("([ -~])"), "\\1&#8203;")) : (">" + data)) + "</td>";
                    //+ (ZWSPs ? (">" + data.replace(QRegExp("([ -~])"), "\\1&shy;")) : (">" + data)) + "</td>";
                    + (shading ? "background-color:rgb(228, 229, 230);" : "")
                    + "padding:0px " + QString::number(padding) + "px 0px 0px;"
                    + (isDefCol ? "white-space:pre-wrap;padding-left:16px;text-indent:-16px" : "")
                    //+ (isDefCol ? "white-space:pre-wrap;" : "")
                    //+ "\""
                    //+ (shading ? " bgcolor=\"#E4E5E6\">" : ">")
                    //+ "\">" + (isDefCol ? data.replace(QRegExp("/"), "/\n") : data)
                    + "\">" + data
                    + "</td>";
                    //+ (nbHyphens ? data.replace(QRegExp("-"), "&#8209;") : data) + "</td>";
            } else
                html += QString("<td") + (shading ? " style=\"background-color:rgb(228, 229, 230);\"" : "") + "></td>";
        }
        html += "</tr>";
    }
    html += "</table></body></html>";

    QPrinter printer;
    //printer.setOutputFormat(QPrinter::NativeFormat);
    QPrintDialog *dialog = new QPrintDialog(&printer);
    if (dialog->exec() == QDialog::Accepted) {
        //QWebView document;
        QTextDocument document;
        //document.setDocumentMargin(6.0);
        //printer.setFullPage(true);
        document.setHtml(html);
        //document.setHtml(document.toPlainText());
        //printer.setOutputFileName("test.pdf");
        //printer.setOutputFormat(QPrinter::PdfFormat);
        //printer.setPageMargins(QMarginsF(10.0, 10.0, 10.0, 5.0));
        document.print(&printer);
    }
}

//---------------------------------------------------------------------------
//  createQuizRequested
//
//! Called when the user indicates that the word list should be used to create
//! a quiz.  Display a quiz dialog with a single In Word List condition
//! already filled in with the contents of the list.
//---------------------------------------------------------------------------
void
WordTableView::createQuizRequested()
{
    int numWords = model()->rowCount();
    if (numWords == 0) {
        QString caption = "Error Creating Quiz";
        QString message = "Cannot create quiz:\nNo words in the list.";
        message = Auxil::dialogWordWrap(message);
        QMessageBox::warning(this, caption, message);
        return;
    }

    QuizSpec quizSpec;
    SearchSpec searchSpec;
    SearchCondition searchCondition;
    QString searchString;
    QModelIndex index = model()->index(0, WordTableModel::WORD_COLUMN);
    for (int i = 0; i < numWords; ) {
        if (!searchString.isEmpty())
            searchString += " ";
        searchString += model()->data(index, Qt::EditRole).toString();
        index = index.sibling(++i, WordTableModel::WORD_COLUMN);
    }

    searchCondition.type = SearchCondition::InWordList;
    searchCondition.stringValue = searchString;
    searchSpec.conditions.push_back(searchCondition);
    WordTableModel* wordModel = static_cast<WordTableModel*>(model());
    QString lexicon = wordModel->getLexicon();
    if (lexicon.isEmpty())
        lexicon = MainSettings::getDefaultLexicon();
    quizSpec.setLexicon(lexicon);
    quizSpec.setSearchSpec(searchSpec);
    MainWindow::getInstance()->newQuizFormInteractive(quizSpec);
}

//---------------------------------------------------------------------------
//  addToCardboxRequested
//
//! Called when the user indicates that the word list should be added to the
//! cardbox system.
//---------------------------------------------------------------------------
void
WordTableView::addToCardboxRequested()
{
    int numWords = model()->rowCount();
    if (numWords == 0) {
        QString caption = "Error Adding Words to Cardbox";
        QString message = "Cannot add words to cardbox:\n"
            "No words in the list.";
        message = Auxil::dialogWordWrap(message);
        QMessageBox::warning(this, caption, message);
        return;
    }

    CardboxAddDialog* dialog = new CardboxAddDialog(this);
    Q_CHECK_PTR(dialog);

    QStringList words;
    QModelIndex index = model()->index(0, WordTableModel::WORD_COLUMN);
    for (int i = 0; i < numWords; ) {
        words.append(model()->data(index, Qt::EditRole).toString());
        index = index.sibling(++i, WordTableModel::WORD_COLUMN);
    }

    dialog->setWords(words);

    int code = dialog->exec();
    if (code == QDialog::Accepted) {
        WordTableModel* wordModel = static_cast<WordTableModel*>(model());
        QString lexicon = wordModel->getLexicon();
        QStringList words = dialog->getWords();
        QString quizType = dialog->getQuizType();
        bool estimateCardbox = dialog->getEstimateCardbox();
        int cardbox = dialog->getSpecifyCardbox();

        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        bool ok = addToCardbox(words, lexicon, quizType, estimateCardbox,
                               cardbox);
        QApplication::restoreOverrideCursor();

        if (!ok) {
            QString caption = "Error Adding Words to Cardbox";
            QString message = "An error occurred while adding words to the "
                "cardbox system.";
            message = Auxil::dialogWordWrap(message);
            QMessageBox::warning(this, caption, message);
        }
    }

    delete dialog;
}

//---------------------------------------------------------------------------
//  removeFromCardboxRequested
//
//! Called when the user indicates that the word list should be added to the
//! cardbox system.
//---------------------------------------------------------------------------
void
WordTableView::removeFromCardboxRequested()
{
    int numWords = model()->rowCount();
    if (numWords == 0) {
        QString caption = "Error Removing Words from Cardbox";
        QString message = "Cannot remove words from cardbox:\n"
            "No words in the list.";
        message = Auxil::dialogWordWrap(message);
        QMessageBox::warning(this, caption, message);
        return;
    }

    CardboxRemoveDialog* dialog = new CardboxRemoveDialog(this);
    Q_CHECK_PTR(dialog);

    QStringList words;
    QModelIndex index = model()->index(0, WordTableModel::WORD_COLUMN);
    for (int i = 0; i < numWords; ) {
        words.append(model()->data(index, Qt::EditRole).toString());
        index = index.sibling(++i, WordTableModel::WORD_COLUMN);
    }

    dialog->setWords(words);

    int code = dialog->exec();
    if (code == QDialog::Accepted) {
        WordTableModel* wordModel = static_cast<WordTableModel*>(model());
        QString lexicon = wordModel->getLexicon();
        QStringList words = dialog->getWords();
        QString quizType = dialog->getQuizType();

        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        bool ok = removeFromCardbox(words, lexicon, quizType);
        QApplication::restoreOverrideCursor();

        if (!ok) {
            QString caption = "Error Removing Words from Cardbox";
            QString message = "An error occurred while removing words "
                "from the cardbox system.";
            message = Auxil::dialogWordWrap(message);
            QMessageBox::warning(this, caption, message);
        }
    }

    delete dialog;
}

//---------------------------------------------------------------------------
//  clearDialogFromList
//
//! Remove pointer to this dialog from the list of child WordVariationDialogs.
//! TODO (JGM) Add an interface to combine this with the same function in
//! MainWindow.
//
//! @param obj pointer to the child WVD object.
//---------------------------------------------------------------------------
void
WordTableView::clearDialogFromList(QObject* obj)
{
    WordVariationDialog *wvd = static_cast<WordVariationDialog*>(obj);
    wordVariationDialogs.removeOne(wvd);
}

//---------------------------------------------------------------------------
//  exportFile
//
//! Export the words in the list to a file, one word per line.
//
//! @param filename the name of the file
//! @param format the save file format
//! @param attributes a list of attributes to export
//! @param err return error string on failure
//
//! @return true if successful or if the user cancels, false otherwise
//---------------------------------------------------------------------------
bool
WordTableView::exportFile(const QString& filename, WordListFormat format,
                          const QList<WordAttribute>& attributes,
                          QString* err, bool append) const
{
    if (model()->rowCount() == 0) {
        if (err)
            *err = "No words to save";
        return false;
    }

    QFile file (filename);
    QIODevice::OpenMode mode = QIODevice::WriteOnly | QIODevice::Text;
    if (append)
        mode |= QIODevice::Append;

    if (!file.open(mode)) {
        if (err)
            *err = file.errorString();
        return false;
    }

    QTextStream stream (&file);

    if (format == WordListOnePerLine) {
        QModelIndex index = model()->index(0, WordTableModel::WORD_COLUMN);
        for (int i = 0; i < model()->rowCount(); ++i) {
            index = index.sibling(i, WordTableModel::WORD_COLUMN);
            QStringList strings = getExportStrings(index, attributes);
            stream << strings.join("\t");
            endl(stream);
        }
    }

    else if ((format == WordListAnagramQuestionAnswer) ||
             (format == WordListAnagramTwoColumn) ||
             (format == WordListDistinctAlphagrams))
    {
        // Build map of alphagrams to indexes
        QMap<QString, QList<int> > alphaIndexes;
        QModelIndex index = model()->index(0, WordTableModel::WORD_COLUMN);
        for (int i = 0; i < model()->rowCount(); ++i) {
            index = index.sibling(i, WordTableModel::WORD_COLUMN);
            QString word = model()->data(index, Qt::EditRole).toString();
            QString alphagram = Auxil::getAlphagram(word);
            alphaIndexes[alphagram].append(i);
        }

        bool twoColumns = (format == WordListAnagramTwoColumn);

        // Iterate over the list, finding out the maximum column width
        // of each of the attribute fields. (Trading a bit of time for space,
        // we call getExportStrings twice per list, rather than store the
        // entire list of lists of strings)

        // Call getExportStrings to test the returned number of columns
        // (fields != attributes.length() since some attributes like inner-hooks
        // don't add a field)
        index = model()->index(0, WordTableModel::WORD_COLUMN);
        index = index.sibling(0, WordTableModel::WORD_COLUMN);
        int fields = getExportStrings(index, attributes).count();
        QVector<int> columnWidths(fields, 0);
        int anagramWidth = 0;

        if (twoColumns) {
            QMapIterator<QString, QList<int> > it (alphaIndexes);
            while (it.hasNext()) {
                it.next();
                anagramWidth = max(anagramWidth, it.key().length());
                QListIterator<int> jt (it.value());
                while (jt.hasNext()) {
                    int row = jt.next();
                    index = index.sibling(row, WordTableModel::WORD_COLUMN);
                    QStringList strings = getExportStrings(index, attributes);
                    for (int i = 0; i < fields; ++i) {
                        columnWidths[i] = max(columnWidths[i], strings[i].length());
                    }
                }
            }

            for (int i = 0; i < fields; ++i) {
                ++columnWidths[i];
            }

            // pad the anagram column a bit more to set it off
            anagramWidth += TWO_COLUMN_ANAGRAM_PADDING;
        }

        QString anagramPadding = QString(anagramWidth, ' ');
        QMapIterator<QString, QList<int> > it (alphaIndexes);
        while (it.hasNext()) {
            it.next();
            if (twoColumns) {
                stream << it.key().leftJustified(anagramWidth, ' ');
            }
            else if (format == WordListDistinctAlphagrams) {
                stream << it.key();
                endl(stream);
                continue;
            }
            else {
                stream << "Q: " << it.key();
                endl(stream);
            }

            bool firstAnagram = true;
            QListIterator<int> jt (it.value());
            while (jt.hasNext()) {
                int row = jt.next();
                index = index.sibling(row, WordTableModel::WORD_COLUMN);
                QStringList strings = getExportStrings(index, attributes);
                if (twoColumns) {
                    if (!firstAnagram) {
                        stream << anagramPadding;
                    }
                    firstAnagram = false;
                    for (int i = 0; i < strings.count(); ++i) {
                        stream << strings[i].leftJustified(columnWidths[i], ' ');
                    }
                }
                else {
                    stream << "A: " << strings.join(" ");
                }
                endl(stream);
            }

            if (!twoColumns) {
                endl(stream);
            }
        }
    }

    return true;
}

//---------------------------------------------------------------------------
//  getExportStrings
//
//! Get a list of strings to be exported corresponding to a model index and a
//! list of word attributes.
//
//! @param index the index to use
//! @param attributes the list of word attributes to export
//
//! @return an ordered list of strings to be exported
//---------------------------------------------------------------------------
QStringList
WordTableView::getExportStrings(QModelIndex& index, const
                                QList<WordAttribute>& attributes) const
{
    QListIterator<WordAttribute> attrIt (attributes);
    QStringList strings;

    bool exportInnerHooks = attributes.contains(WordAttrInnerHooks);
    bool exportSymbols = attributes.contains(WordAttrLexiconSymbols);

    while (attrIt.hasNext()) {
        WordAttribute attribute = attrIt.next();
        int column = -1;
        switch (attribute) {
            case WordAttrWord:
            case WordAttrAlphagram:
            column = WordTableModel::WORD_COLUMN;
            break;

            case WordAttrDefinition:
            column = WordTableModel::DEFINITION_COLUMN;
            break;

            case WordAttrFrontHooks:
            column = WordTableModel::FRONT_HOOK_COLUMN;
            break;

            case WordAttrBackHooks:
            column = WordTableModel::BACK_HOOK_COLUMN;
            break;

            case WordAttrProbabilityOrder:
            column = WordTableModel::PROBABILITY_ORDER_COLUMN;
            break;

            case WordAttrPlayabilityOrder:
            column = WordTableModel::PLAYABILITY_ORDER_COLUMN;
            break;

            default: break;
        }

        if (column < 0)
            continue;

        index = index.sibling(index.row(), column);
        QString str;

        // XXX: inner hook and symbols should be stored under separate roles
        // in the WordTableModel, so they can be easily queried

        // special processing for inner hooks and symbols
        if ((column == WordTableModel::WORD_COLUMN) &&
            (exportInnerHooks || exportSymbols) && (attribute != WordAttrAlphagram))
        {
            str = model()->data(index, Qt::DisplayRole).toString().toUpper();
            if (!exportSymbols) {
                QString ihChar = (MainSettings::getWordListShowHookParents() ?
                                  QString(".") : QString());
                QRegExp regex(QString("([^\\W_\\d]+%1).*").arg(ihChar));
                str.replace(regex, "\\1");
            }
            else if (!exportInnerHooks &&
                     MainSettings::getWordListShowHookParents())
            {
                str.replace(QRegExp(".([^\\W_\\d]+)."), "\\1");
            }
        }
        else if (!exportSymbols &&
                 ((column == WordTableModel::FRONT_HOOK_COLUMN) ||
                  (column == WordTableModel::BACK_HOOK_COLUMN)))
        {
            str = model()->data(index, Qt::DisplayRole).toString();
            str.replace(QRegExp("[\\W_\\d]+"), QString());
        }
        else if (column == WordTableModel::DEFINITION_COLUMN) {
            str = model()->data(index, Qt::EditRole).toString();
            str.replace("\n", " / ");
        }
        else {
            str = model()->data(index, Qt::EditRole).toString();
            if (attribute == WordAttrAlphagram)
                str = Auxil::getAlphagram(str);
        }
        strings.append(str);
    }

    return strings;
}

//---------------------------------------------------------------------------
//  addToCardbox
//
//! Add a list of words to the cardbox system.
//
//! @param words the words to add
//! @param lexicon the lexicon to use
//! @param quizType the quiz type to use
//! @param estimateCardbox whether to estimate cardboxes based on past
//! performance
//
//! @return true if successful, false otherwise
//---------------------------------------------------------------------------
bool
WordTableView::addToCardbox(const QStringList& words, const QString& lexicon,
    const QString& quizType, bool estimateCardbox, int cardbox) const
{
    QuizSpec::QuizType type = Auxil::stringToQuizType(quizType);
    if (type == QuizSpec::UnknownQuizType)
        return false;

    QStringList questions;

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

    QuizDatabase db (lexicon, quizType);
    if (!db.isValid())
        return false;

    db.addToCardbox(questions, estimateCardbox, cardbox);
    return true;
}

//---------------------------------------------------------------------------
//  removeFromCardbox
//
//! Remove a list of words from the cardbox system.
//
//! @param words the words to remove
//! @param lexicon the lexicon to use
//! @param quizType the quiz type to use
//
//! @return true if successful, false otherwise
//---------------------------------------------------------------------------
bool
WordTableView::removeFromCardbox(const QStringList& words, const QString&
                                 lexicon, const QString& quizType) const
{
    QuizSpec::QuizType type = Auxil::stringToQuizType(quizType);
    if (type == QuizSpec::UnknownQuizType)
        return false;

    QStringList questions;

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

    QuizDatabase db (lexicon, quizType);
    if (!db.isValid())
        return false;

    db.removeFromCardbox(questions);
    return true;
}

//---------------------------------------------------------------------------
//  hookToolTipText
//
//! This event handler, for event e, can be reimplemented in a subclass to
//! receive widget context menu events.
//
//! @param word the word whose hooks to display
//! @param hooks the hooks to display
//! @param front true if the hooks are fronthooks, false if back hooks
//
//! @return the tool tip text
//---------------------------------------------------------------------------
QString
WordTableView::hookToolTipText(const QString& word, const QString& hooks,
                               bool front) const
{
    WordTableModel* wordModel = static_cast<WordTableModel*>(model());
    QString lexicon = wordModel->getLexicon();

    QString text;
    int offset = 0;
    QRegExp regex("([^\\W_\\d])([\\W_\\d]*)?");
    while ((offset = regex.indexIn(hooks, offset)) >= 0) {
        QString hook = regex.cap(1);
        QString symbols = regex.cap(2);
        QString hookWord = (front ? (hook.toUpper() + word)
                                  : (word + hook.toUpper()));

        if (!text.isEmpty())
            text += "\n\n";

        text += hookWord + symbols + " : " +
            wordEngine->getDefinition(lexicon, hookWord);

        offset += regex.cap(0).length();
    }

    return text;
}

//---------------------------------------------------------------------------
//  contextMenuEvent
//
//! This event handler, for event e, can be reimplemented in a subclass to
//! receive widget context menu events.
//
//! @param e the context menu event
//
//! @return true if successful or if the user cancels, false otherwise
//---------------------------------------------------------------------------
void
WordTableView::contextMenuEvent(QContextMenuEvent* e)
{
    QModelIndex index = indexAt(e->pos());
    bool wordOptions = (index.isValid() &&
                        (index.row() >= 0) && (index.column() >= 0));

    QMenu* popupMenu = new QMenu;
    Q_CHECK_PTR(popupMenu);

    if (wordOptions) {
        QMenu* wordMenu = new QMenu("Word");
        Q_CHECK_PTR(wordMenu);
        popupMenu->addMenu(wordMenu);

        QSignalMapper* wordMapper = new QSignalMapper(wordMenu);
        Q_CHECK_PTR(wordMapper);

        // Word Definition
        QAction* definitionAction = new QAction("Definition", wordMenu);
        Q_CHECK_PTR(definitionAction);
        connect(definitionAction, SIGNAL(triggered()), SLOT(viewDefinition()));
        wordMenu->addAction(definitionAction);

        // Word Anagrams
        QAction* anagramsAction = new QAction("Anagrams", wordMenu);
        Q_CHECK_PTR(anagramsAction);
        connect(anagramsAction, SIGNAL(triggered()), wordMapper, SLOT(map()));
        wordMapper->setMapping(anagramsAction, VariationAnagrams);
        wordMenu->addAction(anagramsAction);

        // Word Subanagrams
        QAction* subanagramsAction = new QAction("Subanagrams", wordMenu);
        Q_CHECK_PTR(subanagramsAction);
        connect(subanagramsAction, SIGNAL(triggered()),
                wordMapper, SLOT(map()));
        wordMapper->setMapping(subanagramsAction, VariationSubanagrams);
        wordMenu->addAction(subanagramsAction);

        // Word Hooks
        QAction* hooksAction = new QAction("Hooks", wordMenu);
        Q_CHECK_PTR(hooksAction);
        connect(hooksAction, SIGNAL(triggered()), wordMapper, SLOT(map()));
        wordMapper->setMapping(hooksAction, VariationHooks);
        wordMenu->addAction(hooksAction);

        // Word Extensions
        QAction* extensionsAction = new QAction("Extensions", wordMenu);
        Q_CHECK_PTR(extensionsAction);
        connect(extensionsAction, SIGNAL(triggered()), wordMapper, SLOT(map()));
        wordMapper->setMapping(extensionsAction, VariationExtensions);
        wordMenu->addAction(extensionsAction);

        // Word Anagram Hooks
        QAction* anagramHooksAction = new QAction("Anagram Hooks", wordMenu);
        Q_CHECK_PTR(anagramHooksAction);
        connect(anagramHooksAction, SIGNAL(triggered()),
                wordMapper, SLOT(map()));
        wordMapper->setMapping(anagramHooksAction, VariationAnagramHooks);
        wordMenu->addAction(anagramHooksAction);

        // Word Blank Anagrams
        QAction* blankAnagramsAction = new QAction("Blank Anagrams", wordMenu);
        Q_CHECK_PTR(blankAnagramsAction);
        connect(blankAnagramsAction, SIGNAL(triggered()),
                wordMapper, SLOT(map()));
        wordMapper->setMapping(blankAnagramsAction, VariationBlankAnagrams);
        wordMenu->addAction(blankAnagramsAction);

        // Word Blank Matches
        QAction* blankMatchesAction = new QAction("Blank Matches", wordMenu);
        Q_CHECK_PTR(blankMatchesAction);
        connect(blankMatchesAction, SIGNAL(triggered()),
                wordMapper, SLOT(map()));
        wordMapper->setMapping(blankMatchesAction, VariationBlankMatches);
        wordMenu->addAction(blankMatchesAction);

        // Word Transpositions
        QAction* transpositionsAction = new QAction("Transpositions", wordMenu);
        Q_CHECK_PTR(transpositionsAction);
        connect(transpositionsAction, SIGNAL(triggered()),
                wordMapper, SLOT(map()));
        wordMapper->setMapping(transpositionsAction, VariationTranspositions);
        wordMenu->addAction(transpositionsAction);

        // Connect Word signal mappings to viewVariation
        connect(wordMapper, SIGNAL(mapped(int)), SLOT(viewVariation(int)));
    }

    QAction* exportAction = new QAction("Save list...", popupMenu);
    Q_CHECK_PTR(exportAction);
    connect(exportAction, SIGNAL(triggered()), SLOT(exportRequested()));
    popupMenu->addAction(exportAction);

    QAction* createQuizAction = new QAction("Quiz from list...", popupMenu);
    Q_CHECK_PTR(createQuizAction);
    connect(createQuizAction, SIGNAL(triggered()), SLOT(createQuizRequested()));
    popupMenu->addAction(createQuizAction);

    QAction* addToCardboxAction =
        new QAction("Add list to Cardbox...", popupMenu);
    Q_CHECK_PTR(addToCardboxAction);
    connect(addToCardboxAction, SIGNAL(triggered()),
             SLOT(addToCardboxRequested()));
    popupMenu->addAction(addToCardboxAction);

    QAction* removeFromCardboxAction =
        new QAction("Remove list from Cardbox...", popupMenu);
    Q_CHECK_PTR(removeFromCardboxAction);
    connect(removeFromCardboxAction, SIGNAL(triggered()),
            SLOT(removeFromCardboxRequested()));
    popupMenu->addAction(removeFromCardboxAction);

    if (model()->rowCount() > 0) {
        QAction* printAction = new QAction("Print list...", popupMenu);
        Q_CHECK_PTR(printAction);
        connect(printAction, SIGNAL(triggered()), SLOT(printRequested()));
        popupMenu->addAction(printAction);
    }

    popupMenu->exec(QCursor::pos());
    delete popupMenu;
}

//---------------------------------------------------------------------------
//  viewportEvent
//
//! This event handler, for event e, can be reimplemented in a subclass to
//! receive viewport events.
//
//! @param e the viewport event
//
//! @return true if successful, false otherwise
//---------------------------------------------------------------------------
bool
WordTableView::viewportEvent(QEvent* e)
{
    switch (e->type()) {
        case QEvent::ToolTip: {
            QHelpEvent* helpEvent = static_cast<QHelpEvent*>(e);
            QModelIndex index = indexAt(helpEvent->pos());
            if (!index.isValid())
                break;

            QString toolTipText;
            switch (index.column()) {
                case WordTableModel::DEFINITION_COLUMN:
                toolTipText = index.model()->data(index,
                                                  Qt::DisplayRole).toString();
                break;

                case WordTableModel::FRONT_HOOK_COLUMN:
                case WordTableModel::BACK_HOOK_COLUMN: {
                    QString hooks =
                        index.model()->data(index, Qt::EditRole).toString();
                    QString word = index.model()->data(
                        index.sibling(index.row(), WordTableModel::WORD_COLUMN),
                        Qt::EditRole).toString();
                    bool front = (index.column() ==
                                  WordTableModel::FRONT_HOOK_COLUMN);

                    toolTipText = hookToolTipText(word, hooks, front);
                }
                break;

                default:
                break;
            }

            if (!toolTipText.isEmpty())
                QToolTip::showText(helpEvent->globalPos(), toolTipText);
        }
        return true;

        default:
        break;
    }

    return QTreeView::viewportEvent(e);
}

//---------------------------------------------------------------------------
//  sizeHintForColumn
//
//! Return the width size hint for a column.  Directly call the equivalent
//! function from QAbstractItemView instead of allowing the QTableView version
//! to take precedence, because we want the content of all rows to be
//! considered, not just the visible ones.
//
//! @param column the column index
//! @return the size hint for the column
//---------------------------------------------------------------------------
int
WordTableView::sizeHintForColumn(int column) const
{
    return QAbstractItemView::sizeHintForColumn(column) +
        (2 * ITEM_XPADDING);
}

//---------------------------------------------------------------------------
//  sizeHintForRow
//
//! Return the height size hint for a row.  Directly call the equivalent
//! function from QAbstractItemView instead of allowing the QTableView version
//! to take precedence, because we want the content of all columns to be
//! considered, not just the visible ones.
//
//! @param column the column index
//! @return the size hint for the column
//---------------------------------------------------------------------------
int
WordTableView::sizeHintForRow(int row) const
{
    return QAbstractItemView::sizeHintForRow(row) +
        (2 * ITEM_YPADDING);
}
