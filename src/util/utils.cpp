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

#include "utils.h"

#include <QCryptographicHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QStringList>

#include "constants.h"
#include "globalmediator.h"

/* Begin Directory Utils */

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #include <windows.h>
    #define CONFIG_PATH "config"
#elif __linux__
    #define BASE_DIR getenv("HOME")
    #define CONFIG_PATH "/.config/memento/"
#elif __APPLE__
    #define BASE_DIR getenv("HOME")
    #define CONFIG_PATH "/.config/memento/"
#else
    #error "OS not supported"
#endif

#define DICTIONARY_DIR "dict"

QString DirectoryUtils::getProgramDirectory()
{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    WCHAR buf[MAX_PATH];
    GetModuleFileNameW(NULL, buf, MAX_PATH);
    QString path = QString::fromWCharArray(buf);
    return path.left(path.lastIndexOf(SLASH) + 1);
#elif __linux__ || __APPLE__
    return BASE_DIR;
#endif
}

QString DirectoryUtils::getConfigDir()
{
    QString path = getProgramDirectory();
    path += CONFIG_PATH;
    path += SLASH;
    return path;
}

QString DirectoryUtils::getDictionaryDir()
{
    return getConfigDir() + DICTIONARY_DIR + SLASH;
}

QString DirectoryUtils::getDictionaryDB()
{
    return getDictionaryDir() + DICT_DB_FILE;
}

QString DirectoryUtils::getMpvInputConfig()
{
    return getConfigDir() + MPV_INPUT_CONF_FILE;
}

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #undef CONFIG_PATH
#elif __linux__
    #undef BASE_DIR
    #undef CONFIG_PATH
#elif __APPLE__
    #undef BASE_DIR
    #undef CONFIG_PATH
#endif

/* End DirectoryUtils */
/* Begin FileUtils */

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

/* End FileUtils */
/* Begin NetworkUtils */

void NetworkUtils::checkForUpdates()
{
    QNetworkAccessManager *manager = new QNetworkAccessManager;
    manager->setTransferTimeout();
    QNetworkRequest request(GITHUB_API_LINK);
    QNetworkReply *reply = manager->get(request);
    reply->connect(reply, &QNetworkReply::finished, reply,
        [=] {
            GlobalMediator *mediator = GlobalMediator::getGlobalMediator();
            QJsonDocument replyDoc;
            QJsonArray replyArr;
            QJsonValue replyVal;
            QJsonObject replyObj;
            QString tag;
            QString url;

            /* Make sure there weren't any errors */
            if (reply->error())
            {
                Q_EMIT mediator->showCritical(
                    "Error",
                    "Could not check for updates:\n" + reply->errorString()
                );
                goto cleanup;
            }

            /* Error check the reply to make sure it was valid */
            replyDoc = QJsonDocument::fromJson(reply->readAll());
            if (replyDoc.isNull() || !replyDoc.isArray())
            {
                goto error;
            }
            replyArr = replyDoc.array();
            if (replyArr.isEmpty())
            {
                goto error;
            }
            replyVal = replyArr.first();
            if (!replyVal.isObject())
            {
                goto error;
            }
            replyObj = replyVal.toObject();
            tag = replyObj.value("tag_name").toString();
            url = replyObj.value("html_url").toString();
            if (tag.isEmpty() || url.isEmpty())
            {
                goto error;
            }

            /* Get the url and tag */
            if (tag != VERSION)
            {
                Q_EMIT mediator->showInformation(
                    "Update Available",
                    QString("New version <a href='%1'>%2</a> available.")
                        .arg(url)
                        .arg(tag)
                );
                goto cleanup;
            }

            /* If reached, Memento was up to date */
            Q_EMIT mediator->showInformation(
                "Up to Date", "Memento is up to date"
            );

        cleanup:
            reply->deleteLater();
            manager->deleteLater();
            return;

        error:
            Q_EMIT mediator->showCritical(
                "Error",
                "Server did not send a valid reply.\n"
                "Check manually <a href='" + GITHUB_RELEASES +
                "'>here</a>"
            );
            reply->deleteLater();
            manager->deleteLater();
        }
    );
}

