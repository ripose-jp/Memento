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

#ifndef CARDBUILDER_H
#define CARDBUILDER_H

#include <QWidget>

#include <QJsonObject>

namespace Ui
{
    class CardBuilder;
}

class CardBuilder : public QWidget
{
    Q_OBJECT

public:
    CardBuilder(QWidget *parent = nullptr);
    ~CardBuilder();

    void setDecks (const QStringList &decks,  const QString &set = QString());
    void setModels(const QStringList &models, const QString &set = QString());
    void setFields(const QStringList &fields);
    void setFields(const QJsonObject &fields);

    QString     getDeckText()  const;
    QString     getModelText() const;
    QJsonObject getFields()    const;

    void setDeckCurrentText (const QString &text);
    void setModelCurrentText(const QString &text);

Q_SIGNALS:
    void deckTextChanged (const QString &text);
    void modelTextChanged(const QString &text);

private:
    Ui::CardBuilder *m_ui;
};

#endif // CARDBUILDER_H