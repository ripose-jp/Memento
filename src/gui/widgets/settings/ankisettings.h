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

#ifndef ANKISETTINGS_H
#define ANKISETTINGS_H

#include <QWidget>

#include <QMutex>

#include "ankisettingshelp.h"
#include "../../../anki/ankiclient.h"
#include "../../../anki/ankiconfig.h"

namespace Ui
{
    class AnkiSettings;
}

class CardBuilder;

class AnkiSettings : public QWidget
{
    Q_OBJECT

public:
    AnkiSettings(QWidget *parent = 0);
    ~AnkiSettings();

protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private Q_SLOTS:
    void refreshIcons();
    void enabledStateChanged(int state);
    void connectToClient(const bool showErrors = true);
    void updateModelFields(CardBuilder *cb, const QString &model);
    void applyChanges();
    void restoreDefaults();
    void restoreSaved();
    void addProfile();
    void deleteProfile();
    void changeProfile(const QString &text);

private:
    Ui::AnkiSettings             *m_ui;
    AnkiSettingsHelp             *m_ankiSettingsHelp;
    QHash<QString, AnkiConfig *> *m_configs;
    QMap<QString, AudioSource>    m_audioSources;
    QString                       m_currentProfile;
    QMutex                        m_mutexUpdateModelFields;

    void clearConfigs();

    void populateAudioSources();

    void populateFields(const QString &profile, const AnkiConfig *config);

    QString duplicatePolicyToString(AnkiConfig::DuplicatePolicy policy);

    AnkiConfig::DuplicatePolicy stringToDuplicatePolicy(const QString &str);

    QString fileTypeToString(AnkiConfig::FileType type);

    AnkiConfig::FileType stringToFileType(const QString &str);

    void applyToConfig(const QString &profile);

    void renameProfile(const QString &oldName, const QString &newName);
};

#endif // ANKISETTINGS_H