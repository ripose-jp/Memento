#ifndef DEFINITIONWIDGET_H
#define DEFINITIONWIDGET_H

#include <QWidget>

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

private:
    Ui::DefinitionWidget *m_ui;
    const Entry *m_entry;

    QString generateJishoLink(const QString &word);
    QString buildDefinition(const std::vector<std::vector<std::string>> &definitions);
};

#endif // DEFINITIONWIDGET_H