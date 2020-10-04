#include "subtitlewidget.h"

#include "definitionwidget.h"

#include "../../util/directoryutils.h"
#include "../../lib/kiten/dictquery.h"
#include "../../lib/kiten/entrylist.h"

#include <QVBoxLayout>
#include <QDebug>

SubtitleWidget::SubtitleWidget(QWidget *parent) : QLineEdit(parent), m_dictionaryManager(new DictionaryManager), m_currentIndex(-1)
{
    setStyleSheet("QLineEdit { color : white; background-color : black; }");
    setFrame(false);

    QString path = DirectoryUtils::getDictionaryDir() + EDICT_FILENAME;
    m_dictionaryManager->addDictionary(path, EDICT, EDICT);
}

SubtitleWidget::~SubtitleWidget()
{
    m_dictionaryManager->removeAllDictionaries();
    delete m_dictionaryManager;
}

void SubtitleWidget::updateText(const QString &text)
{
    setText(text);
    setAlignment(Qt::AlignCenter);
    m_currentIndex = -1;
}

void SubtitleWidget::findEntry()
{
    QString queryStr = text().remove(0, m_currentIndex);
    while (!queryStr.isEmpty())
    {
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