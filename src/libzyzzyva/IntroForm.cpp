//---------------------------------------------------------------------------
// IntroForm.cpp
//
// A form for displaying introductory help material.
//
// Copyright 2015-2016 Twilight Century Computing.
// Copyright 2006-2012 North American SCRABBLE Players Association.
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

#include "IntroForm.h"
#include "Auxil.h"
#include <QDesktopServices>
#include <QFormLayout>
#include <QPushButton>
#include <QTextBrowser>

const QString TITLE_PREFIX = "Welcome";

//---------------------------------------------------------------------------
//  IntroForm
//
//! Constructor.
//
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
IntroForm::IntroForm(QWidget* parent, Qt::WindowFlags f)
    : ActionForm(IntroFormType, parent, f)
{
    QVBoxLayout* mainLay = new QVBoxLayout(this);
    Q_CHECK_PTR(mainLay);
    mainLay->setContentsMargins(11, 11, 0, 11);

// (JGM) QtWebView removed in Qt 5.6.0
//    view = new QtWebView(this);
//    Q_CHECK_PTR(view);
//    QString mainPage = Auxil::getHelpDir() + "/index.html";
//    view->load(QUrl::fromLocalFile(mainPage));
//    view->page()->setLinkDelegationPolicy(QWebPage::DelegateExternalLinks);
//    connect(view->page(), SIGNAL(linkClicked(const QUrl&)), SLOT(linkIsClicked(const QUrl&)));
//    mainLay->addWidget(view);

    QHBoxLayout* buttonsLay = new QHBoxLayout();
    Q_CHECK_PTR(buttonsLay);
    QPushButton *welcome = new QPushButton("Welcome");
    welcome->setStyleSheet("QPushButton {background-color: #FF69B4;}");
    Q_CHECK_PTR(welcome);
    QPushButton *toc = new QPushButton("Contents");
    toc->setStyleSheet("QPushButton {background-color: #87CEFA;}");
    Q_CHECK_PTR(toc);
    QPushButton *back = new QPushButton();
    Q_CHECK_PTR(back);
    back->setIcon(QIcon(":/left-arrow-icon"));
    QPushButton *forward = new QPushButton();
    Q_CHECK_PTR(forward);
    forward->setIcon(QIcon(":/right-arrow-icon"));
    buttonsLay->addWidget(welcome);
    buttonsLay->addWidget(toc);
    buttonsLay->addWidget(back);
    buttonsLay->addWidget(forward);
    buttonsLay->addStretch(1);
    mainLay->addLayout(buttonsLay);

    browser = new QTextBrowser(this);
    Q_CHECK_PTR(browser);
    mainPage = Auxil::getHelpDir() + "/index.html";
    tocPage = Auxil::getHelpDir() + "/toc.html";
    connect(welcome, SIGNAL(clicked()), this, SLOT(goWelcome()));
    connect(toc, SIGNAL(clicked()), this, SLOT(goTOC()));
    connect(forward, SIGNAL(clicked()), browser, SLOT(forward()));
    connect(back, SIGNAL(clicked()), browser, SLOT(backward()));
    connect(browser, SIGNAL(forwardAvailable(bool)), forward, SLOT(setEnabled(bool)));
    connect(browser, SIGNAL(backwardAvailable(bool)), back, SLOT(setEnabled(bool)));
    browser->setOpenExternalLinks(true);
    browser->setUndoRedoEnabled(true);
    browser->setSource(QUrl::fromLocalFile(mainPage));
    mainLay->addWidget(browser);
}

//---------------------------------------------------------------------------
//  goWelcome
//
//! Called when the Welcome navigation button is clicked.
//
//! @param  The URL of the Welcome page.
//---------------------------------------------------------------------------
void
IntroForm::goWelcome()
{
    browser->home();
}

//---------------------------------------------------------------------------
//  goTOC
//
//! Called when the Contents navigation button is clicked.
//
//! @param  The URL of the TOC page.
//---------------------------------------------------------------------------
void
IntroForm::goTOC()
{
    browser->setSource(QUrl::fromLocalFile(tocPage));
}

////---------------------------------------------------------------------------
////  linkIsClicked
////
////! Called when a delegated link is clicked.
////
////! @param  The URL of the link clicked.
////---------------------------------------------------------------------------
//void
//IntroForm::linkIsClicked(const QUrl& url)
//{
//    QDesktopServices::openUrl(url);
//}

//---------------------------------------------------------------------------
//  getIcon
//
//! Returns the current icon.
//
//! @return the current icon
//---------------------------------------------------------------------------
QIcon
IntroForm::getIcon() const
{
    return QIcon(":/help-icon");
}

//---------------------------------------------------------------------------
//  getTitle
//
//! Returns the current title string.
//
//! @return the current title string
//---------------------------------------------------------------------------
QString
IntroForm::getTitle() const
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
IntroForm::getStatusString() const
{
    return QString();
}
