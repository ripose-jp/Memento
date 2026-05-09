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

#include "dict/dictionarycontroller.h"

#include <QtConcurrentRun>

#ifdef MEMENTO_SYSTEM_QCORO
#include <QCoroFuture>
#else
#include <qcoro/core/qcorofuture.h>
#endif // MEMENTO_SYSTEM_QCORO

/* Begin Constructor/Deconstructor */

DictionaryController::DictionaryController(
    Settings *settings, QObject *parent) :
    Dictionary(parent),
    m_settings(settings)
{
    if (m_settings != nullptr)
    {
        m_order.clear();
        const QList<int64_t> &order = m_settings->dictionaryOrder();
        for (qsizetype i = 0; i < order.size(); ++i)
        {
            m_order.emplace(order[i], i);
        }
    }

    updateDictionaries();
}

DictionaryController::~DictionaryController()
{

}

/* End Constructor/Deconstructor */
/* Begin Actions */

void DictionaryController::addDictionary(QString path)
{
    path = QUrl(path).toLocalFile();
    QFuture<int> result = QtConcurrent::run(
        &DatabaseManager::addDictionary,
        m_db,
        path
    );
    QCoro::connect(
        std::move(result),
        this,
        [this] (int ret) -> void
        {
            if (ret)
            {
                setLastError(m_db->errorCodeToString(ret));
                return;
            }
            updateDictionaries();
            writeOrder();
        }
    );
}

void DictionaryController::removeDictionary(int64_t id)
{
    QFuture<int> result = QtConcurrent::run(
        &DatabaseManager::deleteDictionary,
        m_db,
        id
    );
    QCoro::connect(
        std::move(result),
        this,
        [this] (int ret) -> void
        {
            if (ret)
            {
                setLastError(m_db->errorCodeToString(ret));
                return;
            }
            updateDictionaries();
            writeOrder();
        }
    );
}

void DictionaryController::moveDictionary(int from, int to)
{
    if (!dictionaries()->move(from, to))
    {
        return;
    }
    updateOrder();
    writeOrder();
}

/* End Actions */
/* Begin Slots */

void DictionaryController::updateDictionaries()
{
    QList<DictionaryInfo *> dicts = m_db->getDictionaries(this);
    for (DictionaryInfo *info : dicts)
    {
        connect(
            info, &DictionaryInfo::enabledChanged,
            this,
            [this, info] () { handleEnabledChanged(info); },
            Qt::QueuedConnection
        );
    }
    std::sort(
        std::begin(dicts), std::end(dicts),
        [this] (const DictionaryInfo *lhs, const DictionaryInfo *rhs) -> bool
        {
            qsizetype lhsPriority = m_order.contains(lhs->id()) ?
                m_order[lhs->id()] : std::numeric_limits<qsizetype>::max();
            qsizetype rhsPriority = m_order.contains(rhs->id()) ?
                m_order[rhs->id()] : std::numeric_limits<qsizetype>::max();
            return lhsPriority < rhsPriority;
        }
    );
    dictionaries()->setItems(std::move(dicts));
    updateOrder();
}

void DictionaryController::updateOrder()
{
    const QList<DictionaryInfo *> &dicts = dictionaries()->items();

    m_order.clear();
    for (qsizetype i = 0; i < dicts.size(); ++i)
    {
        m_order.emplace(dicts[i]->id(), i);
    }
}

void DictionaryController::writeOrder()
{
    if (m_settings == nullptr)
    {
        return;
    }

    QList<int64_t> items;
    items.reserve(m_dictionaries->items().size());
    for (const DictionaryInfo *item : m_dictionaries->items())
    {
        items.emplaceBack(item->id());
    }
    m_settings->setDictionaryOrder(items);
}

void DictionaryController::handleEnabledChanged(const DictionaryInfo *info)
{
    QFuture<int> result = QtConcurrent::run(
        info->enabled() ?
            &DatabaseManager::enableDictionary :
            &DatabaseManager::disableDictionary,
        m_db,
        info->id()
    );
    QCoro::connect(
        std::move(result),
        this,
        [this] (int ret) -> void
        {
            if (ret)
            {
                setLastError(m_db->errorCodeToString(ret));
                updateDictionaries();
                return;
            }
        }
    );
}

/* End Slots */
/* Begin Properties */

DictionaryInfoModel *DictionaryController::dictionaries() const noexcept
{
    return m_dictionaries;
}

const QString &DictionaryController::lastError() const noexcept
{
    return m_lastError;
}

void DictionaryController::setLastError(const QString &error)
{
    m_lastError = error;
    if (!m_lastError.isEmpty())
    {
        emit lastErrorChanged(m_lastError);
    }
}

/* End Properties */
