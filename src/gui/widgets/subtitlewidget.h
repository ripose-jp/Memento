#ifndef SUBTITLEWIDGET_H
#define SUBTITLEWIDGET_H

#include "../../dict/jmdict.h"

#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QRunnable>

#define JMDICT_DB_NAME "JMDict_e"

class SubtitleWidget : public QLineEdit
{
    Q_OBJECT

public:
    SubtitleWidget(QWidget *parent = 0);
    ~SubtitleWidget();

public Q_SLOTS:
    void updateText(const QString &text);
    void deselectText();

Q_SIGNALS:
    void entryChanged(const Entry *entry);

protected:
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    JMDict *m_dictionary;
    int m_currentIndex;

    void findEntry();

    class QueryThread : public QRunnable
    {
    public:
        QueryThread(SubtitleWidget *sw, const QString &query, 
                    const QString &currentSubtitle, const int currentIndex) : m_parent(sw), 
                                                                              m_query(query), 
                                                                              m_currentSubtitle(currentSubtitle),
                                                                              m_currentIndex(currentIndex) {}
        void run();

    private:
        SubtitleWidget *m_parent;
        QString m_query;
        QString m_currentSubtitle;
        const int m_currentIndex;
    };
};

#endif // SUBTITLEWIDGET_H