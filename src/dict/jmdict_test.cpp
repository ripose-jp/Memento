/*
jmdict, a frontend to the JMdict file. http://mandrill.fuxx0r.net/jmdict.php
Copyright (C) 2004 Florian Bluemel (florian.bluemel@uni-dortmund.de)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <iomanip>
#include <string>
#include <stdexcept>
#include <exception>
#include <memory>
#include <unistd.h>
#include "sqlite.h"
#include "kana2romaji.h"
using namespace std;

void usage() {
    cout << "jmdict [options] subject\n"
            "  -b        search for entries beginning with <subject>\n"
            "  -f        perform a fulltext search\n"
            "  -i        case-insensitive search (implied by -b or -f)\n"
            "  -r        also translate kana to romaji\n"
            "\n"
            "  -j        translate from japanese\n"
            "  -J        translate to japanese\n"
            "            if neither -j nor -J is given, source language will be guessed\n"
            "\n"
            "  -l lang   target language is lang, where lang is a three-letter language code\n"
            "            default: eng\n";
}

namespace options {
    enum Language { UNKNOWN, JAPANESE, JAPANESE_ROMAJI, NOT_JAPANESE };

    Language source = UNKNOWN;
    string target("eng");
    bool fulltext = false;
    bool beginning = false;
    bool ci_search = false;
    bool show_romaji = false;

    void getFrom(int argc, char** argv) {
        int opt;
        while ((opt = getopt(argc, argv, "bfirjJl:")) != -1)
            switch (opt) {
                case 'b':   beginning = true;       break;
                case 'f':   fulltext = true;        break;
                case 'i':   ci_search = true;       break;
                case 'r':   show_romaji = true;     break;
                case 'j':   source = JAPANESE;      break;
                case 'J':   source = NOT_JAPANESE;  break;
                case 'l':   target = optarg;        break;
                case '?':   throw invalid_argument(string("unrecognized option"));
            }
    }
}

auto_ptr<sql::db> db;
unsigned entries(0);

int accumulate(void* to, int, char** what, char**) {
    string& app = *static_cast<string*>(to);
    if (app.size())
        app += ", ";
    app += *what;
    return 0;
}

int showGloss(void* s, int, char** value, char**) {
    string& sense = *static_cast<string*>(s);
    if (sense != value[0]) {
        sense = value[0];
        cout << "  " << setw(2) << sense << ")  ";
    }
    else
        cout << "       ";
    cout << value[1] << endl;
    return 0;
}

int showEntry(void*, int, char** value, char**) {
    string kanji, kana;
    db->exec(
        sql::query("SELECT kanji FROM kanji WHERE entry=%s") % *value,
        accumulate, &kanji);
    db->exec(
        sql::query("SELECT kana FROM reading WHERE entry=%s") % *value,
        accumulate, &kana);

    if (kanji.size())
        cout << kanji << " (" << kana << ')';
    else
        cout << kana;

    if(options::show_romaji) {
        string rom;
        kana2romaji(kana,rom);

        cout << " (" << rom << ')';
    }

    cout << endl;
    
    string sense;
    db->exec(
        sql::query("SELECT sense, gloss FROM gloss WHERE lang=%Q AND entry=%s "
                   "ORDER BY sense") % options::target % *value,
        showGloss, &sense);
    ++entries;
    return 0;
}

string compare() {
    if (options::fulltext)
        return " LIKE '%%%q%%'";
    if (options::beginning)
        return " LIKE '%q%%'";
    if (options::ci_search)
        return " LIKE %Q";
    return "=%Q";
}

void fromRomaji(const string& r) {
    db->exec(
        sql::query("SELECT DISTINCT entry FROM reading WHERE romaji" + compare()) % r,
        showEntry);
}

void fromJapanese(const string& j) {
    db->exec(
        sql::query("SELECT DISTINCT entry FROM reading WHERE kana" + compare()) % j,
        showEntry);
    db->exec(
        sql::query("SELECT DISTINCT entry FROM kanji WHERE kanji" + compare()) % j,
        showEntry);
}

void toJapanese(const string& e) {
    sql::query q;
    q = "SELECT DISTINCT entry FROM gloss WHERE lang=%Q AND gloss" + compare();
    db->exec(q % options::target % e, showEntry);
}

void guessLanguage(const std::string& subject) {
    bool isUTF8 = subject[0] & 0x80;
    if (options::source == options::JAPANESE && !isUTF8)
        options::source = options::JAPANESE_ROMAJI;
    else if (options::source == options::UNKNOWN)
        options::source = isUTF8 ? options::JAPANESE : options::NOT_JAPANESE;
}

int main(int argc, char** argv)
try {
    initRomaji();
    options::getFrom(argc, argv);
    if (optind == argc) {
        usage();
        return EXIT_FAILURE;
    }
    string subject = argv[optind];
    db.reset(new sql::db(DICTIONARY_PATH));
    
    guessLanguage(subject);
    if (options::source == options::JAPANESE)
        fromJapanese(subject);
    else if (options::source == options::JAPANESE_ROMAJI)
        fromRomaji(subject);
    else
        toJapanese(subject);
    cout << entries << " match(es) found." << endl;

    return EXIT_SUCCESS;
}
catch(const std::exception& e)
{
    cerr << e.what() << '\n';
    return EXIT_FAILURE;
}
