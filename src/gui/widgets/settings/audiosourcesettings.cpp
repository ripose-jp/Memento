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

#include "audiosourcesettings.h"
#include "ui_audiosourcesettings.h"

#include <QComboBox>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>

#include "util/constants.h"
#include "util/globalmediator.h"
#include "util/iconfactory.h"

/* Column indices */
#define COL_NAME    0
#define COL_URL     1
#define COL_TYPE    2
#define COL_MD5     3

/* Combo Box Type Names */
#define TYPE_COMBO_BOX_FILE "File"
#define TYPE_COMBO_BOX_JSON "JSON"

/* Begin Constructor/Destructors */

AudioSourceSettings::AudioSourceSettings(QWidget *parent)
    : QWidget(parent),
      m_ui(new Ui::AudioSourceSettings)
{
    m_ui->setupUi(this);

    m_ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    IconFactory *factory = IconFactory::create();
    m_ui->buttonUp->setIcon(factory->getIcon(IconFactory::Icon::up));
    m_ui->buttonDown->setIcon(factory->getIcon(IconFactory::Icon::down));

    /* Table Actions */
    connect(
        m_ui->table, &QTableWidget::cellChanged,
        this,        &AudioSourceSettings::updateRows
    );
    connect(
        m_ui->table, &QTableWidget::currentCellChanged,
        this,        &AudioSourceSettings::updateButtons
    );
    connect(
        m_ui->table, &QTableWidget::cellChanged,
        this,        &AudioSourceSettings::updateButtons
    );

    /* Up/Down Buttons */
    connect(
        m_ui->buttonUp, &QToolButton::clicked,
        this,           &AudioSourceSettings::moveUp
    );
    connect(
        m_ui->buttonDown, &QToolButton::clicked,
        this,             &AudioSourceSettings::moveDown
    );

    /* Dialog Buttons */
    connect(
        m_ui->buttonBox->button(QDialogButtonBox::StandardButton::Apply),
        &QPushButton::clicked,
        this,
        &AudioSourceSettings::applyChanges
    );
    connect(
        m_ui->buttonBox->button(
            QDialogButtonBox::StandardButton::RestoreDefaults
        ),
        &QPushButton::clicked,
        this,
        &AudioSourceSettings::restoreDefaults
    );
    connect(
        m_ui->buttonBox->button(QDialogButtonBox::StandardButton::Reset),
        &QPushButton::clicked,
        this,
        &AudioSourceSettings::restoreSaved
    );
    connect(
        m_ui->buttonBox->button(QDialogButtonBox::StandardButton::Help),
        &QPushButton::clicked,
        this,
        &AudioSourceSettings::showHelp
    );

    /* Make sure at least the default are saved to settings */
    restoreSaved();
    applyChanges();
}

AudioSourceSettings::~AudioSourceSettings()
{
    delete m_ui;
}

/* End Constructor/Destructors */
/* Begin Event Handlers */

void AudioSourceSettings::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    restoreSaved();
    m_ui->buttonDown->setEnabled(false);
    m_ui->buttonUp->setEnabled(false);
}

/* End Event Handlers */
/* Begin Button Box Handlers */

void AudioSourceSettings::applyChanges()
{
    QString error = verifyNames();
    if (!error.isEmpty())
    {
        QMessageBox::critical(
            this, "Audio Source Error",
            "Could not apply changes:\n" + error
        );
        return;
    }

    QTableWidget *table = m_ui->table;
    if (table->rowCount() == 1)
    {
        restoreDefaults();
    }

    QSettings settings;
    settings.remove(Constants::Settings::AudioSource::GROUP);
    settings.beginWriteArray(Constants::Settings::AudioSource::GROUP);
    for (int i = 0; i < table->rowCount() - 1; ++i)
    {
        settings.setArrayIndex(i);

        QString name =
            itemEmpty(i, COL_NAME) ? "" : table->item(i, COL_NAME)->text();
        QString url =
            itemEmpty(i, COL_URL) ? "" : table->item(i, COL_URL)->text();
        QComboBox *comboType = ((QComboBox *)table->cellWidget(i, COL_TYPE));
        int type = comboType ?
            comboType->currentData().toInt() :
            static_cast<int>(Constants::AudioSourceType::File);
        QString md5 =
            itemEmpty(i, COL_MD5)  ? "" : table->item(i, COL_MD5)->text();

        settings.setValue(Constants::Settings::AudioSource::NAME, name);
        settings.setValue(Constants::Settings::AudioSource::URL, url);
        settings.setValue(Constants::Settings::AudioSource::TYPE, (int)type);
        settings.setValue(Constants::Settings::AudioSource::MD5, md5);
    }
    settings.endArray();

    Q_EMIT GlobalMediator::getGlobalMediator()->audioSourceSettingsChanged();
}

