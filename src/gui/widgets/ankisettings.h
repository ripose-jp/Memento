////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2020 Ripose
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

#ifndef ANKISETTINGS_H
#define ANKISETTINGS_H

#include <QWidget>

#include <QMutex>

#include "../../anki/ankiclient.h"

namespace Ui
{
    class AnkiSettings;
}

class AnkiSettings : public QWidget
{
    Q_OBJECT

public:
    AnkiSettings(AnkiClient *client, QWidget *parent = 0);
    ~AnkiSettings();

private Q_SLOTS:
    void enabledStateChanged(int state);
    void connectToClient(const bool showErrors = true);
    void updateModelFields(const QString &model);
    void applyChanges();
    void restoreDefaults();
    void restoreSaved();

private:
    Ui::AnkiSettings *m_ui;
    AnkiClient *m_client;
    QMutex m_mutexUpdateModelFields;
};

#endif // ANKISETTINGS_H