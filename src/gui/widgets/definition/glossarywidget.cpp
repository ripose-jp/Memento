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

#include "glossarywidget.h"

#include "tagwidget.h"

GlossaryWidget::GlossaryWidget(const size_t          number,
                               const TermDefinition &def,
                               QWidget              *parent)
    : QWidget(parent),
      m_def(def)
{
    m_parentLayout  = new QVBoxLayout(this);
    m_layoutHeader  = new FlowLayout(-1, 6);
    m_checkBoxAdd   = new QCheckBox;
    m_labelNumber   = new QLabel;
    m_labelGlossary = new QLabel;

    m_parentLayout->setMargin(0);
    m_parentLayout->addLayout(m_layoutHeader);
    m_parentLayout->addWidget(m_labelGlossary);

    m_layoutHeader->addWidget(m_checkBoxAdd);
    m_layoutHeader->addWidget(m_labelNumber);
    for (const Tag &tag : m_def.tags)
    {
        m_layoutHeader->addWidget(new TagWidget(tag));
    }
    for (const Tag &rule : m_def.rules)
    {
        if (m_def.tags.contains(rule))
        {
            continue;
        }
        m_layoutHeader->addWidget(new TagWidget(rule));
    }
    m_layoutHeader->addWidget(new TagWidget(m_def.dictionary));

    m_checkBoxAdd->setChecked(true);
#if defined(Q_OS_MACOS)
    m_checkBoxAdd->setText(" ");
#else
    m_checkBoxAdd->setText("");
#endif
    m_checkBoxAdd->setToolTip("Add this entry to an Anki note");
    m_checkBoxAdd->hide();

    m_labelNumber->setText(QString::number(number) + ".");

    m_labelGlossary->setWordWrap(true);
    m_labelGlossary->setTextInteractionFlags(
        Qt::TextInteractionFlag::TextSelectableByMouse
    );
    QString glos;
    glos += "<ul>";
    for (QString def : m_def.glossary)
    {
        glos += "<li>";
        glos += def.replace('\n', "</li><li>");
        glos += "</li>";
    }
    glos += "</ul>";
    m_labelGlossary->setText(glos);
}

void GlossaryWidget::setCheckable(const bool value)
{
    m_checkBoxAdd->setVisible(value);
}

void GlossaryWidget::setChecked(const bool value)
{
    m_checkBoxAdd->setChecked(value);
}

bool GlossaryWidget::isChecked() const
{
    return m_checkBoxAdd->isChecked();
}