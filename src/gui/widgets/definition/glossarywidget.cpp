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

#include "util/utils.h"

GlossaryWidget::GlossaryWidget(
    const size_t number,
    const TermDefinition &def,
    const Qt::KeyboardModifier modifier,
    const Constants::GlossaryStyle style,
    QWidget *parent)
    : QWidget(parent),
      m_def(def)
{
    m_parentLayout  = new QVBoxLayout(this);
    m_layoutHeader  = new FlowLayout(-1, 6);
    m_checkBoxAdd   = new QCheckBox;
    m_labelNumber   = new QLabel;
    m_glossaryLabel = new GlossaryLabel(modifier, style);

    m_parentLayout->setContentsMargins(0, 0, 0, 0);
    m_parentLayout->addLayout(m_layoutHeader);
    m_parentLayout->addWidget(m_glossaryLabel);

    m_layoutHeader->addWidget(m_checkBoxAdd);
    m_layoutHeader->addWidget(m_labelNumber);
    for (const Tag &tag : m_def.tags)
    {
        m_layoutHeader->addWidget(new TagWidget(tag));
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

    m_glossaryLabel->setContents(
        m_def.glossary,
        DirectoryUtils::getDictionaryResourceDir() + SLASH + m_def.dictionary
    );
    connect(
        m_glossaryLabel, &GlossaryLabel::contentSearched,
        this, &GlossaryWidget::contentSearched
    );
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
