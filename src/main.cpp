////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2026 Ripose
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

#include <clocale>
#include <iostream>

#ifdef MEMENTO_QAPPLICATION
#include <QApplication>
#else
#include <QGuiApplication>
#endif // MEMENTO_QAPPLICATION

#include <QDir>
#include <QLocale>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QQuickWindow>
#include <QTranslator>

#ifdef MEMENTO_SYSTEM_QCORO
#include <QCoroQml>
#else
#include <qcoro/qml/qcoroqml.h>
#endif // MEMENTO_SYSTEM_QCORO

#include "anki/ankiclient.h"
#include "anki/ankiconfig.h"
#include "anki/ankifieldlistmodel.h"
#include "anki/ankiprofile.h"
#include "audio/audioplayer.h"
#include "definition/structuredrichtext.h"
#include "dict/dictionary.h"
#include "dict/dictionarycontroller.h"
#include "dict/dictionaryinfomodel.h"
#include "dict/dictionarysearch.h"
#include "dict/dictionarysearchcontroller.h"
#include "manager/mainmanager.h"
#include "ocr/ocrcontroller.h"
#include "os/screensaver.h"
#include "player/mpvplayer.h"
#include "player/mpvthumbnail.h"
#include "quick/clipboard.h"
#include "quick/coloredsvgprovider.h"
#include "quick/features.h"
#include "quick/keytracker.h"
#include "quick/paths.h"
#include "setting/settings.h"
#include "state/context.h"
#include "subtitle/subtitlelistmodel.h"
#include "subtitle/subtitlelists.h"
#include "util/utils.h"

static constexpr const char *MEMENTO_URI{"Ripose.Memento"};

/**
 * @brief Check if the --help command line arg was passed in.
 *
 * @param argc The number of command line arguments
 * @param argv The values of the command line arguments
 * @return true if the help message should be shown,
 * @return false otherwise
 */
[[nodiscard]]
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

/**
 * @brief Create the Memento config directory if it doesn't already exist.
 *
 * @return true if the directory was created or already existed,
 * @return false otherwise.
 */
[[nodiscard]]
static bool makeConfigDirectory()
{
    QDir dir;
    return dir.mkpath(DirectoryUtils::getConfigDir());
}

/**
 * @brief Register all types for use in QML.
 *
 * @param context The application context.
 */
