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
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QFontDatabase>

#include "gui/mainwindow.h"
#include "util/directoryutils.h"

int main(int argc, char *argv[])
{
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        QCoreApplication::addLibraryPath(DirectoryUtils::getProgramDirectory());
    #endif
    QGuiApplication::setWindowIcon(QIcon(":memento.svg"));
    /* HiDPI support */
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    /* Fonts */
    //QFontDatabase::addApplicationFont(":/fonts/stroke_order/KanjiStrokeOrders.ttf");
    //QFontDatabase::addApplicationFont(":/fonts/noto_sans/NotoSansCJKjp-Regular.otf");
    //QFontDatabase::addApplicationFont(":/fonts/noto_sans/NotoSansCJKjp-Bold.otf");

    /* Create the configuration directory if it doesn't exist */
    if (!QDir(DirectoryUtils::getConfigDir()).exists())
    {
        if (!QDir().mkdir(DirectoryUtils::getConfigDir()))
        {
            qDebug() << "Could not make config dir at"
                     << DirectoryUtils::getConfigDir();
            return EXIT_FAILURE;
        }

        if (!QDir().mkdir(DirectoryUtils::getDictionaryDir()))
        {
            qDebug() << "Could not make dictionary dir at"
                     << DirectoryUtils::getDictionaryDir();
            return EXIT_FAILURE;
        }
    }
    
    QApplication memento(argc, argv);
    setlocale(LC_NUMERIC, "C");
    MainWindow main_window;
    main_window.show();
    return memento.exec();
}