void AudioSourceSettings::restoreDefaults()
{
    QTableWidget *table = m_ui->table;
    table->clearContents();
    table->setRowCount(2);

    table->setItem(
        0,
        COL_NAME,
        new QTableWidgetItem(Constants::Settings::AudioSource::NAME_DEFAULT)
    );
    table->setItem(
        0,
        COL_URL,
        new QTableWidgetItem(Constants::Settings::AudioSource::URL_DEFAULT)
    );
    table->setCellWidget(
        0,
        COL_TYPE,
        createTypeComboBox(Constants::Settings::AudioSource::TYPE_DEFAULT)
    );
    table->setItem(
        0,
        COL_MD5,
        new QTableWidgetItem(Constants::Settings::AudioSource::MD5_DEFAULT)
    );

    table->setCellWidget(
        1,
        COL_TYPE,
        createTypeComboBox(Constants::Settings::AudioSource::TYPE_DEFAULT)
    );
}

void AudioSourceSettings::restoreSaved()
{
    QSettings settings;
    size_t size = settings.beginReadArray(
        Constants::Settings::AudioSource::GROUP
    );
    if (size == 0)
    {
        restoreDefaults();
        return;
    }

    QTableWidget *table = m_ui->table;
    table->clearContents();
    table->setRowCount(size + 1);
    for (size_t i = 0; i < size; ++i)
    {
        settings.setArrayIndex(i);

        table->setItem(
            i,
            COL_NAME,
            new QTableWidgetItem(
                settings.value(
                    Constants::Settings::AudioSource::NAME,
                    Constants::Settings::AudioSource::NAME_DEFAULT
                ).toString()
            )
        );
        table->setItem(
            i,
            COL_URL,
            new QTableWidgetItem(
                settings.value(
                    Constants::Settings::AudioSource::URL,
                    Constants::Settings::AudioSource::URL_DEFAULT
                ).toString()
            )
        );
        table->setCellWidget(
            i,
            COL_TYPE,
            createTypeComboBox(
                static_cast<Constants::AudioSourceType>(settings.value(
                    Constants::Settings::AudioSource::TYPE,
                    static_cast<int>(
                        Constants::Settings::AudioSource::TYPE_DEFAULT
                    )
                ).toInt())
            )
        );
        table->setItem(
            i,
            COL_MD5,
            new QTableWidgetItem(
                settings.value(
                    Constants::Settings::AudioSource::MD5,
                    Constants::Settings::AudioSource::MD5_DEFAULT
                ).toString()
            )
        );
    }
    settings.endArray();

    table->setCellWidget(
        size,
        COL_TYPE,
        createTypeComboBox(Constants::Settings::AudioSource::TYPE_DEFAULT)
    );
}

void AudioSourceSettings::showHelp()
{
    QMessageBox::information(this, "Audio Source Help",
        "<b>Source Name</b>: The name of the audio source as it will appear in Memento."
        "<br><br>"
        "<b>URL</b>: The URL of the audio source. "
            "Supports inserting {expression} and {reading} markers into the URL. "
            "See Anki Integration Help for more information."
        "<br><br>"
        "<b>MD5 Skip Hash</b>: Audio that matches this MD5 hash will be ignored."
    );
}

/* End Button Box Handlers */
/* Begin Move Methods */

