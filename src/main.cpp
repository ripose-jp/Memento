////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2020 Ripose
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

#include <QApplication>
#include <QFile>
#include <QDebug>

#include "gui/mainwindow.h"
#include "util/directoryutils.h"

int main(int argc, char *argv[])
{
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        QCoreApplication::addLibraryPath(DirectoryUtils::getProgramDirectory());
    #endif
    QGuiApplication::setWindowIcon(QIcon(":memento.svg"));
    // HiDPI support
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    
    QFile db(DirectoryUtils::getDictionaryDir() + JMDICT_DB_NAME);
    if (!db.exists())
    {
        qDebug() << "Could not find JMDict in" 
                 << DirectoryUtils::getDictionaryDir();
        if (!db.open(QIODevice::WriteOnly))
        {
            qDebug() << "Could not create placeholder file in JMDict directory";
            return EXIT_FAILURE;
        }
        qDebug() << "Created a placeholder file. Please download the latest "
                    "JMDict file and update it from within the Memento.";
        db.close();
    }

    QApplication memento(argc, argv);
    setlocale(LC_NUMERIC, "C");
    MainWindow main_window;
    main_window.show();
    return memento.exec();
}
