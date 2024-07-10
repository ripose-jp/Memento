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

#include <cstring>
#include <iostream>

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFontDatabase>
#include <QMessageBox>
#include <QSettings>

#if defined(Q_OS_WIN)
#include <QStandardPaths>
#endif

#if defined(Q_OS_MACOS)
#include <locale.h>
#include <QSurfaceFormat>
#endif

#include "audio/audioplayer.h"
#include "dict/dictionary.h"
#include "dict/expression.h"
#include "gui/mainwindow.h"
#include "util/constants.h"
#include "util/globalmediator.h"
#include "util/iconfactory.h"
#include "util/utils.h"

/**
 * Makes sure nothing compiles on unsupported OSes
 */
#if !(defined(Q_OS_UNIX) && !defined(Q_OS_DARWIN)) && !defined(Q_OS_WIN) && !defined(Q_OS_MACOS)
#error "OS not supported"
#endif

Q_DECLARE_METATYPE(SharedTerm)
Q_DECLARE_METATYPE(SharedTermList)
Q_DECLARE_METATYPE(SharedKanji)

/**
 * Updates the QSettings before the MainWindow is created.
 * This is used during updates to make sure that configurations for old versions
 * don't cause problems.
 */
void updateSettings()
{
    QSettings settings;
    uint version = settings.value(Constants::Settings::Version::VERSION, 0).toUInt();
    if (version == Constants::Settings::Version::CURRENT)
    {
        return;
    }
    else if (version > Constants::Settings::Version::CURRENT)
    {
        QMessageBox message;
        message.critical(
            0, "Settings From a Newer Version Found",
            "The Memento settings found belong to a newer version.\n"
            "No guarantees can be made that nothing will break or get lost."
        );
    }

    /* Migrate the settings */
    switch(version)
    {
    case 0:
    {
        settings.beginGroup(Constants::Settings::Interface::GROUP);
        settings.remove(Constants::Settings::Interface::Style::SUBTITLE_LIST);
        settings.endGroup();
        __attribute__((fallthrough));
    }
    case 1:
    {
        /* These paths are hardcoded because DirectoryUtils may change in the
         * future. */
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
        QDir configDir(QString(getenv("HOME")) + "/.config/memento");
        configDir.rename("./dict/dictionaries.sqlite", "./dictionaries.sqlite");

        QDir dictDir(configDir.absolutePath() + "/dict");
        dictDir.removeRecursively();
#elif defined(Q_OS_WIN)
        QDir programDir(DirectoryUtils::getProgramDirectory());
        programDir.rename(
            ".\\config\\dict\\dictionaries.sqlite",
            ".\\config\\dictionaries.sqlite"
        );

        QDir dictDir(DirectoryUtils::getProgramDirectory() + "config\\dict");
        dictDir.removeRecursively();

        QString configPath = QDir::toNativeSeparators(
                QStandardPaths::writableLocation(
                    QStandardPaths::AppConfigLocation
                )
            );
        configPath.chop(sizeof("memento") - 1);
        QDir configDir(configPath);
        configDir.removeRecursively();

        programDir.rename(".\\config", configDir.absolutePath());
#endif
        __attribute__((fallthrough));
    }
    case 2:
    {
        settings.beginGroup(Constants::Settings::Search::GROUP);
        bool list = settings.value(
                Constants::Settings::Search::LIST_GLOSSARY,
                true
            ).toBool();
        settings.setValue(
            Constants::Settings::Search::LIST_GLOSSARY,
            static_cast<int>(
                list ?
                    Constants::GlossaryStyle::Bullet :
                    Constants::GlossaryStyle::LineBreak
            )
        );
        settings.endGroup();
    }
    }

    /* Remove saved window configurations just to be safe */
    settings.beginGroup(Constants::Settings::Window::GROUP);
    settings.remove("");
    settings.endGroup();

    /* Set the version */
    settings.setValue(
        Constants::Settings::Version::VERSION,
        Constants::Settings::Version::CURRENT
    );
}

/**
 * Registers MetaTypes for use with signals and slots.
 */
static void registerMetaTypes()
{
    qRegisterMetaType<SharedTerm>("SharedTerm");
    qRegisterMetaType<SharedTermList>("SharedTermList");
    qRegisterMetaType<SharedKanji>("SharedKanji");
}

/**
 * Checks if the --help command line arg was passed in
 * @param argc The number of command line arguments
 * @param argv The values of the command line arguments
 * @return true if the help message should be shown
 * @return false otherwise
 */
static inline bool showHelpMessage(int argc, char **argv)
{
    for (int i = 1; i < argc; ++i)
    {
        if (std::strcmp(argv[i], "-h") == 0 ||
            std::strcmp(argv[i], "--help") == 0)
        {
            return true;
        }
    }
    return false;
}

int main(int argc, char **argv)
{
    if (showHelpMessage(argc, argv))
    {
        std::cout << "Usage:\t memento [options] [url|path]\n"
            << "\n"
            << "For more information about command line arguments, see "
               "https://mpv.io/manual/\n";
        return 0;
    }

#if defined(Q_OS_WIN)
    /* Image Formats Windows */
    QCoreApplication::addLibraryPath(DirectoryUtils::getProgramDirectory());
#endif

    /* Organization Info */
    QCoreApplication::setOrganizationName("memento");
    QCoreApplication::setOrganizationDomain("ripose.projects");
    QCoreApplication::setApplicationName("memento");

    /* Make the icon show up on Wayland */
    QGuiApplication::setDesktopFileName("memento");

    /* Construct the application */
    QApplication memento(argc, argv);

#if defined(Q_OS_MACOS)
    /* Change the OpenGL version to 4.1 on macOS */
    QSurfaceFormat qSurfaceFormat;
    qSurfaceFormat.setMajorVersion(4);
    qSurfaceFormat.setMinorVersion(1);
    qSurfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(qSurfaceFormat);
#endif

#if !defined(Q_OS_MACOS)
    /* Set the window icon */
    QGuiApplication::setWindowIcon(QIcon(":memento.svg"));
#endif

    updateSettings();

    /* Create the configuration directory if it doesn't exist */
    if (!QDir(DirectoryUtils::getConfigDir()).exists() &&
        !QDir().mkdir(DirectoryUtils::getConfigDir()))
    {
        QMessageBox message;
        message.critical(
            0, "Error Creating Config Directory",
            "Could not make configuration directory at " +
            DirectoryUtils::getConfigDir()
        );
        return EXIT_FAILURE;
    }

    /* Create the resource directory if it doesn't exist */
    if (!QDir(DirectoryUtils::getDictionaryResourceDir()).exists() &&
        !QDir().mkdir(DirectoryUtils::getDictionaryResourceDir()))
    {
        QMessageBox message;
        message.critical(
            0, "Error Creating Resource Directory",
            "Could not make resource directory at " +
            DirectoryUtils::getDictionaryResourceDir()
        );
        return EXIT_FAILURE;
    }

    /* General Setup */
    registerMetaTypes();
    setlocale(LC_NUMERIC, "C"); // mpv requires this

    GlobalMediator::createGlobalMediator();
    GlobalMediator::getGlobalMediator()->setAudioPlayer(new AudioPlayer);

    MainWindow *main_window = new MainWindow;
    main_window->show();
    int ret = memento.exec();

    /* Deallocate shared resources */
    delete main_window;
    delete GlobalMediator::getGlobalMediator()->getAudioPlayer();
    delete GlobalMediator::getGlobalMediator()->getDictionary();
    delete GlobalMediator::getGlobalMediator();
    IconFactory::destroy();

    return ret;
}