static void registerQmlTypes(Context &context)
{
    /* QCoro Types */

    QCoro::Qml::registerTypes();

    /* Anki Types */

    qmlRegisterType<AnkiFieldListModel>(
        MEMENTO_URI, 1, 0, "AnkiFieldListModel"
    );
    qmlRegisterType<AnkiProfile>(MEMENTO_URI, 1, 0, "AnkiProfile");
    qmlRegisterSingletonInstance<AnkiConfig>(
        MEMENTO_URI, 1, 0, "AnkiConfig", context.ankiConfig()
    );
    qmlRegisterUncreatableMetaObject(
        Anki::staticMetaObject,
        MEMENTO_URI, 1, 0, "AnkiSetting",
        "AnkiSetting is an enums only namespace"
    );
    qmlRegisterSingletonInstance<AnkiClient>(
        MEMENTO_URI, 1, 0, "AnkiClient", context.ankiClient()
    );

    /* Audio Types */

    qmlRegisterSingletonInstance<AudioPlayer>(
        MEMENTO_URI, 1, 0, "AudioPlayer", context.audioPlayer()
    );

    /* Dictionary Types */

    qmlRegisterType<DictionaryInfo>(MEMENTO_URI, 1, 0, "DictionaryInfo");
    qmlRegisterType<Expression>(MEMENTO_URI, 1, 0, "Expression");
    qmlRegisterType<Frequency>(MEMENTO_URI, 1, 0, "Frequency");
    qmlRegisterType<Kanji>(MEMENTO_URI, 1, 0, "Kanji");
    qmlRegisterType<KanjiDefinition>(MEMENTO_URI, 1, 0, "KanjiDefinition");
    qmlRegisterType<Pitch>(MEMENTO_URI, 1, 0, "Pitch");
    qmlRegisterType<Tag>(MEMENTO_URI, 1, 0, "Tag");
    qmlRegisterType<Term>(MEMENTO_URI, 1, 0, "Term");
    qmlRegisterType<TermDefinition>(MEMENTO_URI, 1, 0, "TermDefinition");

    qmlRegisterType<Dictionary>(MEMENTO_URI, 1, 0, "Dictionary");
    qmlRegisterUncreatableType<DictionaryInfoModel>(
        MEMENTO_URI, 1, 0, "DictionaryItemModel",
        "DictionaryItemModel should only be created by DictionaryController"
    );
    qmlRegisterType<DictionarySearch>(
        MEMENTO_URI, 1, 0, "DictionarySearch"
    );
    qmlRegisterSingletonInstance<DictionaryController>(
        MEMENTO_URI, 1, 0, "DictionaryController",
        context.dictionaryController()
    );

    /* Definition Types */

    qmlRegisterSingletonInstance<StructuredRichText>(
        MEMENTO_URI, 1, 0, "StructuredRichText",
        new StructuredRichText(&context)
    );

    /* OS Types */

    qmlRegisterType<Screensaver>(MEMENTO_URI, 1, 0, "Screensaver");

    /* OCR Types */

    qmlRegisterSingletonInstance<OcrController>(
        MEMENTO_URI, 1, 0, "OcrController",
        new OcrController(context.settings(), &context)
    );

    /* Player Types */

    qmlRegisterType<MpvPlayer>(MEMENTO_URI, 1, 0, "MpvPlayer");
    qmlRegisterType<MpvThumbnail>(MEMENTO_URI, 1, 0, "MpvThumbnail");
    qmlRegisterUncreatableType<MpvState>(
        MEMENTO_URI, 1, 0, "MpvState",
        "MpvState cannot be created directly from QML. "
            "Create an MpvPlayer instead."
    );
    qmlRegisterUncreatableType<MpvSubtitle>(
        MEMENTO_URI, 1, 0, "MpvSubtitle",
        "MpvSubtitle cannot be created directly from QML. "
            "Create an MpvPlayer instead."
    );
    qmlRegisterUncreatableType<MpvTrack>(
        MEMENTO_URI, 1, 0, "MpvTrack",
        "MpvSubtitle cannot be created directly from QML. "
            "Create an MpvPlayer instead."
    );
    qmlRegisterUncreatableType<MpvController>(
        MEMENTO_URI, 1, 0, "MpvController",
        "MpvController cannot be created directly from QML. "
            "Create an MpvPlayer instead."
    );
    qmlRegisterType<MpvAudioClipArgs>(
        MEMENTO_URI, 1, 0, "mpvAudioClipArgs"
    );
    qmlRegisterType<MpvVideoClipArgs>(
        MEMENTO_URI, 1, 0, "mpvVideoClipArgs"
    );

    /* Setting Types */

    qmlRegisterSingletonInstance<Settings>(
        MEMENTO_URI, 1, 0, "MementoSettings", context.settings()
    );
    qmlRegisterUncreatableType<AudioSourceModel>(
        MEMENTO_URI, 1, 0, "AudioSourceModel",
        "AudioSourceModel should only be created by MementoSettings"
    );
    qmlRegisterUncreatableType<KeybindProfile>(
        MEMENTO_URI, 1, 0, "KeybindProfile",
        "KeybindProfile should only be created by MementoSettings"
    );
    qmlRegisterUncreatableType<KeybindProfileModel>(
        MEMENTO_URI, 1, 0, "KeybindProfileModel",
        "KeybindProfileModel should only be created by MementoSettings"
    );
    qmlRegisterUncreatableMetaObject(
        Setting::staticMetaObject,
        MEMENTO_URI, 1, 0, "MementoSetting",
        "MementoSetting is an enums only namespace"
    );

    /* Quick Types */

    qmlRegisterType<Clipboard>(MEMENTO_URI, 1, 0, "Clipboard");
    qmlRegisterSingletonInstance<Features>(
        MEMENTO_URI, 1, 0, "Features", new Features(&context)
    );
    qmlRegisterSingletonInstance<FileOpenHandler>(
        MEMENTO_URI, 1, 0, "FileOpenHandler", context.fileOpenHandler()
    );
    qmlRegisterSingletonInstance<KeyTracker>(
        MEMENTO_URI, 1, 0, "KeyTracker", new KeyTracker(&context)
    );
    qmlRegisterSingletonInstance<Paths>(
        MEMENTO_URI, 1, 0, "MementoPaths", new Paths(&context)
    );

    /* Subtitle Types */

    qmlRegisterUncreatableType<SubtitleListModel>(
        MEMENTO_URI, 1, 0, "SubtitleListModel",
        "SubtitleListModel should only be created by SubtitleLists"
    );
    qmlRegisterSingletonInstance<SubtitleLists>(
        MEMENTO_URI, 1, 0, "SubtitleLists", context.subtitleLists()
    );
}

