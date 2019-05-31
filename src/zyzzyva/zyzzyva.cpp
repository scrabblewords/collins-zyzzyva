//---------------------------------------------------------------------------
// zyzzyva.cpp
//
// The main Zyzzyva program.
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

#include "MainSettings.h"
#include "MainWindow.h"
#include "ZApplication.h"
#include <QDesktopWidget>
#include <QFile>
#include <QObject>
#include <QPixmap>
#include <QSplashScreen>
#include <QDebug>
#include <QThread>

const QString SETTINGS_ORGANIZATION_NAME = "Twilight Century Computing";
const QString SETTINGS_DOMAIN_NAME = "twilightcenturycomputing.com";
const QString SETTINGS_APPLICATION_NAME = "Collins Zyzzyva";

int main(int argc, char** argv)
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    // JGM Testing
    //app.setStyleSheet("* {background-color: red}");
    QCoreApplication::setOrganizationName(SETTINGS_ORGANIZATION_NAME);
    QCoreApplication::setOrganizationDomain(SETTINGS_DOMAIN_NAME);
    QCoreApplication::setApplicationName(SETTINGS_APPLICATION_NAME);
    ZApplication app(argc, argv);

    QPixmap pixmap (":/zyzzyva-splash");
    QSplashScreen* splash = new QSplashScreen(pixmap);
    splash->show();

    MainWindow* window = new MainWindow(0, splash);
    QThread::sleep(2);

    window->tryAutoImport();
    window->tryConnectToDatabases();

#if defined(Q_OS_WIN)
    QRect srect = qApp->desktop()->availableGeometry();
    if ((MainSettings::getMainWindowPos().x() < 0) || (MainSettings::getMainWindowPos().y() < 0) ||
        (MainSettings::getMainWindowPos().x() > srect.width()) ||
        (MainSettings::getMainWindowPos().y() > srect.height()) ||
        (MainSettings::getMainWindowSize().width() < 0) || (MainSettings::getMainWindowSize().height() < 0) ||
        (MainSettings::getMainWindowSize().width() > srect.width()) ||
        (MainSettings::getMainWindowSize().height() > srect.height()))
        window->setWindowState(Qt::WindowMaximized);
#endif

    splash->finish(window);
    delete splash;
    window->show();

    // Now that the splash screen is gone, process any database errors
    window->processDatabaseErrors();

    // Handle command-line arguments
    if (argc > 1) {
        QStringList args;
        for (int i = 1; i < argc; ++i) {
            args.append(QString(argv[i]));
        }
        window->processArguments(args);
    }

#ifndef Z_LINUX
    // Handle file open requests
    QStringList files = app.getFileOpenRequests();
    foreach (const QString& file, files) {
        window->fileOpenRequested(file);
    }
    app.clearFileOpenRequests();

    QObject::connect(&app, SIGNAL(fileOpenRequested(const QString&)),
                     window, SLOT(fileOpenRequested(const QString&)));
#endif

    return app.exec();
}

