#ifndef DEFINITIONWIDGET_H
#define DEFINITIONWIDGET_H

#include <QWidget>

#include <QWheelEvent>

#include "../../dict/entry.h"

namespace Ui
{
    class DefinitionWidget;
}

class DefinitionWidget : public QWidget
{
    Q_OBJECT

public:
    DefinitionWidget(QWidget *parent = 0);
    ~DefinitionWidget();

public Q_SLOTS:  
    /**
     * Sets the entry of this widget, deletes the old entry
     */
    void setEntry(const Entry *entry);

Q_SIGNALS:
    void definitionHidden();

protected:
    void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;
    // Prevents these events from being sent to mpv when widget has focus
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE { event->accept(); }
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE { event->accept(); }
    void mouseDoubleClickEvent(QMouseEvent *event) Q_DECL_OVERRIDE { event->accept(); }
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE { event->accept(); }

private:
    Ui::DefinitionWidget *m_ui;
    const Entry *m_entry;

    QString generateJishoLink(const QString &word);
    QString buildDefinition(const std::vector<std::vector<std::string>> &definitions);
};

#endif // DEFINITIONWIDGET_H