/**
 * @brief Register all image providers with the QML application engine.
 *
 * @param engine The QML application engine.
 */
static void registerImageProviders(QQmlApplicationEngine &engine)
{
    engine.addImageProvider("svgicon", new ColoredSvgProvider);
}

/**
 * @brief Get candidate translation directories for this platform/build.
 *
 * @return A list of directories that may contain Memento .qm files.
 */
[[nodiscard]]
static QStringList translationDirectories()
{
    const QString appDir = QCoreApplication::applicationDirPath();
    QStringList dirs;

#if defined(Q_OS_MACOS) && MEMENTO_BUNDLE
    dirs.emplaceBack(
        QDir(appDir).absoluteFilePath("../Resources/translations")
    );
#endif // defined(Q_OS_MACOS) && MEMENTO_BUNDLE

    dirs.emplaceBack(QDir(appDir).absoluteFilePath("translations"));

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    dirs.emplaceBack(
        QDir(appDir).absoluteFilePath("../share/memento/translations")
    );
#endif // defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)

    dirs.removeDuplicates();
    return dirs;
}

/**
 * @brief Convert a QLocal::Language to a Setting::Language value.
 *
 * @param locale The locale to convert.
 * @return The corresponding language. English if it isn't supported.
 */
static Setting::Language localeToLanguage(const QLocale &locale) noexcept
{
    switch (locale.language())
    {
        case QLocale::Chinese:
            switch (locale.script())
            {
                case QLocale::TraditionalChineseScript:
                    return Setting::Language::LanguageChineseTraditional;

                case QLocale::SimplifiedChineseScript:
                    return Setting::Language::LanguageChineseSimplified;

                default:
                    break;
            }

            switch (locale.territory())
            {
                case QLocale::HongKong:
                case QLocale::Macau:
                case QLocale::Taiwan:
                    return Setting::Language::LanguageChineseTraditional;

                default:
                    return Setting::Language::LanguageChineseSimplified;
            }

        case QLocale::Korean:
            return Setting::Language::LanguageKorean;

        case QLocale::English:
        default:
            return Setting::Language::LanguageEnglish;
    }
}

/**
 * @brief Convert a setting language to the language to load.
 *
 * @param language The configured language.
 * @return The resolved language. English if the system language is unsupported.
 */
static inline Setting::Language resolveLanguage(
    Setting::Language language) noexcept
{
    if (language == Setting::Language::LanguageSystem)
    {
        return localeToLanguage(QLocale::system());
    }
    return language;
}

/**
 * @brief Install the configured application translator.
 *
 * @param translator The translator instance that will live until shutdown.
 * @param context The application context.
 */
