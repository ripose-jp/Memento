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

#include "setting/keys.h"

/**
 * @brief Allows QML to get information about currently held keys.
 */
class KeyTracker : public QObject
{
    Q_OBJECT

public:
    KeyTracker(QObject *parent = nullptr);
    ~KeyTracker();

    /**
     * @brief Get the currently held modifiers.
     *
     * @return The currently held modifiers.
     */
    [[nodiscard]]
    Q_INVOKABLE static Qt::KeyboardModifiers modifiers();

    /**
     * @brief Get if the modifier is held.
     *
     * @param key The modifier key.
     * @return true if the modifier is held,
     * @return false otherwise.
     */
    [[nodiscard]]
    Q_INVOKABLE static bool modifierHeld(Setting::Modifier key);

    /**
     * @brief Get a sequence string from a key combination.
     *
     * @param key The key being pressed.
     * @param modifiers The modifiers being pressed.
     * @return The portable key sequence string.
     */
    [[nodiscard]]
    Q_INVOKABLE static QString keyComboToString(int key, int modifiers);
};
