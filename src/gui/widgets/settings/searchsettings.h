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

#ifndef SEARCHSETTINGS_H
#define SEARCHSETTINGS_H

#include <QWidget>

#include <memory>

#include <QPointer>

#include "state/context.h"

namespace Ui
{
    class SearchSettings;
}

/**
 * Settings widget for configuring SubtitleWidget.
 */
class SearchSettings : public QWidget
{
    Q_OBJECT

public:
    SearchSettings(QPointer<Context> context, QWidget *parent = nullptr);
    virtual ~SearchSettings();

protected:
    /**
     * Restores saved settings on show.
     * @param event The show event, not used.
     */
    void showEvent(QShowEvent *event) override;

private Q_SLOTS:
    /**
     * Restores saved settings. Does not apply them.
     */
    void restoreSaved();

    /**
     * Restores the default settings. Does not apply them.
     */
    void restoreDefaults();

    /**
     * Applies current choices in the UI to the saved settings.
     */
    void applySettings();

    /**
     * Handles changes to the UI when the search method combobox is changed.
     * @param text The text of the combo box.
     */
    void methodTextChanged(const QString &text);

private:
    /* The UI object containing all the widgets. */
    std::unique_ptr<Ui::SearchSettings> m_ui;

    /* The application context */
    QPointer<Context> m_context = nullptr;
};

#endif // SEARCHSETTINGS_H
