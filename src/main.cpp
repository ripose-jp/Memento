#include <QApplication>
#include <QDebug>

#include "gui/mainwindow.h"
#include "lib/kiten/dictionarymanager.h"
#include "lib/kiten/entrylist.h"
#include "lib/kiten/dictquery.h"
#include "util/directoryutils.h"

int main(int argc, char *argv[])
{
    QApplication memento(argc, argv);
    // Qt sets the locale in the QApplication constructor, but libmpv requires
    // the LC_NUMERIC category to be set to "C", so change it back.
    setlocale(LC_NUMERIC, "C");

    qDebug() << "Loading dictionary";

    DictionaryManager dictionary;
    dictionary.addDictionary(DirectoryUtils::getDictionaryDir() + "edict", "edict", "edict");
    DictQuery query("かかる");
    EntryList *result = dictionary.doSearch(query);
    qDebug() << (*result->begin())->dumpEntry();
    delete result;
    dictionary.removeAllDictionaries();

    qDebug() << "Dictionary loaded";

    MainWindow main_window;
    main_window.show();
    return memento.exec();
}
