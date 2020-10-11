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

#include "definition.h"
#include "ui_definition.h"

#define ALT_KANJI_TEXT "Alternative Kanji: "
#define ALT_KANA_TEXT "Alternative Kana: "

Definition::Definition(const Entry *entry, QWidget *parent) 
    : QWidget(parent), m_ui(new Ui::Definition), m_entry(entry)
{
    m_ui->setupUi(this);
    setEntry(entry);
}

Definition::~Definition()
{
    delete m_ui;
    delete m_entry;
}

void Definition::setEntry(const Entry *entry)
{
    if (entry->m_kanji->isEmpty())
    {
        m_ui->m_labelKana->hide();
        m_ui->m_labelKanji->setText(*entry->m_kana);
        m_ui->m_labelJisho->setText(generateJishoLink(*entry->m_kana));
    }
    else
    {
        m_ui->m_labelKana->show();
        m_ui->m_labelKana->setText(*entry->m_kana);
        m_ui->m_labelKanji->setText(*entry->m_kanji);
        m_ui->m_labelJisho->setText(generateJishoLink(*entry->m_kanji));
    }

    m_ui->m_labelDefinition->setText(buildDefinition(*entry->m_descriptions));

    if (entry->m_altkanji->isEmpty())
    {
        m_ui->m_labelAltKanji->hide();
    }
    else
    {
        m_ui->m_labelAltKanji->show();
        m_ui->m_labelAltKanji->setText(ALT_KANJI_TEXT + *entry->m_altkanji);
    }

    if (entry->m_altkana->isEmpty())
    {
        m_ui->m_labelAltKana->hide();
    }
    else
    {
        m_ui->m_labelAltKana->show();
        m_ui->m_labelAltKana->setText(ALT_KANA_TEXT + *entry->m_altkana);
    }

    delete entry;
}

QString Definition::generateJishoLink(const QString &word)
{
    return QString("<a href=\"https://jisho.org/search/%1\">"
                   "Jisho</a>").arg(word);
}

QString Definition::buildDefinition(const QList<QList<QString>> &definitions)
{
    QString defStr;
    for (uint i = 0; i < definitions.size(); ++i)
    {
        if (i)
            defStr.push_back('\n');
        
        defStr += QString::number(i + 1);
        defStr += ") ";
        defStr.push_back('(');
        defStr += definitions[i].front();
        defStr.push_back(')');
        for (uint j = 1; j < definitions[i].size(); ++j)
        {
            defStr.push_back('\n');
            defStr += "- ";
            defStr += definitions[i][j];
        }
    }
    return defStr;
}