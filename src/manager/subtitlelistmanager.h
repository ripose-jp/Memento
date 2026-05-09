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

#include <memory>

#ifdef MEMENTO_SYSTEM_QCORO
#include <QCoroTask>
#else
#include <qcoro/qcorotask.h>
#endif // MEMENTO_SYSTEM_QCORO

class Context;
class MpvSubtitle;
class SubtitleListModel;
class SubtitleParser;

/**
 * @brief Manager for populating and handling subtitle lists.
 */
class SubtitleListManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Construct a subtitle list manager.
     *
     * @param context The application context.
     * @param parent The parent of this object.
     */
    SubtitleListManager(Context *context, QObject *parent = nullptr);
    virtual ~SubtitleListManager();

private slots:
    /**
     * @brief Clear all subtitle lists and reset the SubtitleLists object.
     */
    void clearLists();

    /**
     * @brief Allocate all subtitle tracks. Clears the model only when
     * necessary.
     */
    void handleSubtitleTracksChanged();

    /**
     * @brief Update the primary list model.
     *
     * @param sid The primary SID.
     */
    void handleSidChanged(int64_t sid);

    /**
     * @brief Update the secondary list model.
     *
     * @param sid The secondary SID.
     */
    void handleSecondarySidChanged(int64_t sid);

    /**
     * @brief Add and select the latest primary subtitle.
     */
    void handlePrimarySubtitleChanged();

    /**
     * @brief Add and select the latest secondary subtitle.
     */
    void handleSecondarySubtitleChanged();

    /**
     * @brief Add and select a subtitle.
     *
     * @param model The model to update.
     * @param subtitle The subtitle to add.
     * @param position The current player position without delay.
     */
    void addSubtitle(
        SubtitleListModel *model,
        MpvSubtitle *subtitle,
        double position);

    /**
     * @brief Read external subtitle files and populate the models.
     *
     * @param model The model to read subtitles into.
     * @param path Path to the subtitles.
     * @return An awaitable task.
     */
    QCoro::Task<void> readExternalSubtitles(
        QPointer<SubtitleListModel> model, QString path);

private:
    /* The application context */
    Context *m_context{nullptr};

    /* The subtitle parser */
    std::unique_ptr<SubtitleParser> m_subtitleParser;

    /* The subtitle list models */
    QList<SubtitleListModel *> m_models;
};
