#ifndef SUBTITLEWIDGET_H
#define SUBTITLEWIDGET_H

#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>

#include "../../lib/kiten/dictionarymanager.h"
#include "../../lib/kiten/entry.h"
#include "../../lib/kiten/DictEdict/indexededictfile.h"

#define EDICT_FILENAME "edict"
#define EDICT "edict"

class SubtitleWidget : public QLineEdit
{
    Q_OBJECT

public:
    SubtitleWidget(QWidget *parent = 0);
    ~SubtitleWidget();
    void updateText(const QString &text);

Q_SIGNALS:
    void entryFound(const Entry *entry);

protected:
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;

private:
    DictionaryManager *m_dictionaryManager;
    IndexedEdictFile *m_edict;
    int m_currentIndex;

    void findEntry();
};

#endif // SUBTITLEWIDGET_H