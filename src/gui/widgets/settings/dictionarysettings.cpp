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

#include <QFileDialog>
#include <QPushButton>
#include <QSettings>
#include <QThreadPool>

#include "dict/dictionary.h"
#include "util/constants.h"
#include "util/globalmediator.h"
#include "util/iconfactory.h"

/* Begin Constructor/Destructor */

DictionarySettings::DictionarySettings(QWidget *parent)
 : QWidget(parent),
   m_ui(new Ui::DictionarySettings)
{
    m_ui->setupUi(this);

    initIcons();

    connect(
        m_ui->buttonBox->button(QDialogButtonBox::StandardButton::Apply),
        &QPushButton::clicked,
        this,
        &DictionarySettings::applySettings
    );
    connect(
        m_ui->buttonBox->button(QDialogButtonBox::StandardButton::Reset),
        &QPushButton::clicked,
        this,
        &DictionarySettings::restoreSaved
    );

    connect(
        m_ui->listDictionaries, &QListWidget::currentRowChanged,
        this,                   [=] (int row) { setButtonsEnabled(row != -1); }
    );

    connect(
        m_ui->buttonUp, &QToolButton::clicked,
        this,           &DictionarySettings::moveUp
    );
    connect(
        m_ui->buttonDown, &QToolButton::clicked,
        this,             &DictionarySettings::moveDown
    );
    connect(
        m_ui->buttonAdd, &QToolButton::clicked,
        this,            &DictionarySettings::addDictionary
    );
    connect(
        m_ui->buttonDelete, &QToolButton::clicked,
        this,               &DictionarySettings::deleteDictionary
    );

    connect(
        this, &DictionarySettings::restoreSavedReady,
        this, &DictionarySettings::restoreSavedHelper
    );

    GlobalMediator *mediator = GlobalMediator::getGlobalMediator();
    connect(
        mediator, &GlobalMediator::dictionariesChanged,
        this,     &DictionarySettings::restoreSaved
    );
    connect(
        mediator, &GlobalMediator::requestThemeRefresh,
        this,     &DictionarySettings::initIcons
    );
}

DictionarySettings::~DictionarySettings()
{
    disconnect();
    delete m_ui;
}

/* End Constructor/Destructor */
/* Begin Initializers */

void DictionarySettings::initIcons()
{
    IconFactory *factory = IconFactory::create();
    m_ui->buttonAdd->   setIcon(factory->getIcon(IconFactory::Icon::plus));
    m_ui->buttonDelete->setIcon(factory->getIcon(IconFactory::Icon::minus));
    m_ui->buttonUp->    setIcon(factory->getIcon(IconFactory::Icon::up));
    m_ui->buttonDown->  setIcon(factory->getIcon(IconFactory::Icon::down));
}

/* End Initializers */
/* Begin Event Handlers */

void DictionarySettings::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    restoreSaved();
}

void DictionarySettings::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);
    setButtonsEnabled(false);
}

/* End Event Handlers */
/* Begin Button Box Handlers */

void DictionarySettings::restoreSaved()
{
    if (!m_restoreSavedActive.tryLock())
    {
        return;
    }

    setEnabled(false);

    QThreadPool::globalInstance()->start(
        [this] {
            Dictionary *dict =
                GlobalMediator::getGlobalMediator()->getDictionary();
            QStringList dicts = dict->getDictionaries();
            QStringList disabledNames = dict->getDisabledDictionaries();

            Q_EMIT restoreSavedReady(dicts, disabledNames);
        }
    );
}

void DictionarySettings::restoreSavedHelper(
    const QStringList &dicts,
    const QStringList &disabledNames)
{
    m_ui->listDictionaries->clear();
    m_ui->listDictionaries->addItems(dicts);

    QSet<QString> disabled(disabledNames.begin(), disabledNames.end());
    for (int i = 0; i < m_ui->listDictionaries->count(); ++i)
    {
        QListWidgetItem *item = m_ui->listDictionaries->item(i);
        item->setFlags(item->flags() | Qt::ItemFlag::ItemIsUserCheckable);
        item->setCheckState(
            disabled.contains(item->text()) ? Qt::Unchecked : Qt::Checked
        );
    }

    setEnabled(true);
    m_restoreSavedActive.unlock();
}

void DictionarySettings::applySettings()
{
    QStringList dictionaries;

    QSettings settings;
    settings.beginGroup(Constants::Settings::Dictionaries::GROUP);
    settings.remove("");
    for (int i = 0; i < m_ui->listDictionaries->count(); ++i)
    {
        QListWidgetItem *item = m_ui->listDictionaries->item(i);
        if (item->checkState() == Qt::Unchecked)
        {
            dictionaries << item->text();
        }
        settings.setValue(item->text(), i);
    }
    settings.endGroup();

    Dictionary *dict = GlobalMediator::getGlobalMediator()->getDictionary();
    dict->disableDictionaries(dictionaries);

    Q_EMIT GlobalMediator::getGlobalMediator()->dictionaryOrderChanged();
}

/* End Button Box Handlers */
/* Begin Move Button Handlers */

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

/* End Move Button Handlers */
/* Begin Add/Sub Buttons */

void DictionarySettings::addDictionary()
{
    QStringList files = QFileDialog::getOpenFileNames(
        0, "Open Dictionaries", QString(), "Dictionaries (*.zip);;All Files (*)"
    );
    if (files.isEmpty())
    {
        return;
    }

    QThreadPool::globalInstance()->start(
        [this, files] {
            setEnabled(false);
            Dictionary *dic =
                GlobalMediator::getGlobalMediator()->getDictionary();
            QString err = dic->addDictionary(files);
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
    {
        return;
    }

    QListWidgetItem *item = m_ui->listDictionaries->takeItem(
        m_ui->listDictionaries->currentRow()
    );
    QSettings settings;
    settings.beginGroup(Constants::Settings::Dictionaries::GROUP);
    settings.remove(item->text());
    settings.endGroup();
    QThreadPool::globalInstance()->start(
        [=] {
            setEnabled(false);
            Dictionary *dic =
                GlobalMediator::getGlobalMediator()->getDictionary();
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

/* End Add/Sub Buttons */
/* Begin Helpers */

void DictionarySettings::setButtonsEnabled(const bool value)
{
    m_ui->buttonDelete->setEnabled(value);
    m_ui->buttonDown  ->setEnabled(value);
    m_ui->buttonUp    ->setEnabled(value);
}

/* End Helpers */
