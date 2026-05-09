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

class Clipboard : public QObject
{
    Q_OBJECT

public:
    Clipboard(QObject *parent = nullptr);
    virtual ~Clipboard() = default;

    /**
     * @brief Gets the current clipboard text.
     *
     * @return The current clipboard text.
     */
    Q_INVOKABLE QString text() const;

    /**
     * @brief Sets the current text of the clipboard.
     *
     * @param text The text to set.
     */
    Q_INVOKABLE void setText(const QString &text);
};
