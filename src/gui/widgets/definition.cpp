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

#include "../../util/iconfactory.h"

#include <QMessageBox>

#define ALT_KANJI_TEXT QString("Alternative Kanji: " \
    "<font face='font-family: \"Noto Sans CJK JP\", sans-serif;'>%1</font>")
#define ALT_KANA_TEXT QString("Alternative Kana: "\
    "<font face='font-family: \"Noto Sans CJK JP\", sans-serif;'>%1</font>")

Definition::Definition(const Entry *entry, AnkiClient *client, QWidget *parent) 
    : QWidget(parent), m_ui(new Ui::Definition), m_entry(entry),
      m_client(client)
{
    m_ui->setupUi(this);

    IconFactory *factory = IconFactory::create(this);
    m_ui->m_buttonAddCard->setIcon(factory->getIcon(IconFactory::Icon::plus));
    delete factory;

    setEntry(entry);

    connect(m_ui->m_buttonAddCard, &QToolButton::clicked,
        this, &Definition::addNote);
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
        m_ui->m_labelAltKanji->setText(ALT_KANJI_TEXT.arg(*entry->m_altkanji));
    }

    if (entry->m_altkana->isEmpty())
    {
        m_ui->m_labelAltKana->hide();
    }
    else
    {
        m_ui->m_labelAltKana->show();
        m_ui->m_labelAltKana->setText(ALT_KANA_TEXT.arg(*entry->m_altkana));
    }
}

QString Definition::generateJishoLink(const QString &word)
{
    return QString("<a href=\"https://jisho.org/search/%1\">"
                   "Jisho</a>").arg(word);
}

QString Definition::buildDefinition(const QList<QList<QString>> &definitions)
{
    QString defStr;
    for (size_t i = 0; i < definitions.size(); ++i)
    {
        if (i)
            defStr.push_back('\n');
        
        defStr += QString::number(i + 1);
        defStr += ") ";
        defStr.push_back('(');
        defStr += definitions[i].front();
        defStr.push_back(')');
        for (size_t j = 1; j < definitions[i].size(); ++j)
        {
            defStr.push_back('\n');
            defStr += "- ";
            defStr += definitions[i][j];
        }
    }
    return defStr;
}

void Definition::setAddable(bool value)
{
    m_ui->m_buttonAddCard->setEnabled(value);
}

void Definition::addNote()
{
    m_ui->m_buttonAddCard->setEnabled(false);
    m_client->addEntry([=](const int id, const QString &error) {
        if (!error.isEmpty())
        {
            QMessageBox messageBox;
            messageBox.critical(0,"Error", error);
        }
    }, m_entry);
}
