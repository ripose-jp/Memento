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

#include "aboutwindow.h"

#include <QVBoxLayout>
#include <QLabel>

#include "util/globalmediator.h"
#include "version.h"

AboutWindow::AboutWindow(QWidget *parent) : QDialog(parent)
{
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    setWindowTitle("About Memento");

    QVBoxLayout *parentLayout = new QVBoxLayout(this);
    parentLayout->setContentsMargins(20, 20, 20, 20);

    QLabel *labelImage = new QLabel;
    labelImage->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    labelImage->setPixmap(QPixmap(":/memento.svg"));
    parentLayout->addWidget(labelImage);

    QLabel *labelInfo = new QLabel;
    labelInfo->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    labelInfo->setOpenExternalLinks(true);
    labelInfo->setTextInteractionFlags(
        Qt::LinksAccessibleByKeyboard | Qt::LinksAccessibleByMouse
    );
    QString version{Memento::VERSION};
    QString versionHash{Memento::VERSION_HASH};
    if (!versionHash.isEmpty())
    {
        version += '-';
        version += versionHash;
    }
    labelInfo->setText(
        "<b>Version</b>: " + version + "<br>"
        "<b>License</b>: GPLv2"
        "<br>"
        "<b>Website</b>: "
            "<a href='https://ripose-jp.github.io/Memento'>"
                "https://ripose-jp.github.io/Memento"
            "</a>"
        "<br>"
        "<b>Project Page</b>: "
            "<a href='https://github.com/ripose-jp/memento'>"
                "https://github.com/ripose-jp/memento"
            "</a>"
        "<br>"
        "Copyright Ripose 2020-2024"
    );
    parentLayout->addWidget(labelInfo);
}
