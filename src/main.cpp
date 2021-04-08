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
#include <QMessageBox>
#include <QFontDatabase>

#include "gui/mainwindow.h"
#include "util/directoryutils.h"
#include "util/globalmediator.h"
#include "util/iconfactory.h"

int main(int argc, char *argv[])
{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    /* Image Formats Windows */
    QCoreApplication::addLibraryPath(DirectoryUtils::getProgramDirectory());
#endif
    QGuiApplication::setWindowIcon(QIcon(":memento.svg"));
    /* HiDPI support */
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    /* Originization Info */
    QCoreApplication::setOrganizationName("memento");
    QCoreApplication::setOrganizationDomain("ripose.projects");
    QCoreApplication::setApplicationName("memento");

    /* Construct the application */
    QApplication memento(argc, argv);

    /* Create the configuration directory if it doesn't exist */
    if (!QDir(DirectoryUtils::getConfigDir()).exists())
    {
        if (!QDir().mkdir(DirectoryUtils::getConfigDir()))
        {
            QMessageBox message;
            message.critical(
                0, "Error Creating Config Directory",
                "Could not make configuration directory at " + DirectoryUtils::getConfigDir()
            );
            return EXIT_FAILURE;
        }

        if (!QDir().mkdir(DirectoryUtils::getDictionaryDir()))
        {
            QMessageBox message;
            message.critical(
                0, "Error Creating Dict Directory",
                "Could not make dictionary directory at " + DirectoryUtils::getConfigDir()
            );
            return EXIT_FAILURE;
        }
    }
    
    setlocale(LC_NUMERIC, "C");
    MainWindow *main_window = new MainWindow;
    main_window->show();
    int ret = memento.exec();

    /* Deallocate shared resources */
    delete main_window;
    delete GlobalMediator::getGlobalMediator();
    delete IconFactory::create();

    return ret;
}
