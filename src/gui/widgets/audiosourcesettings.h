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

#ifndef AUDIOSOURCESETTINGS_H
#define AUDIOSOURCESETTINGS_H

#include <QWidget>

namespace Ui
{
    class AudioSourceSettings;
}

class AudioSourceSettings : public QWidget
{
    Q_OBJECT

public:
    AudioSourceSettings(QWidget *parent = 0);
    ~AudioSourceSettings();

protected:
    void showEvent(QShowEvent *event) override;

private Q_SLOTS:
    void applyChanges();
    void restoreDefaults();
    void restoreSaved();
    void showHelp();

private:
    Ui::AudioSourceSettings *m_ui;

    QString verifyNames() const;
    void updateRows();

    bool inline itemEmpty(const int row, const int col) const;
    bool inline rowEmpty (const int row)                const;
};

#endif // AUDIOSOURCESETTINGS_H