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

#include "definitionwidget.h"
#include "ui_definitionwidget.h"

#define ALT_KANJI_TEXT "Alternative Kanji: "
#define ALT_KANA_TEXT "Alternative Kana: "

DefinitionWidget::DefinitionWidget(QWidget *parent) : QWidget(parent), m_ui(new Ui::DefinitionWidget), m_entry(0)
{
    m_ui->setupUi(this);
    hide();
}

DefinitionWidget::~DefinitionWidget()
{
    delete m_ui;
    delete m_entry;
}

void DefinitionWidget::setEntry(const Entry *entry)
{
    delete m_entry;
    m_entry = entry;

    if (m_entry->m_kanji->empty())
    {
        m_ui->m_labelKana->hide();
        m_ui->m_labelKanji->setText(QString::fromStdString(*m_entry->m_kana));
        m_ui->m_labelJisho->setText(generateJishoLink(QString::fromStdString(*m_entry->m_kana)));
    }
    else
    {
        m_ui->m_labelKana->show();
        m_ui->m_labelKana->setText(QString::fromStdString(*m_entry->m_kana));
        m_ui->m_labelKanji->setText(QString::fromStdString(*m_entry->m_kanji));
        m_ui->m_labelJisho->setText(generateJishoLink(QString::fromStdString(*m_entry->m_kanji)));
    }

    m_ui->m_labelDefinition->setText(buildDefinition(*m_entry->m_descriptions));

    if (m_entry->m_altkanji->empty())
    {
        m_ui->m_labelAltKanji->hide();
    }
    else
    {
        m_ui->m_labelAltKanji->show();
        m_ui->m_labelAltKanji->setText(QString::fromStdString(ALT_KANJI_TEXT + *m_entry->m_altkanji));
    }

    if (m_entry->m_altkana->empty())
    {
        m_ui->m_labelAltKana->hide();
    }
    else
    {
        m_ui->m_labelAltKana->show();
        m_ui->m_labelAltKana->setText(QString::fromStdString(ALT_KANA_TEXT + *m_entry->m_altkana));
    }

    show();
}

QString DefinitionWidget::generateJishoLink(const QString &word)
{
    return QString("<a href=\"https://jisho.org/search/%1\">Jisho</a>").arg(word);
}

QString DefinitionWidget::buildDefinition(const std::vector<std::vector<std::string>> &definitions)
{
    QString defStr;
    for (uint i = 0; i < definitions.size(); ++i)
    {
        if (i)
            defStr.push_back('\n');
        
        defStr += QString::number(i + 1);
        defStr += ") ";
        defStr.push_back('(');
        defStr += QString::fromStdString(definitions[i].front());
        defStr.push_back(')');
        for (uint j = 1; j < definitions[i].size(); ++j)
        {
            defStr.push_back('\n');
            defStr += "- ";
            defStr += QString::fromStdString(definitions[i][j]);
        }
    }
    return defStr;
}

void DefinitionWidget::leaveEvent(QEvent *event)
{
    hide();
    Q_EMIT definitionHidden();
}