void AudioSourceSettings::moveRow(const int row, const int step)
{
    QTableWidget *table = m_ui->table;

    const bool signalsBlocked = table->signalsBlocked();
    table->blockSignals(true);

    QTableWidgetItem *name_1 = table->takeItem(row, COL_NAME);
    QTableWidgetItem *url_1 = table->takeItem(row, COL_URL);
    int type_1 =
        ((QComboBox *)table->cellWidget(row, COL_TYPE))->currentIndex();
    QTableWidgetItem *md5_1 = table->takeItem(row, COL_MD5);

    const int newRow = row + step;
    QTableWidgetItem *name_2 = table->takeItem(newRow, COL_NAME);
    QTableWidgetItem *url_2 = table->takeItem(newRow, COL_URL);
    int type_2 =
        ((QComboBox *)table->cellWidget(newRow, COL_TYPE))->currentIndex();
    QTableWidgetItem *md5_2 = table->takeItem(newRow, COL_MD5);

    table->setItem(newRow, COL_NAME, name_1);
    table->setItem(newRow, COL_URL, url_1);
    ((QComboBox *)table->cellWidget(newRow, COL_TYPE))->setCurrentIndex(type_1);
    table->setItem(newRow, COL_MD5, md5_1);

    table->setItem(row, COL_NAME, name_2);
    table->setItem(row, COL_URL, url_2);
    ((QComboBox *)table->cellWidget(row, COL_TYPE))->setCurrentIndex(type_2);
    table->setItem(row, COL_MD5, md5_2);

    table->blockSignals(signalsBlocked);
}

void AudioSourceSettings::moveUp()
{
    moveRow(m_ui->table->currentRow(), -1);
    m_ui->table->selectRow(m_ui->table->currentRow() - 1);
}

void AudioSourceSettings::moveDown()
{
    moveRow(m_ui->table->currentRow(), 1);
    m_ui->table->selectRow(m_ui->table->currentRow() + 1);
}

/* End Move Methods */
/* Begin Updaters */

void AudioSourceSettings::updateButtons()
{
    const int currentRow = m_ui->table->currentRow();
    const int rowCount = m_ui->table->rowCount();

    m_ui->buttonUp->setEnabled(0 < currentRow && currentRow < rowCount - 1);
    m_ui->buttonDown->setEnabled(currentRow < rowCount - 2);
}

void AudioSourceSettings::updateRows()
{
    /* Remove empty rows */
    QTableWidget *table = m_ui->table;
    for (int i = 0; i < table->rowCount() - 1; ++i)
    {
        if (rowEmpty(i))
        {
            table->removeRow(i--);
        }
    }

    /* Add an empty row at the end if needed */
    size_t lastRow = table->rowCount() - 1;
    if (!rowEmpty(lastRow))
    {
        table->insertRow(lastRow + 1);
        table->setCellWidget(
            lastRow + 1,
            COL_TYPE,
            createTypeComboBox(Constants::AudioSourceType::File)
        );
    }
}

/* End Updaters */
/* Begin Helper Methods */

QString AudioSourceSettings::verifyNames() const
{
    QSet<QString> names;
    QTableWidget *table = m_ui->table;
    for (int i = 0; i < table->rowCount() - 1; ++i)
    {
        if (itemEmpty(i, COL_NAME))
        {
            return QString("Missing name at row %1.").arg(i + 1);
        }

        QString name = table->item(i, COL_NAME)->text();
        if (names.contains(name))
        {
            return QString("Duplicate name at row %1.").arg(i + 1);
        }
        names.insert(name);
    }

    return "";
}

inline bool AudioSourceSettings::itemEmpty(const int row, const int col) const
{
    QTableWidgetItem *item = m_ui->table->item(row, col);
    return item == nullptr || item->text().isEmpty();
}

inline bool AudioSourceSettings::rowEmpty(const int row) const
{
    for (int i = 0; i < m_ui->table->columnCount(); ++i)
    {
        if (!itemEmpty(row, i))
        {
            return false;
        }
    }
    return true;
}

inline QComboBox *AudioSourceSettings::createTypeComboBox(
    Constants::AudioSourceType type) const
{
    switch(type)
    {
    case Constants::AudioSourceType::File:
        return createTypeComboBox(TYPE_COMBO_BOX_FILE);
    case Constants::AudioSourceType::JSON:
        return createTypeComboBox(TYPE_COMBO_BOX_JSON);
    }
    return createTypeComboBox(TYPE_COMBO_BOX_FILE);
}

inline QComboBox *AudioSourceSettings::createTypeComboBox(
    const QString &setting) const
{
    QComboBox *box = new QComboBox;
    box->addItem(
        TYPE_COMBO_BOX_FILE,
        QVariant(static_cast<int>(Constants::AudioSourceType::File))
    );
    box->addItem(
        TYPE_COMBO_BOX_JSON,
        QVariant(QVariant(static_cast<int>(Constants::AudioSourceType::JSON)))
    );
    box->setCurrentText(setting);
    return box;
}

/* End Helper Methods */
