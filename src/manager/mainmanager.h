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

#include <QQmlApplicationEngine>

class Context;
class PlayerManager;
class SubtitleListManager;

/**
 * @brief Manages state and initialization for all of the C++ backend.
 */
class MainManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Construct a new Main Manager object
     *
     * @param engine The QML application engine to manage. Doesn't take
     * ownership.
     * @param context The context for the application. Doesn't take ownership.
     * @param parent The parent of this object.
     */
    MainManager(
        QQmlApplicationEngine *engine,
        Context *context,
        QObject *parent = nullptr);
    virtual ~MainManager() = default;

private slots:
    /**
     * @brief Handles the objectCreated signal from the engine. Allows this
     * manager to populate context fields and initialize other objects.
     */
    void handleObjectCreated();

private:
    /* The QML application engine */
    QQmlApplicationEngine *m_engine{nullptr};

    /* The context of the application */
    Context *m_context{nullptr};

    /* The player manager */
    PlayerManager *m_playerManager{nullptr};

    /* The subtitle list manager */
    SubtitleListManager *m_subtitleListManager{nullptr};
};
