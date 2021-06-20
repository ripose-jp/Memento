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

#include "cardbuilder.h"
#include "ui_cardbuilder.h"

CardBuilder::CardBuilder(QWidget *parent)
    : QWidget(parent), m_ui(new Ui::CardBuilder)
{
    m_ui->setupUi(this);

    connect(m_ui->comboBoxDeck,  &QComboBox::currentTextChanged, this, &CardBuilder::deckTextChanged);
    connect(m_ui->comboBoxModel, &QComboBox::currentTextChanged, this, &CardBuilder::modelTextChanged);
}

CardBuilder::~CardBuilder()
{
    delete m_ui;
}

void CardBuilder::setDecks(const QStringList &decks, const QString &set)
{
    m_ui->comboBoxDeck->clear();
    m_ui->comboBoxDeck->addItems(decks);
    m_ui->comboBoxDeck->setCurrentText(set);
    m_ui->comboBoxDeck->model()->sort(0);
}

void CardBuilder::setModels(const QStringList &models, const QString &set)
{
    m_ui->comboBoxModel->clear();
    m_ui->comboBoxModel->addItems(models);
    m_ui->comboBoxModel->setCurrentText(set);
    m_ui->comboBoxModel->model()->sort(0);
}

void CardBuilder::setFields(const QStringList &fields)
{
    m_ui->tableFields->setRowCount(fields.size());
    for (size_t i = 0; i < fields.size(); ++i)
    {
        QTableWidgetItem *item = new QTableWidgetItem(fields[i]);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        m_ui->tableFields->setItem(i, 0, item);
    }
    for (size_t i = 0; i < fields.size(); ++i)
    {
        QTableWidgetItem *item = new QTableWidgetItem("");
        m_ui->tableFields->setItem(i, 1, item);
    }
}

void CardBuilder::setFields(const QJsonObject &fields)
{
    QStringList keys = fields.keys();
    m_ui->tableFields->setRowCount(fields.size());
    for (size_t i = 0; i < fields.size(); ++i)
    {
        QTableWidgetItem *item = new QTableWidgetItem(keys[i]);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        m_ui->tableFields->setItem(i, 0, item);
    }
    for (size_t i = 0; i < fields.size(); ++i)
    {
        QString itemText;
        if (!fields[keys[i]].isUndefined() && !fields[keys[i]].isNull()) 
        {
            itemText = fields[keys[i]].toString();
        }
        QTableWidgetItem *item = new QTableWidgetItem(itemText);
        m_ui->tableFields->setItem(i, 1, item);
    }
}

QString CardBuilder::getDeckText() const
{
    return m_ui->comboBoxDeck->currentText();
}

QString CardBuilder::getModelText() const
{
    return m_ui->comboBoxModel->currentText();
}

QJsonObject CardBuilder::getFields() const
{
    QJsonObject fields;
    for (size_t i = 0; i < m_ui->tableFields->rowCount(); ++i)
    {
        QTableWidgetItem *field = m_ui->tableFields->item(i, 0);
        QTableWidgetItem *value = m_ui->tableFields->item(i, 1);
        fields[field->text()] = value->text();
    }
    return fields;
}

void CardBuilder::setDeckCurrentText(const QString &text)
{
    m_ui->comboBoxDeck->setCurrentText(text);
}

void CardBuilder::setModelCurrentText(const QString &text)
{
    m_ui->comboBoxModel->setCurrentText(text);
}
