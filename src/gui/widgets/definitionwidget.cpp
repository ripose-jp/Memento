#include "definitionwidget.h"
#include "ui_definitionwidget.h"

#include <iostream>

#define ALT_KANJI_TEXT "Alternative Kanji: "
#define ALT_KANA_TEXT "Alternative Kana: "

DefinitionWidget::DefinitionWidget(QWidget *parent) : QWidget(parent), m_ui(new Ui::DefinitionWidget), m_entry(0)
{
    m_ui->setupUi(this);
    hide();
}

DefinitionWidget::~DefinitionWidget()
{
    delete m_ui;
    delete m_entry;
}

void DefinitionWidget::setEntry(const Entry *entry)
{
    delete m_entry;
    m_entry = entry;

    if (m_entry->m_kanji->empty())
    {
        m_ui->m_labelKana->hide();
        m_ui->m_labelKanji->setText(QString::fromStdString(*m_entry->m_kana));
        m_ui->m_labelJisho->setText(generateJishoLink(QString::fromStdString(*m_entry->m_kana)));
    }
    else
    {
        m_ui->m_labelKana->show();
        m_ui->m_labelKana->setText(QString::fromStdString(*m_entry->m_kana));
        m_ui->m_labelKanji->setText(QString::fromStdString(*m_entry->m_kanji));
        m_ui->m_labelJisho->setText(generateJishoLink(QString::fromStdString(*m_entry->m_kanji)));
    }

    m_ui->m_labelDefinition->setText(buildDefinition(*m_entry->m_descriptions));

    if (m_entry->m_altkanji->empty())
    {
        m_ui->m_labelAltKanji->hide();
    }
    else
    {
        m_ui->m_labelAltKanji->show();
        m_ui->m_labelAltKanji->setText(QString::fromStdString(ALT_KANJI_TEXT + *m_entry->m_altkanji));
    }

    if (m_entry->m_altkana->empty())
    {
        m_ui->m_labelAltKana->hide();
    }
    else
    {
        m_ui->m_labelAltKana->show();
        m_ui->m_labelAltKana->setText(QString::fromStdString(ALT_KANA_TEXT + *m_entry->m_altkana));
    }

    show();
}

QString DefinitionWidget::generateJishoLink(const QString &word)
{
    return QString("<a href=\"https://jisho.org/search/%1\">Jisho</a>").arg(word);
}

QString DefinitionWidget::buildDefinition(const std::vector<std::vector<std::string>> &definitions)
{
    QString defStr;
    for (uint i = 0; i < definitions.size(); ++i)
    {
        if (i)
            defStr.push_back('\n');
        
        defStr += QString::number(i + 1);
        defStr += ") ";
        defStr.push_back('(');
        defStr += QString::fromStdString(definitions[i].front());
        defStr.push_back(')');
        for (uint j = 1; j < definitions[i].size(); ++j)
        {
            defStr.push_back('\n');
            defStr += "- ";
            defStr += QString::fromStdString(definitions[i][j]);
        }
    }
    return defStr;
}