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

#include <QCheckBox>
#include <QLabel>

#include "../../../dict/expression.h"
#include "../common/flowlayout.h"

/**
 * Widget for displaying a TermDefinition.
 */
class GlossaryWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructor for the GlossaryWidget.
     * @param number The number to label the term.
     * @param def    The term definition to display.
     * @param list   Whether newlines are displayed in an ordered list.
     * @param parent The parent of the GlossaryWidget.
     */
    GlossaryWidget(const size_t          number,
                   const TermDefinition &def,
                   const bool            list,
                   QWidget              *parent = nullptr);

    /**
     * Shows the checkbox next to the term.
     * @param value If true shows the checkbox, otherwise it hides it.
     */
    void setCheckable(const bool value);

    /**
     * Sets the the check value of the glossary.
     * @param value true to check, false to uncheck.
     */
    void setChecked(const bool value);

    /**
     * Returns if the checkbox is checked.
     * Undefined behavior if the checkbox is not visible.
     * @returns true if checked, false otherwise.
     */
    bool isChecked() const;

private:
    /* The term definition this widget displays. */
    const TermDefinition &m_def;

    /* The parent layout of this widget. */
    QVBoxLayout *m_parentLayout;

    /* The layout of the first line of the definition. Includes checkbox, number
     * and tags.
     */
    FlowLayout *m_layoutHeader;

    /* The checkbox in the header. */
    QCheckBox *m_checkBoxAdd;

    /* The label that shows this term definition's number. */
    QLabel *m_labelNumber;

    /* The label showing the glossary entry of the term definition. */
    QLabel *m_labelGlossary;
};

#endif // GLOSSARYWIDGET_H