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

#ifndef DEFINITION_H
#define DEFINITION_H

#include <QWidget>

#include "../../dict/entry.h"

namespace Ui
{
    class Definition;
}

class Definition : public QWidget
{
    Q_OBJECT

public:
    Definition(const Entry *entry, QWidget *parent = 0);
    ~Definition();

private:
    Ui::Definition *m_ui;
    const Entry *m_entry;

    void setEntry(const Entry *entry);
    QString generateJishoLink(const QString &word);
    QString buildDefinition(const QList<QList<QString>> &definitions);
};

#endif // DEFINITION_H