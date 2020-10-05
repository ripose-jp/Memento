#include "subtitlewidget.h"

#include "definitionwidget.h"

#include "../../util/directoryutils.h"

#include <QDebug>

SubtitleWidget::SubtitleWidget(QWidget *parent) : QLineEdit(parent), m_currentIndex(-1)
{
    setStyleSheet("QLineEdit { color : white; background-color : black; }");

    QString path = DirectoryUtils::getDictionaryDir() + JMDICT_DB_NAME;
    m_dictionary = new JMDict(path.toStdString());
}

SubtitleWidget::~SubtitleWidget()
{
    delete m_dictionary;
}

void SubtitleWidget::updateText(const QString &text)
{
    setText(text);
    setAlignment(Qt::AlignCenter);
    m_currentIndex = -1;
}

void SubtitleWidget::deselectText()
{
    m_currentIndex = -1;
    deselect();
}

void SubtitleWidget::findEntry()
{
    QString queryStr = text().remove(0, m_currentIndex);
    while (!queryStr.isEmpty())
    {
        Entry *entry = m_dictionary->query(queryStr.toStdString(), JMDict::FULLTEXT);
        if (!entry->m_descriptions->empty())
        {
            Q_EMIT entryChanged(entry);
            setSelection(m_currentIndex, queryStr.size());
            break;
        }
        delete entry;
        queryStr.chop(1);
    }
}

void SubtitleWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (cursorPositionAt(event->pos()) != m_currentIndex)
    {
        m_currentIndex = cursorPositionAt(event->pos());
        findEntry();
    }
    event->ignore();
}