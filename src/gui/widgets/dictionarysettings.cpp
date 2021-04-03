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

#include "dictionarysettings.h"
#include "ui_dictionarysettings.h"

#include "../../util/globalmediator.h"
#include "../../util/iconfactory.h"
#include "../../util/constants.h"
#include "../../dict/dictionary.h"

#include <QPushButton>
#include <QSettings>
#include <QFileDialog>
#include <QThreadPool>

DictionarySettings::DictionarySettings(QWidget *parent) : QWidget(parent), m_ui(new Ui::DictionarySettings)
{
    m_ui->setupUi(this);

    refreshIcons();

    connect(m_ui->buttonBox->button(QDialogButtonBox::StandardButton::Apply), &QPushButton::clicked,
        this, &DictionarySettings::applySettings);
    connect(m_ui->buttonBox->button(QDialogButtonBox::StandardButton::Reset), &QPushButton::clicked,
        this, &DictionarySettings::refresh);

    connect(m_ui->listDictionaries, &QListWidget::currentRowChanged, this,
        [=] (int row) { setButtonsEnabled(row != -1); } );
    
    connect(m_ui->buttonUp,     &QToolButton::clicked, this, &DictionarySettings::moveUp);
    connect(m_ui->buttonDown,   &QToolButton::clicked, this, &DictionarySettings::moveDown);
    connect(m_ui->buttonAdd,    &QToolButton::clicked, this, &DictionarySettings::addDictionary);
    connect(m_ui->buttonDelete, &QToolButton::clicked, this, &DictionarySettings::deleteDictionary);

    connect(GlobalMediator::getGlobalMediator(), &GlobalMediator::dictionaryAdded,     this, &DictionarySettings::refresh);
    connect(GlobalMediator::getGlobalMediator(), &GlobalMediator::requestThemeRefresh, this, &DictionarySettings::refreshIcons);
}

DictionarySettings::~DictionarySettings()
{
    delete m_ui;
}

void DictionarySettings::refreshIcons()
{
    IconFactory *factory = IconFactory::create();
    m_ui->buttonAdd->   setIcon(factory->getIcon(IconFactory::Icon::plus));
    m_ui->buttonDelete->setIcon(factory->getIcon(IconFactory::Icon::minus));
    m_ui->buttonUp->    setIcon(factory->getIcon(IconFactory::Icon::up));
    m_ui->buttonDown->  setIcon(factory->getIcon(IconFactory::Icon::down));
}

void DictionarySettings::showEvent(QShowEvent *event)
{
    refresh();
}

void DictionarySettings::hideEvent(QHideEvent *event)
{
    setButtonsEnabled(false);
}

void DictionarySettings::refresh()
{
    m_ui->listDictionaries->clear();
    m_ui->listDictionaries->addItems(
        GlobalMediator::getGlobalMediator()->getDictionary()->getDictionaries()
    );
    applySettings();
}

void DictionarySettings::applySettings()
{
    QSettings settings;
    settings.beginGroup(SETTINGS_DICTIONARIES);
    settings.remove("");
    for (int i = 0; i < m_ui->listDictionaries->count(); ++i)
    {
        settings.setValue(m_ui->listDictionaries->item(i)->text(), i);   
    }
    settings.endGroup();
}

void DictionarySettings::setButtonsEnabled(const bool value)
{
    m_ui->buttonDelete->setEnabled(value);
    m_ui->buttonDown->setEnabled(value);
    m_ui->buttonUp->setEnabled(value);
}

void DictionarySettings::moveUp()
{
    int current = m_ui->listDictionaries->currentRow();
    if (current != 0)
    {
        QListWidgetItem *item = m_ui->listDictionaries->takeItem(current);
        m_ui->listDictionaries->insertItem(current - 1, item);
        m_ui->listDictionaries->setCurrentRow(current - 1);
    }
}

void DictionarySettings::moveDown()
{
    int current = m_ui->listDictionaries->currentRow();
    if (current != m_ui->listDictionaries->count() - 1)
    {
        QListWidgetItem *item = m_ui->listDictionaries->takeItem(current);
        m_ui->listDictionaries->insertItem(current + 1, item);
        m_ui->listDictionaries->setCurrentRow(current + 1);
    }
}

void DictionarySettings::addDictionary()
{
    QString file = QFileDialog::getOpenFileName(0, "Open the dictionary");
    if (file.isEmpty())
        return;

    QThreadPool::globalInstance()->start([=]
        {
            setEnabled(false);
            Dictionary *dic = GlobalMediator::getGlobalMediator()->getDictionary();
            QString err = dic->addDictionary(file);
            if (!err.isEmpty())
            {
                Q_EMIT GlobalMediator::getGlobalMediator()->showCritical(
                    "Error adding dictionary", err
                );
            }
            setEnabled(true);
        }
    );
}

void DictionarySettings::deleteDictionary()
{
    if (!m_ui->listDictionaries->count())
        return;
    
    QListWidgetItem *item = m_ui->listDictionaries->takeItem(m_ui->listDictionaries->currentRow());
    QSettings settings;
    settings.beginGroup(SETTINGS_DICTIONARIES);
    settings.remove(item->text());
    settings.endGroup();
    QThreadPool::globalInstance()->start(
        [=] {
            setEnabled(false);
            Dictionary *dic = GlobalMediator::getGlobalMediator()->getDictionary();
            QString err = dic->deleteDictionary(item->text());
            if (!err.isEmpty())
            {
                Q_EMIT GlobalMediator::getGlobalMediator()->showCritical(
                    "Error deleting dictionary", err
                );
            }
            delete item;
            setEnabled(true);
        } 
    );
}
