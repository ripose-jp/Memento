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

#include "pitchwidget.h"

#include "tagwidget.h"

#include <QSet>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>

#define LH_STYLE    (QString(\
                        "QLabel {"\
	                        "border-style: solid;"\
	                        "border-top-width: 1px;"\
                            "border-bottom-width: 1px;"\
                            "border-right-width: 1px;"\
                            "border-top-color: rgba(0, 0, 0, 0);"\
	                        "border-bottom-color: %1;"\
                            "border-right-color: %1;"\
                        "}"\
                    ))
#define HL_STYLE    (QString(\
                        "QLabel {"\
	                        "border-style: solid;"\
	                        "border-top-width: 1px;"\
                            "border-bottom-width: 1px;"\
                            "border-right-width: 1px;"\
                            "border-bottom-color: rgba(0, 0, 0, 0);"\
	                        "border-top-color: %1;"\
                            "border-right-color: %1;"\
                        "}"\
                    ))
#define H_STYLE     (QString(\
                        "QLabel {"\
	                        "border-style: solid;"\
	                        "border-top-width: 1px;"\
                            "border-bottom-width: 1px;"\
                            "border-bottom-color: rgba(0, 0, 0, 0);"\
	                        "border-top-color: %1;"\
                        "}"\
                    ))
#define L_STYLE     (QString(\
                        "QLabel {"\
	                        "border-style: solid;"\
	                        "border-top-width: 1px;"\
                            "border-bottom-width: 1px;"\
                            "border-top-color: rgba(0, 0, 0, 0);"\
	                        "border-bottom-color: %1;"\
                        "}"\
                    ))

PitchWidget::PitchWidget(const Pitch &pitch, QWidget *parent) : QWidget(parent)
{
    if (pitch.reading.isEmpty())
    {
        qDebug() << "Reading of empty size from dictionary" << pitch.dictionary;
        return;
    }

    QBoxLayout *layoutParent;
    if (pitch.position.size() > 1)
    {
        layoutParent = new QVBoxLayout(this);
    }
    else
    {
        layoutParent = new QHBoxLayout(this);
    } 
    layoutParent->setMargin(0);

    layoutParent->addWidget(new TagWidget(pitch), 0, Qt::AlignLeft);

    QStringList mora  = breakIntoMora(pitch.reading);
    QString     color = palette().color(foregroundRole()).name();
    for (const uint8_t pos : pitch.position)
    {
        QHBoxLayout *layoutLine = new QHBoxLayout;
        layoutLine->setMargin(0);
        layoutLine->setSpacing(0);
        layoutParent->addLayout(layoutLine);
        
        switch (pos)
        {
        case 0:
        {
            QString text  = mora.first();
            QString style = mora.size() <= 1 ? L_STYLE.arg(color) : LH_STYLE.arg(color);
            layoutLine->addWidget(createLabel(text, style));

            if (mora.size() > 1)
            {
                text  = QString(pitch.reading).remove(0, mora.first().size());
                style = H_STYLE.arg(color);
                layoutLine->addWidget(createLabel(text, style));
            }
            break;
        }
        case 1:
        {
            QString text  = mora.first();
            QString style = mora.size() <= 1 ? H_STYLE.arg(color) : HL_STYLE.arg(color);
            layoutLine->addWidget(createLabel(text, style));

            if (mora.size() > 1)
            {
                text  = QString(pitch.reading).remove(0, mora.first().size());
                style = L_STYLE.arg(color);
                layoutLine->addWidget(createLabel(text, style));
            }
            break;
        }
        default:
        {
            QString text  = mora.first();
            QString style = mora.size() <= 1 ? L_STYLE.arg(color) : LH_STYLE.arg(color);
            layoutLine->addWidget(createLabel(text, style));

            text.clear();
            for (size_t i = 1; i < pos - 1; ++i)
            {
                text += mora[i];
            }
            if (!text.isEmpty())
            {
                style = H_STYLE.arg(color);
                layoutLine->addWidget(createLabel(text, style));
            }
            
            style = HL_STYLE.arg(color);
            text  = mora[pos - 1];
            layoutLine->addWidget(createLabel(text, style));

            text.clear();
            for (size_t i = pos; i < mora.size(); ++i)
            {
                text += mora[i];
            }
            if (!text.isEmpty())
            {
                style = L_STYLE.arg(color);
                layoutLine->addWidget(createLabel(text, style));
            }
        }
        }

        QLabel *labelNumber = new QLabel;
        labelNumber->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        labelNumber->setText(" [" +QString::number(pos) + "]");
        layoutLine->addWidget(labelNumber);
        layoutLine->addStretch();
    }
}

QStringList PitchWidget::breakIntoMora(const QString &reading)
{
    QSet<QString> skipChar;
    skipChar << "ぁ"
             << "ぃ"
             << "ぅ"
             << "ぇ"
             << "ぉ"
             << "ゃ"
             << "ゅ"
             << "ょ"
             << "ァ"
             << "ィ"
             << "ゥ"
             << "ェ"
             << "ォ"
             << "ャ"
             << "ュ"
             << "ョ";

    QStringList mora;
    QString currentMora;

    for (const QString &ch : reading)
    {
        if (!currentMora.isEmpty() && !skipChar.contains(ch))
        {
            mora.append(currentMora);
            currentMora.clear();
        }
        currentMora += ch;
    }
    if (!currentMora.isEmpty())
    {
        mora.append(currentMora);
    }

    return mora;
}

QLabel *PitchWidget::createLabel(const QString &text, const QString &style)
{
    QLabel *label = new QLabel;
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    label->setStyleSheet(style);
    label->setText(text);
    return label;
}