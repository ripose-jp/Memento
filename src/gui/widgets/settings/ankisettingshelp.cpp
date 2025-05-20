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

#include "ankisettingshelp.h"
#include "ui_ankisettingshelp.h"

#include "player/playeradapter.h"

AnkiSettingsHelp::AnkiSettingsHelp(Context *context, QWidget *parent) :
    QDialog(parent),
    m_ui(std::make_unique<Ui::AnkiSettingsHelp>()),
    m_context(std::move(context))
{
    m_ui->setupUi(this);
    resize(minimumSizeHint());
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

    /* Decide whether or not to show secondary subtitle properties */
    PlayerAdapter *player = m_context->getPlayerAdapter();
    if (!player->canGetSecondarySubText())
    {
        m_ui->labelTermSentence2->hide();
        m_ui->labelTermSentence2Ex->hide();

        m_ui->labelTermContext2->hide();
        m_ui->labelTermContext2Ex->hide();

        m_ui->labelKanjiSentence2->hide();
        m_ui->labelKanjiSentence2Ex->hide();

        m_ui->labelKanjiContext2->hide();
        m_ui->labelKanjiContext2Ex->hide();
    }
}

AnkiSettingsHelp::~AnkiSettingsHelp()
{

}
