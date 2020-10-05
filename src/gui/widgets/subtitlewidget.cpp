#include "subtitlewidget.h"

#include "definitionwidget.h"

#include "../../util/directoryutils.h"

#include <QThreadPool>
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
    QueryThread *queryThread = new QueryThread(this, queryStr, text(), m_currentIndex);
    QThreadPool::globalInstance()->start(queryThread);
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

void SubtitleWidget::QueryThread::run()
{
    while (!m_query.isEmpty() && m_currentIndex == m_parent->m_currentIndex)
    {
        Entry *entry = m_parent->m_dictionary->query(m_query.toStdString(), JMDict::FULLTEXT);
        if (!entry->m_descriptions->empty() && m_currentIndex == m_parent->m_currentIndex && m_currentSubtitle == m_parent->text())
        {
            Q_EMIT m_parent->entryChanged(entry);
            m_parent->setSelection(m_parent->m_currentIndex, m_query.size());
            break;
        }
        delete entry;
        m_query.chop(1);
    }
}