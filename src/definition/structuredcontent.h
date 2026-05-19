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

#pragma once

#include <QObject>

#include <QFont>
#include <QJsonArray>
#include <QString>
#include <QVariantMap>

#include "setting/keys.h"

class DictionaryInfo;

/**
 * @brief Parses glossary content into a native QML document description.
 *
 * The parser intentionally supports only the subset of CSS that maps cleanly to
 * native Qt Quick items. Unsupported declarations are ignored so dictionaries
 * still render with sensible structural defaults instead of requiring a browser
 * engine.
 */
class StructuredContent : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Construct a StructuredContent helper.
     *
     * @param parent The parent of this object.
     */
    StructuredContent(QObject *parent = nullptr);

    /**
     * @brief Destroy this StructuredContent helper.
     */
    virtual ~StructuredContent();

    /**
     * @brief Parse glossary data into a native document description.
     *
     * @param dictionaryInfo The dictionary metadata.
     * @param content The glossary content to parse.
     * @param glossaryStyle The requested glossary display style.
     * @param font The base glossary font.
     * @return A nested QVariantMap document suitable for QML rendering.
     */
    [[nodiscard]]
    Q_INVOKABLE static QVariantMap document(
        DictionaryInfo *dictionaryInfo,
        const QJsonArray &content,
        Setting::GlossaryStyle glossaryStyle,
        const QFont &font);
};
