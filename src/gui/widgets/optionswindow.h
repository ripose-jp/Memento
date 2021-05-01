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

#ifndef OPTIONSWINDOW_H
#define OPTIONSWINDOW_H

#include <QWidget>

#include <QMap>

namespace Ui
{
    class OptionsWindow;
}

class AnkiSettings;
class DictionarySettings;

class OptionsWindow : public QWidget
{
    Q_OBJECT

public:
    OptionsWindow(QWidget *parent = nullptr);
    ~OptionsWindow();

protected:
    void showEvent(QShowEvent *event) override;

private:
    Ui::OptionsWindow *m_ui;

    QWidget                  *m_currentWidget;
    QMap<QString, QWidget *>  widgets;

    void addOption(const QString &name, QWidget *widget);
    void showSelectedOption();
};

#endif // OPTIONSWINDOW_H