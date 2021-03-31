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

namespace Ui
{
    class SearchSettings;
}

class SearchSettings : public QWidget
{
    Q_OBJECT

public:
    SearchSettings(QWidget *parent = nullptr);
    ~SearchSettings();

protected:
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;

private Q_SLOTS:
    void resetSaved();
    void resetDefault();
    void applySettings();

    void methodTextChanged(const QString &text);

private:
    Ui::SearchSettings *m_ui;
};

#endif
