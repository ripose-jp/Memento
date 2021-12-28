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

#include "searchwidget.h"

#include <QLineEdit>
#include <QThreadPool>
#include <QVBoxLayout>

#include "../../dict/dictionary.h"
#include "../../util/globalmediator.h"
#include "definition/definitionwidget.h"

SearchWidget::SearchWidget(QWidget *parent)
    : QWidget(parent),
      m_dictionary(GlobalMediator::getGlobalMediator()->getDictionary())
{
    if (m_dictionary == nullptr)
    {
        m_dictionary = new Dictionary;
    }

    setAutoFillBackground(true);

    m_layoutParent = new QVBoxLayout(this);

    m_lineEditSearch = new QLineEdit;
    m_lineEditSearch->setPlaceholderText("Search");
    m_layoutParent->addWidget(m_lineEditSearch);

    m_definition = new DefinitionWidget;
    m_definition->layout()->setMargin(0);
    m_definition->setMinimumSize(300, 300);
    m_definition->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    m_layoutParent->addWidget(m_definition);

    connect(
        m_lineEditSearch, &QLineEdit::textEdited,
        this, &SearchWidget::updateSearch
    );
    connect(
        this, &SearchWidget::termsUpdated,
        m_definition, &DefinitionWidget::setTerms
    );
    connect(
        this, &SearchWidget::requestClear,
        m_definition, &DefinitionWidget::clearTerms
    );
    connect(
        GlobalMediator::getGlobalMediator(),
        &GlobalMediator::interfaceSettingsChanged,
        this, [=] { updateSearch(m_lineEditSearch->text()); }
    );
}

/* Prevents large searches from being executed and freezing everything up */
#define MAX_SEARCH_SIZE 40

void SearchWidget::updateSearch(const QString text)
{
    QThreadPool::globalInstance()->start(
        [=] {
            QList<Term *> *terms =
                m_dictionary->searchTerms(text.left(MAX_SEARCH_SIZE));
            if (terms == nullptr)
            {
                Q_EMIT requestClear();
            }
            else if (terms->isEmpty())
            {
                delete terms;
                Q_EMIT requestClear();
            }
            else
            {
                Q_EMIT termsUpdated(terms);
            }
        }
    );
}

#undef MAX_SEARCH_SIZE
