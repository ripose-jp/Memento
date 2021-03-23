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

#ifndef DICTIONARYSETTINGS_H
#define DICTIONARYSETTINGS_H

#include <QWidget>

namespace Ui
{
    class DictionarySettings;
}

class DictionarySettings : public QWidget
{
    Q_OBJECT

public:
    DictionarySettings(QWidget *parent = nullptr);
    ~DictionarySettings();

protected:
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
    void hideEvent(QHideEvent *event) Q_DECL_OVERRIDE;

private Q_SLOTS:
    void refresh();
    void applySettings();
    void setButtonsEnabled(const bool value);
    void moveUp();
    void moveDown();
    void addDictionary();
    void deleteDictionary();

private:
    Ui::DictionarySettings *m_ui;
};

#endif // DICTIONARYSETTINGS_H