#include "subtitlewidget.h"

#include "definitionwidget.h"

#include "../../util/directoryutils.h"
#include "../../dict/jmdict.h"

#include <QVBoxLayout>
#include <QDebug>

SubtitleWidget::SubtitleWidget(QWidget *parent) : QLineEdit(parent), m_currentIndex(-1)
{
    setStyleSheet("QLineEdit { color : white; background-color : black; }");
    setFrame(false);
}

SubtitleWidget::~SubtitleWidget()
{
}

void SubtitleWidget::updateText(const QString &text)
{
    setText(text);
    setAlignment(Qt::AlignCenter);
    m_currentIndex = -1;
}

void SubtitleWidget::findEntry()
{
    JMDict dictionary;
    QString queryStr = text().remove(0, m_currentIndex);
    while (!queryStr.isEmpty())
    {
        /*
        DictQuery query(queryStr);
        query.setMatchType(DictQuery::MatchType::Beginning);
        EntryList *entries = m_dictionaryManager->doSearch(query);
        if (!entries->empty())
        {
            Q_EMIT entryFound(entries->first());
            qDebug() << entries->first()->dumpEntry();
            entries->deleteAll();
            delete entries;
            setSelection(m_currentIndex, queryStr.size());
            break;
        }
        entries->deleteAll();
        delete entries;
        */
        dictionary.query(queryStr.toStdString(), JMDict::FULLTEXT);
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

void SubtitleWidget::leaveEvent(QEvent *event)
{
    //deselect();
}