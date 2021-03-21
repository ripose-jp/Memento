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

#ifndef TERMWIDGET_H
#define TERMWIDGET_H

#include <QWidget>

#include "flowlayout.h"
#include "../../dict/expression.h"
#include "../../anki/ankiclient.h"

namespace Ui
{
    class TermWidget;
}

class TermWidget : public QWidget
{
    Q_OBJECT

public:
    TermWidget(const Term *term, AnkiClient *client, QWidget *parent = 0);
    ~TermWidget();

    void setAddable(bool value);

private Q_SLOTS:
    void addNote();
    void searchKanji(const QString &ch);

private:
    Ui::TermWidget *m_ui;
    const Term     *m_term;
    AnkiClient     *m_client;

    FlowLayout  *m_layoutTermTags;
    FlowLayout  *m_layoutFreqTags;
    QVBoxLayout *m_layoutGlossary;

    void setTerm(const Term &term);
    inline QString generateJishoLink(const QString &word);
    inline bool isKanji(const QString &ch);
};

#endif // TERMWIDGET_H