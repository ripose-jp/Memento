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

#ifndef ANKISETTINGSHELP_H
#define ANKISETTINGSHELP_H

#include <QDialog>

namespace Ui
{
    class AnkiSettingsHelp;
}

/**
 * Help screen for Anki Integration Settings. Lists card markers and their
 * function.
 */
class AnkiSettingsHelp : public QDialog
{
    Q_OBJECT

public:
    AnkiSettingsHelp(QWidget *parent = 0);
    ~AnkiSettingsHelp();

private:
    Ui::AnkiSettingsHelp *m_ui;
};

#endif // ANKISETTINGSHELP_H
