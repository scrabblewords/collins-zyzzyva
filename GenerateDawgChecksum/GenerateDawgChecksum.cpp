//---------------------------------------------------------------------------
// GenerateDawgChecksum.cpp
// JG Miller
//
// A class for generating the checksums for dawg files.
//
// Copyright 2015 Twilight Century Computing, LLC.
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

#include <QByteArray>
#include <QCoreApplication>
#include <QFile>
#include <QObject>
#include <QString>
#include <QStringList>


static const QString dawgFileName = "F:\\jim\\Documents\\My Documents\\GAMES\\SCRABBLE\\CSW15\\Collins Zyzzyva development\\word files\\CSW19.dwg";

qint32 importDawgFile(const QString& filename)
{
    QFile file (filename);
    if (!file.open(QIODevice::ReadOnly)) {
        return 0;
    }

    qint32 checksum;
    qint32* dawg;
    qint32 numEdges;
    qint32* p = &numEdges;
    char* cp = (char*) p;
    file.read(cp, 1 * sizeof(qint32));

    dawg = new qint32[numEdges + 1];
    dawg[0] = 0;
    p = &dawg[1];
    cp = (char*) p;
    file.read(cp, numEdges * sizeof(qint32));

    checksum = qChecksum(cp, numEdges);

    return checksum;
}

int main(int argc, char *argv[])
{
    qint32 checksum;
    QCoreApplication a(argc, argv);

    checksum = importDawgFile(dawgFileName);
    printf("%d\n", checksum);

    return a.exec();
}
