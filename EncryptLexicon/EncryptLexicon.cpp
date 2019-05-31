//---------------------------------------------------------------------------
// EncryptLexicon.cpp
// Jim Miller (JGM)
//
// A short utility for putting light encryption on a lexicon file.
//
// Usage:
// Set plaintext file at Qt Creator/Projects/Run/Run/Command line arguments
// Adjust global constants below as needed.
// Run from Qt Creator.
//
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


#include "../src/simplecrypt/simplecrypt.h"
#include <QByteArray>
#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QObject>
#include <QString>
#include <QStringList>


static const QString plaintextFileName = "F:\\Programming_projects\\boshvark-zyzzyva-pc-2.2.3-0-g9ddd4ac\\boshvark-zyzzyva-pc-9ddd4ac\\PRIVATE\\CSW19-Playability.txt";
// Set to empty string to have no header written.  A header must start with '#' character.
static const QString cipherFileHeader = "# The (C) HarperCollins 2019 word list (CSW19) is copyright of HarperCollins 2019 and used with permission.\r\n";
static const QString cipherFileSuffix = ".bin";


qint32 encryptLexiconFile(const QString& filename)
{
    // Get cryptographic hash.
    qulonglong cryptHash =
    {
#include "../PRIVATE/crypthash"
    };

    SimpleCrypt crypto(cryptHash);
    crypto.setCompressionMode(SimpleCrypt::CompressionAlways);
    crypto.setIntegrityProtectionMode(SimpleCrypt::ProtectionHash);

    QFile plaintextFile(filename);
    if (!plaintextFile.open(QIODevice::ReadOnly)) {
        return 1;
    }
    QByteArray *fileBlob = new QByteArray(plaintextFile.readAll());
    plaintextFile.close();

    printf("Encrypting file:  %s ...\n", filename.toUtf8().data());
    QByteArray *ciphertextData = new QByteArray(crypto.encryptToByteArray(*fileBlob));
    delete fileBlob;
//    if (!crypto.lastError() == SimpleCrypt::ErrorNoError) {
//        // error code
//        delete ciphertextData;
//        return;
//    }

    QFileInfo plaintextFileInfo(plaintextFileName);

    QFile ciphertextFile(plaintextFileInfo.completeBaseName() + cipherFileSuffix);
    if (!ciphertextFile.open(QIODevice::WriteOnly)) {
        delete ciphertextData;
        return 1;
    }

    if (!cipherFileHeader.isEmpty())
        ciphertextFile.write(cipherFileHeader.toUtf8().data());
    ciphertextFile.write(*ciphertextData);

    delete ciphertextData;
    ciphertextFile.close();

    printf("\nFinished!  Encrypted file written to:  %s\n", QFileInfo(ciphertextFile).absoluteFilePath().toUtf8().data());

    return 0;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    encryptLexiconFile(plaintextFileName);

    return a.exec();
}
