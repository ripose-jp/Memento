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
// encoding: utf-8
#include "kana2romaji.h"
#include <map>
#include <iostream>
#include <ostream>
#include <string>

using namespace std;

namespace  {
void utfchar(const string& from, string::size_type pos, string& to) {
    unsigned first = from[pos];
    if ((first & 0x80) == 0)
        to = from[pos];
    else {
        string::size_type len = 0;
        while (first & 0x80) {
            ++len;
            first <<= 1;
        }
        to = from.substr(pos, len);
    }
}
}

typedef map<string, string> romaji_map;
romaji_map romaji;

void initRomaji() {
    // -- hiragana -----
    romaji["あ"] = "a";
    romaji["い"] = "i";
    romaji["う"] = "u";
    romaji["え"] = "e";
    romaji["お"] = "o";    
    romaji["か"] = "ka";
    romaji["き"] = "ki";
    romaji["く"] = "ku";
    romaji["け"] = "ke";
    romaji["こ"] = "ko";
    romaji["さ"] = "sa";
    romaji["し"] = "shi";
    romaji["す"] = "su";
    romaji["せ"] = "se";
    romaji["そ"] = "so";
    romaji["た"] = "ta";
    romaji["ち"] = "chi";
    romaji["つ"] = "tsu";
    romaji["て"] = "te";
    romaji["と"] = "to";
    romaji["な"] = "na";
    romaji["に"] = "ni";
    romaji["ぬ"] = "nu";
    romaji["ね"] = "ne";
    romaji["の"] = "no";
    romaji["は"] = "ha";
    romaji["ひ"] = "hi";
    romaji["ふ"] = "fu";
    romaji["へ"] = "he";
    romaji["ほ"] = "ho";
    romaji["ま"] = "ma";
    romaji["み"] = "mi";
    romaji["む"] = "mu";
    romaji["め"] = "me";
    romaji["も"] = "mo";
    romaji["や"] = "ya";
    romaji["ゆ"] = "yu";
    romaji["よ"] = "yo";
    romaji["ら"] = "ra";
    romaji["り"] = "ri";
    romaji["る"] = "ru";
    romaji["れ"] = "re";
    romaji["ろ"] = "ro";
    romaji["わ"] = "wa";
    romaji["ゐ"] = "wi";
    romaji["ゑ"] = "we";
    romaji["を"] = "wo";
    romaji["ん"] = "n";
    
    romaji["ぁ"] = "\1a";
    romaji["ぃ"] = "\1i";
    romaji["ぇ"] = "\1e";
    romaji["ぉ"] = "\1o";
    romaji["ゃ"] = "\1ya";
    romaji["ゅ"] = "\1yu";
    romaji["ょ"] = "\1yo";
    romaji["っ"] = "\2";

    romaji["ゔ"] = "vu";
    romaji["が"] = "ga";
    romaji["ぎ"] = "gi";
    romaji["ぐ"] = "gu";
    romaji["げ"] = "ge";
    romaji["ご"] = "go";
    romaji["ざ"] = "za";
    romaji["じ"] = "ji";
    romaji["ず"] = "zu";
    romaji["ぜ"] = "ze";
    romaji["ぞ"] = "zo";
    romaji["だ"] = "da";
    romaji["ぢ"] = "dzi";
    romaji["づ"] = "dzu";
    romaji["で"] = "de";
    romaji["ど"] = "do";
    romaji["ば"] = "ba";
    romaji["び"] = "bi";
    romaji["ぶ"] = "bu";
    romaji["べ"] = "be";
    romaji["ぼ"] = "bo";
    romaji["ぱ"] = "pa";
    romaji["ぴ"] = "pi";
    romaji["ぷ"] = "pu";
    romaji["ぺ"] = "pe";
    romaji["ぽ"] = "po";

    // -- katakana -----
    romaji["ア"] = "a";
    romaji["イ"] = "i";
    romaji["ウ"] = "u";
    romaji["エ"] = "e";
    romaji["オ"] = "o";    
    romaji["カ"] = "ka";
    romaji["キ"] = "ki";
    romaji["ク"] = "ku";
    romaji["ケ"] = "ke";
    romaji["コ"] = "ko";
    romaji["サ"] = "sa";
    romaji["シ"] = "shi";
    romaji["ス"] = "su";
    romaji["セ"] = "se";
    romaji["ソ"] = "so";
    romaji["タ"] = "ta";
    romaji["チ"] = "chi";
    romaji["ツ"] = "tsu";
    romaji["テ"] = "te";
    romaji["ト"] = "to";
    romaji["ナ"] = "na";
    romaji["ニ"] = "ni";
    romaji["ヌ"] = "nu";
    romaji["ネ"] = "ne";
    romaji["ノ"] = "no";
    romaji["ハ"] = "ha";
    romaji["ヒ"] = "hi";
    romaji["フ"] = "fu";
    romaji["ヘ"] = "he";
    romaji["ホ"] = "ho";
    romaji["マ"] = "ma";
    romaji["ミ"] = "mi";
    romaji["ム"] = "mu";
    romaji["メ"] = "me";
    romaji["モ"] = "mo";
    romaji["ヤ"] = "ya";
    romaji["ユ"] = "yu";
    romaji["ヨ"] = "yo";
    romaji["ラ"] = "ra";
    romaji["リ"] = "ri";
    romaji["ル"] = "ru";
    romaji["レ"] = "re";
    romaji["ロ"] = "ro";
    romaji["ワ"] = "wa";
    romaji["ヰ"] = "wi";
    romaji["ヱ"] = "we";
    romaji["ヲ"] = "wo";
    romaji["ン"] = "n";
    
    romaji["ァ"] = "\1a";
    romaji["ィ"] = "\1i";
    romaji["ゥ"] = "\1u";
    romaji["ェ"] = "\1e";
    romaji["ォ"] = "\1o";
    romaji["ヮ"] = "\1wa";
    romaji["ャ"] = "\1ya";
    romaji["ュ"] = "\1yu";
    romaji["ョ"] = "\1yo";
    romaji["ッ"] = "\2";

    romaji["ヴ"] = "vu";
    romaji["ガ"] = "ga";
    romaji["ギ"] = "gi";
    romaji["グ"] = "gu";
    romaji["ゲ"] = "ge";
    romaji["ゴ"] = "go";
    romaji["ザ"] = "za";
    romaji["ジ"] = "ji";
    romaji["ズ"] = "zu";
    romaji["ゼ"] = "ze";
    romaji["ゾ"] = "zo";
    romaji["ダ"] = "da";
    romaji["ヂ"] = "dzi";
    romaji["ヅ"] = "dzu";
    romaji["デ"] = "de";
    romaji["ド"] = "do";
    romaji["バ"] = "ba";
    romaji["ビ"] = "bi";
    romaji["ブ"] = "bu";
    romaji["ベ"] = "be";
    romaji["ボ"] = "bo";
    romaji["パ"] = "pa";
    romaji["ピ"] = "pi";
    romaji["プ"] = "pu";
    romaji["ペ"] = "pe";
    romaji["ポ"] = "po";
    romaji["・"] = " ";
    
    // -- double width letters ------
    romaji["Ａ"] = "A";
    romaji["Ｂ"] = "B";
    romaji["Ｃ"] = "C";
    romaji["Ｄ"] = "D";
    romaji["Ｅ"] = "E";
    romaji["Ｆ"] = "F";
    romaji["Ｇ"] = "G";
    romaji["Ｈ"] = "H";
    romaji["Ｉ"] = "I";
    romaji["Ｊ"] = "J";
    romaji["Ｋ"] = "K";
    romaji["Ｌ"] = "L";
    romaji["Ｍ"] = "M";
    romaji["Ｎ"] = "N";
    romaji["Ｏ"] = "O";
    romaji["Ｐ"] = "P";
    romaji["Ｑ"] = "Q";
    romaji["Ｒ"] = "R";
    romaji["Ｓ"] = "S";
    romaji["Ｔ"] = "T";
    romaji["Ｕ"] = "U";
    romaji["Ｖ"] = "V";
    romaji["Ｗ"] = "W";
    romaji["Ｘ"] = "X";
    romaji["Ｙ"] = "Y";
    romaji["Ｚ"] = "Z";

    romaji["ａ"] = "a";
    romaji["ｂ"] = "b";
    romaji["ｃ"] = "c";
    romaji["ｄ"] = "d";
    romaji["ｅ"] = "e";
    romaji["ｆ"] = "f";
    romaji["ｇ"] = "g";
    romaji["ｈ"] = "h";
    romaji["ｉ"] = "i";
    romaji["ｊ"] = "j";
    romaji["ｋ"] = "k";
    romaji["ｌ"] = "l";
    romaji["ｍ"] = "m";
    romaji["ｎ"] = "n";
    romaji["ｏ"] = "o";
    romaji["ｐ"] = "p";
    romaji["ｑ"] = "q";
    romaji["ｒ"] = "r";
    romaji["ｓ"] = "s";
    romaji["ｔ"] = "t";
    romaji["ｕ"] = "u";
    romaji["ｖ"] = "v";
    romaji["ｗ"] = "w";
    romaji["ｘ"] = "x";
    romaji["ｙ"] = "y";
    romaji["ｚ"] = "z";

    romaji["０"] = "0";
    romaji["１"] = "1";
    romaji["２"] = "2";
    romaji["３"] = "3";
    romaji["４"] = "4";
    romaji["５"] = "5";
    romaji["６"] = "6";
    romaji["７"] = "7";
    romaji["８"] = "8";
    romaji["９"] = "9";
   
    romaji["！"] = "!";
    romaji["＂"] = "\"";
    romaji["＃"] = "#";
    romaji["＄"] = "$";
    romaji["％"] = "%";
    romaji["＆"] = "&";
    romaji["＇"] = "'";
    romaji["＊"] = "*";
    romaji["＋"] = "+";
    romaji["，"] = ",";
    romaji["－"] = "-";
    romaji["．"] = ".";
    romaji["／"] = "/";

    romaji["："] = ":";
    romaji["；"] = ";";
    romaji["＜"] = "<";
    romaji["＝"] = "=";
    romaji["＞"] = ">";
    romaji["？"] = "?";
    romaji["＠"] = "@";

    romaji["〔"] = "(";
    romaji["〕"] = ")";
    romaji["（"] = "(";
    romaji["）"] = ")";
    romaji["［"] = "[";
    romaji["］"] = "]";
    romaji["【"] = "[";
    romaji["】"] = "]";
    romaji["｛"] = "{";
    romaji["｝"] = "}";
    romaji["＼"] = "\\";
    romaji["＾"] = "^";
    romaji["＿"] = "_";
    romaji["｀"] = "`";
    romaji["｜"] = "|";
    romaji["～"] = "~";
    romaji["ー"] = "";
    romaji["。"] = ".";
    romaji["、"] = ",";
    romaji["〜"] = "~";
    romaji["−"] = "-";
    romaji["―"] = "-";
    romaji["　"] = " ";

    romaji["ー"] = "\3";
}

