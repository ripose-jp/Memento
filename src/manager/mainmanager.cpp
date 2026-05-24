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

#include "manager/mainmanager.h"

#include <QCoreApplication>

#include "dict/dictionary.h"
#include "manager/playermanager.h"
#include "manager/subtitlelistmanager.h"
#include "player/mpvplayer.h"
#include "state/context.h"

MainManager::MainManager(
    QQmlApplicationEngine *engine, Context *context, QObject *parent) :
    QObject(parent),
    m_engine(engine),
    m_context(context)
{
    connect(
        engine, &QQmlApplicationEngine::objectCreated,
        this, &MainManager::handleObjectCreated,
        Qt::QueuedConnection
    );
}

void MainManager::handleObjectCreated()
{
    QObjectList rootObjects = m_engine->rootObjects();
    if (rootObjects.isEmpty())
    {
        qFatal("Could not get root object. Exiting...");
        qApp->exit(-1);
        return;
    }
    QObject *qmlRoot = rootObjects.first();

    m_context->setPlayer(qmlRoot->findChild<MpvPlayer *>("mainPlayer"));
    if (m_context->player() == nullptr)
    {
        qFatal("Cannot find MpvPlayer in QML. Exiting...");
        qApp->exit(-1);
        return;
    }

    /* Initialize managers */
    m_playerManager = new PlayerManager(m_context, this);
    m_subtitleListManager = new SubtitleListManager(m_context, this);
}
