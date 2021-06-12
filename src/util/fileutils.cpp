////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2021 Ripose
//
// This file is part of Memento.
//
// Memento is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2 of the License.
//
// Memento is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Memento.  If not, see <https://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////

#include "fileutils.h"

#include <QCryptographicHash>

QString FileUtils::calculateMd5(const QString &path)
{
    QFile file(path);
    return calculateMd5(&file);
}

QString FileUtils::calculateMd5(QFile *file)
{
    QString hash;
    if (file->open(QFile::ReadOnly))
    {
        QCryptographicHash hasher(QCryptographicHash::Md5);
        if (hasher.addData(file))
        {
            hash = hasher.result().toHex();
        }
        file->close();
    }
    return hash;
}
