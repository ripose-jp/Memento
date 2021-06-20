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

#ifndef GLOSSARYWIDGET_H
#define GLOSSARYWIDGET_H

#include <QWidget>

#include "../common/flowlayout.h"
#include <QLabel>
#include <QCheckBox>

#include "../../../dict/expression.h"

class GlossaryWidget : public QWidget
{
    Q_OBJECT

public:
    GlossaryWidget(size_t number, const TermDefinition &def, QWidget *parent = nullptr);

    void setCheckable(const bool value);
    bool isChecked() const;

private:
    const TermDefinition &m_def;

    QVBoxLayout *m_parentLayout;
    FlowLayout  *m_layoutHeader;
    QCheckBox   *m_checkBoxAdd;
    QLabel      *m_labelNumber;
    QLabel      *m_labelGlossary;
};

#endif // GLOSSARYWIDGET_H