/* End NetworkUtils */
/* Begin GraphicUtils */

/**
 * Header for pitch graphs.
 * @param 1 Width of the viewBox.
 * @param 2 Fill color. Hex string format.
 * @param 3 Stroke color. Hex string format.
 */
#define PITCH_GRAPH_HEADER (QString(                                                                                    \
    "<svg xmlns=\"http://www.w3.org/2000/svg\" height=\"2em\" viewBox=\"0 0 %1 100\" style=\"display: inline-block;\">" \
    "<defs>"                                                                                                            \
    "<g id=\"black-dot\">"                                                                                              \
    "<circle cx=\"0\" cy=\"0\" r=\"15\" style=\"fill: %3; stroke: %3; stroke-width: 5px;\"/>"                           \
    "</g>"                                                                                                              \
    "<g id=\"white-dot\">"                                                                                              \
    "<circle cx=\"0\" cy=\"0\" r=\"15\" style=\"fill: %2; stroke: %3; stroke-width: 5px;\"/>"                           \
    "</g>"                                                                                                              \
    "<g id=\"change-dot\">"                                                                                             \
    "<circle cx=\"0\" cy=\"0\" r=\"15\" style=\"fill: %2; stroke: %3; stroke-width: 5px;\"/>"                           \
    "<circle cx=\"0\" cy=\"0\" r=\"5\" style=\"fill: %2; stroke: %3; stroke-width: 5px;\"/>"                            \
    "</g>"                                                                                                              \
    "</defs>"))
#define PITCH_GRAPH_TRAILER (QString("</svg>"))

/**
 * Creates a dot filled with the stroke color and stroked with the stroke color.
 * @param 1 The x coordinate.
 * @param 2 The y coordinate.
 */
#define BLACK_DOT_FORMAT    (QString("<use href=\"#black-dot\" x=\"%1\" y=\"%2\"/>"))

/**
 * Creates a dot filled with the fill color and stroked with the stroke color.
 * @param 1 The x coordinate.
 * @param 2 The y coordinate.
 */
#define WHITE_DOT_FORMAT    (QString("<use href=\"#white-dot\" x=\"%1\" y=\"%2\"/>"))

/**
 * Creates a dot filled with the fill color and stroked with the stroke color.
 * @param 1 The x coordinate.
 * @param 2 The y coordinate.
 */
#define CHANGE_DOT_FORMAT   (QString("<use href=\"#change-dot\" x=\"%1\" y=\"%2\"/>"))

/**
 * Draws a line.
 * @param 1 The color of the line.
 * @param 2 The starting x coordinate.
 * @param 3 The starting y coordinate.
 * @param 4 The ending x coordinate.
 * @param 5 The ending y coordinate.
 */
#define LINE_FORMAT         (QString("<path d=\"M%2 %3 L%4 %5\" style=\"stroke: %1; stroke-width: 5px;\"/>"))

#define GRAPH_TOP_Y     25
#define GRAPH_BOTTOM_Y  75
#define GRAPH_STEP      50
#define GRAPH_OFFSET    25