void remove_quote_1(
  string::size_type const pos,
  string &rom)
{
  // if we encounter something like
  // "ki" + '\1' 
  // remove the previous character of \1 and the \1
  // if \1 is followed by an 'y' remove that also
  rom.erase(
    pos - 1,
    (pos + 1 < rom.size()
     && rom[pos + 1] == 'y'
    )
      ? 3
      : 2);
}

void kana2romaji(const string& kana, string& rom) {
    rom.clear();
    for (string::size_type pos = 0; pos < kana.size(); ) {
        string ch;
        utfchar(kana, pos, ch);
        romaji_map::const_iterator trans = romaji.find(ch);
        if (trans == romaji.end()) {
            rom += ch;
            if (ch.size() > 1)
                cout << "Don't know how to translate '" << ch << "' in '" << kana << "' to romaji.\n";
        }
        else
            rom += trans->second;
        pos += ch.size();
    }
    for (string::size_type pos = 0; pos < rom.size(); ++pos)
        if (rom[pos] == '\1') {

            if (pos > 2) {
               string const pred = rom.substr(pos - 3, 3);
               if(pred == "chi" ||
                  pred == "shi" ||
                  pred == "dzi" ||
                  pred == "tsu" ||
                  pred == "shi"
               ) {
                 remove_quote_1(pos, rom);
                 pos -= 2;
                 continue;
               }
            }
            if (pos > 1) {
             
               string const pred = rom.substr(pos - 2, 2);
               if(pred == "ki" ||
                  pred == "ni" ||
                  pred == "mi" ||
                  pred == "ri" ||
                  pred == "gi" ||
                  pred == "ji" ||
                  pred == "hi" ||
                  pred == "bi" ||
                  pred == "pi"
                )
               {
                 // shorten "ji\1y" to "j"
                 // otherwise remove "\1" and the preceding character
                 // but not the y
                 rom.erase(
                   pos - 1,
                   (pos + 1 < rom.size()
                    && rom[pos + 1] == 'y'
                    && pred[0] == 'j')
                      ? 3
                      : 2);
                  pos -= 2;
                 continue;
               }
               else if(
                  pred == "fu" ||
                  pred == "de" ||
                  pred == "te" ||
                  pred == "vu")
               {
                  remove_quote_1(pos, rom);
                  pos -= 2;
                 continue;
               }
               else if(
                 pred == "su" ||
                 pred == "zu" ||
                 pred == "te" ||
                 pred == "de" ||
                 pred == "ku" ||
                 pred == "gu" ||
                 pred == "mu"
               )
               {
                 rom[pos - 1] = 'w'; 
                 rom.erase(pos, 1);
                 --pos;
                 continue;
               }
               else if(
                 pred == "to" ||
                 pred == "do"
               )
               {
                 rom[pos - 1] = 'h';
                 rom.erase(pos, 1);
                 --pos;
                 continue;
               }
            }

            if (pos > 0) {
              char const pred = rom[pos - 1];

              switch(pred)
              {
              case 'a':
              case 'i':
              case 'u':
              case 'e':
              case 'o':
                rom.erase(pos, 1);
                --pos;
                break;
              default:
                cout << "Encountered a special character in " << kana << " but don't know what to do with it.\n";
              }
            }
            else
            {
              rom.erase(pos, 1);
              --pos;
            }
        }
        else if (rom[pos] == '\2')
        {
	  // two tsu may follow each other, so just remove them
          if(pos + 1 < rom.size() && rom[pos + 1] != '\2')
            rom[pos] = rom[pos + 1];
          else
          {
            rom.erase(pos, 1);
            --pos;
          }
        }
        else if (rom[pos] == '\3')
        {
          if(pos == 0)
          {
            if(rom.size() == 1)
              rom = "chouon";
            else
            {
              cout << "ー is the first letter of " << kana << ". Don't know how to translate this.\n";
              rom.erase(pos, 1);
              --pos;
            }
          }
          else
            rom[pos] = rom[pos-1];
        }

  for (string::size_type pos = 0; pos < rom.size(); ++pos)
    switch(rom[pos])
    {
    case '\1':
    case '\2':
    case '\3':
      cout << "Failed to translate " << kana << '\n';
      return;
    }
}