static void installTranslator(QTranslator &translator, const Context &context)
{
    const Setting::Language language = resolveLanguage(
        context.settings()->applicationLanguage()
    );
    if (language == Setting::LanguageEnglish)
    {
        return;
    }

    QString fileName;
    switch (language)
    {
        case Setting::LanguageKorean:
            fileName = QStringLiteral("memento_ko");
            break;

        case Setting::LanguageChineseSimplified:
            fileName = QStringLiteral("memento_zh_CN");
            break;

        case Setting::LanguageChineseTraditional:
            fileName = QStringLiteral("memento_zh_TW");
            break;

        case Setting::LanguageSystem:
        case Setting::LanguageEnglish:
        default:
            return;
    }

    for (const QString &directory : translationDirectories())
    {
        if (translator.load(fileName, directory))
        {
            QCoreApplication::installTranslator(&translator);
            return;
        }
    }

    qWarning() << "Could not load translation" << fileName
               << "from" << translationDirectories();
}

/**
 * @brief Run the application.
 *
 * @return The application return code.
 */
[[nodiscard]]
static int runApplication()
{
    QQmlApplicationEngine engine;
#if defined(Q_OS_WIN)
    engine.addImportPath(QCoreApplication::applicationDirPath() + "/qml");
#endif // defined(Q_OS_WIN)
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        QCoreApplication::instance(),
        [] () { QCoreApplication::exit(-1); },
        Qt::QueuedConnection
    );

    Context context(&engine);
    QQmlApplicationEngine::setObjectOwnership(
        &context, QQmlEngine::CppOwnership
    );
    QCoreApplication::instance()->installEventFilter(
        context.fileOpenHandler()
    );

    QTranslator translator;
    installTranslator(translator, context);

    DictionarySearchController::createInstance(context.settings());

    registerQmlTypes(context);
    registerImageProviders(engine);

    MainManager mainManager(&engine, &context, &engine);

    engine.loadFromModule(MEMENTO_URI, "Main");

    return QCoreApplication::exec();
}

int main(int argc, char *argv[])
{
    if (showHelpMessage(argc, argv))
    {
        std::cout << "Usage: memento [options] [url|path]\n"
            << "\n"
            << "For more information about commandline arguments, see "
               "https://mpv.io/manual/\n";
        return 0;
    }

    /* Organization Info */
    QCoreApplication::setOrganizationName("memento");
    QCoreApplication::setOrganizationDomain("ripose.projects");
    QCoreApplication::setApplicationName("memento");

    /* Make the icon show up on Wayland */
    QGuiApplication::setDesktopFileName("memento");

#ifndef Q_OS_MACOS
    /* mpv uses OpenGL on these platforms, so Qt Quick must share that API. */
    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);
#endif // Q_OS_MACOS

#ifdef MEMENTO_QAPPLICATION
    QApplication app(argc, argv);
#else
    QGuiApplication app(argc, argv);
#endif // MEMENTO_QAPPLICATION

    if (!makeConfigDirectory())
    {
        qFatal("Could not make config directory");
    }

    std::setlocale(LC_NUMERIC, "C");

#if defined(Q_OS_WIN)
    app.setWindowIcon(QIcon(":/memento.ico"));
#endif // defined(Q_OS_WIN)

#if defined(Q_OS_MACOS) || defined(Q_OS_WIN)
    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE"))
    {
        QQuickStyle::setStyle(QStringLiteral("FluentWinUI3"));
    }
#else
    /* Try to avoid overriding the default theme unless it's org.kde.desktop */
    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE") &&
        QQuickStyle::name() == "org.kde.desktop")
    {
        QQuickStyle::setStyle(QStringLiteral("org.kde.breeze"));
        QQuickStyle::setFallbackStyle(QStringLiteral("Fusion"));
    }
#endif // defined(Q_OS_MACOS) || defined(Q_OS_WIN)

    Dictionary::createDatabaseInstance();

    int ret = runApplication();

    DictionarySearchController::destroyInstance();
    Dictionary::destroyDatabaseInstance();

    return ret;
}