QString GraphicUtils::generatePitchGraph(const int     moraSize,
                                         const uint8_t pos,
                                         const QString &fill,
                                         const QString &stroke)
{
    QString pitchGraph = PITCH_GRAPH_HEADER
        .arg(GRAPH_STEP * (moraSize + 1))
        .arg(fill)
        .arg(stroke);
    QString lineFormat = LINE_FORMAT.arg(stroke);

    QStringList lines;
    QStringList dots;

    switch (pos)
    {
    case 0:
    {
        int x1 = GRAPH_OFFSET;
        int y1 = GRAPH_BOTTOM_Y;
        int x2 = GRAPH_OFFSET + GRAPH_STEP;
        int y2 = GRAPH_TOP_Y;

        lines.append(lineFormat.arg(x1).arg(y1).arg(x2).arg(y2));
        dots.append(BLACK_DOT_FORMAT.arg(x1).arg(y1));

        size_t i;
        for (i = 1; i < moraSize; ++i)
        {
            x1 = x2;
            y1 = y2;
            x2 = GRAPH_OFFSET + GRAPH_STEP * (i + 1);
            y2 = GRAPH_TOP_Y;

            lines.append(lineFormat.arg(x1).arg(y1).arg(x2).arg(y2));
            dots.append(BLACK_DOT_FORMAT.arg(x1).arg(y1));
        }

        dots.append(WHITE_DOT_FORMAT.arg(x2).arg(y2));
        break;
    }
    case 1:
    {
        int x1 = GRAPH_OFFSET;
        int y1 = GRAPH_TOP_Y;
        int x2 = GRAPH_OFFSET + GRAPH_STEP;
        int y2 = GRAPH_BOTTOM_Y;

        lines.append(lineFormat.arg(x1).arg(y1).arg(x2).arg(y2));
        dots.append(CHANGE_DOT_FORMAT.arg(x1).arg(y1));

        size_t i;
        for (i = 1; i < moraSize; ++i)
        {
            x1 = x2;
            y1 = y2;
            x2 = GRAPH_OFFSET + GRAPH_STEP * (i + 1);
            y2 = GRAPH_BOTTOM_Y;

            lines.append(lineFormat.arg(x1).arg(y1).arg(x2).arg(y2));
            dots.append(BLACK_DOT_FORMAT.arg(x1).arg(y1));
        }

        dots.append(WHITE_DOT_FORMAT.arg(x2).arg(y2));
        break;
    }
    default:
    {
        int x1 = GRAPH_OFFSET;
        int y1 = GRAPH_BOTTOM_Y;
        int x2 = GRAPH_OFFSET + GRAPH_STEP;
        int y2 = GRAPH_TOP_Y;

        lines.append(lineFormat.arg(x1).arg(y1).arg(x2).arg(y2));
        dots.append(BLACK_DOT_FORMAT.arg(x1).arg(y1));

        size_t i;
        for (i = 1; i < pos - 1; ++i)
        {
            x1 = x2;
            y1 = y2;
            x2 = GRAPH_OFFSET + GRAPH_STEP * (i + 1);
            y2 = GRAPH_TOP_Y;

            lines.append(lineFormat.arg(x1).arg(y1).arg(x2).arg(y2));
            dots.append(BLACK_DOT_FORMAT.arg(x1).arg(y1));
        }

        /* Change dot */
        x1 = x2;
        y1 = y2;
        x2 = GRAPH_OFFSET + GRAPH_STEP * (i + 1);
        y2 = GRAPH_BOTTOM_Y;

        lines.append(lineFormat.arg(x1).arg(y1).arg(x2).arg(y2));
        dots.append(CHANGE_DOT_FORMAT.arg(x1).arg(y1));

        /* End dots */
        for (i = pos; i < moraSize; ++i)
        {
            x1 = x2;
            y1 = y2;
            x2 = GRAPH_OFFSET + GRAPH_STEP * (i + 1);
            y2 = GRAPH_BOTTOM_Y;

            lines.append(lineFormat.arg(x1).arg(y1).arg(x2).arg(y2));
            dots.append(BLACK_DOT_FORMAT.arg(x1).arg(y1));
        }

        dots.append(WHITE_DOT_FORMAT.arg(x2).arg(GRAPH_BOTTOM_Y));
    }
    }

    pitchGraph += lines.join("\n");
    pitchGraph += dots.join("\n");

    pitchGraph += PITCH_GRAPH_TRAILER;

    return pitchGraph;
}

#undef PITCH_GRAPH_HEADER
#undef PITCH_GRAPH_TRAILER

#undef BLACK_DOT_FORMAT
#undef WHITE_DOT_FORMAT
#undef CHANGE_DOT_FORMAT
#undef LINE_FORMAT

#undef GRAPH_TOP_Y
#undef GRAPH_BOTTOM_Y
#undef GRAPH_STEP
#undef GRAPH_OFFSET

/* End GraphicUtils */
