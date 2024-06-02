////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2024 Spacehamster
//
// This file is part of Memento.
//
// Memento is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2 of the License.
//
// Memento is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Memento.  If not, see <https://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////

#include "deconjugator.h"
#include <QString>
#include <QList>
#include <QMap>
#include <QStack>
#include <QPair>

struct Rule
{
    QString base;
    QString conjugated;
    WordForm baseType;
    WordForm conjugatedType;
};

static const QList<Rule> silentRules =
{
    { u8"ない", u8"ない", WordForm::negative, WordForm::adjective },
    { u8"たい", u8"たい", WordForm::tai, WordForm::adjective },
    { u8"せる", u8"せる", WordForm::causative, WordForm::ichidanVerb },
    { u8"れる", u8"れる", WordForm::passive, WordForm::ichidanVerb },
    { u8"る", u8"る", WordForm::potential, WordForm::ichidanVerb },
    { u8"られる", u8"られる", WordForm::potentialPassive, WordForm::ichidanVerb },
    { u8"しまう", u8"しまう", WordForm::shimau, WordForm::godanVerb },
    { u8"ゃう", u8"ゃう", WordForm::chau, WordForm::godanVerb },
    { u8"まう", u8"まう", WordForm::chimau, WordForm::godanVerb },
    { u8"る", u8"る", WordForm::continuous, WordForm::ichidanVerb },
    { u8"おる", u8"おる", WordForm::continuous, WordForm::godanVerb },
    { u8"すぎる", u8"すぎる", WordForm::sugiru, WordForm::ichidanVerb },
    { u8"とく", u8"とく", WordForm::toku, WordForm::godanVerb },
};

static const QList<Rule> rules = {
    //Negative
    { u8"る", u8"らない", WordForm::godanVerb, WordForm::negative },
    { u8"う", u8"わない", WordForm::godanVerb, WordForm::negative },
    { u8"つ", u8"たない", WordForm::godanVerb, WordForm::negative },
    { u8"す", u8"さない", WordForm::godanVerb, WordForm::negative },
    { u8"く", u8"かない", WordForm::godanVerb, WordForm::negative },
    { u8"ぐ", u8"がない", WordForm::godanVerb, WordForm::negative },
    { u8"ぶ", u8"ばない", WordForm::godanVerb, WordForm::negative },
    { u8"む", u8"まない", WordForm::godanVerb, WordForm::negative },
    { u8"ぬ", u8"なない", WordForm::godanVerb, WordForm::negative },
    { u8"る", u8"ない", WordForm::ichidanVerb, WordForm::negative },
    { u8"くる", u8"こない", WordForm::kuruVerb, WordForm::negative },
    { u8"来る", u8"来ない", WordForm::kuruVerb, WordForm::negative },
    { u8"する", u8"しない", WordForm::suruVerb, WordForm::negative },
    { u8"為る", u8"為ない", WordForm::suruVerb, WordForm::negative },

    //Past
    { u8"る", u8"った", WordForm::godanVerb, WordForm::past },
    { u8"う", u8"った", WordForm::godanVerb, WordForm::past },
    { u8"つ", u8"った", WordForm::godanVerb, WordForm::past },
    { u8"す", u8"した", WordForm::godanVerb, WordForm::past },
    { u8"く", u8"いた", WordForm::godanVerb, WordForm::past },
    { u8"ぐ", u8"いだ", WordForm::godanVerb, WordForm::past },
    { u8"ぶ", u8"んだ", WordForm::godanVerb, WordForm::past },
    { u8"む", u8"んだ", WordForm::godanVerb, WordForm::past },
    { u8"ぬ", u8"んだ", WordForm::godanVerb, WordForm::past },
    { u8"る", u8"た", WordForm::ichidanVerb, WordForm::past },
    { u8"くる", u8"きた", WordForm::kuruVerb, WordForm::past },
    { u8"来る", u8"来た", WordForm::kuruVerb, WordForm::past },
    { u8"する", u8"した", WordForm::suruVerb, WordForm::past },
    { u8"為る", u8"為た", WordForm::suruVerb, WordForm::past },
    { u8"行く", u8"行った", WordForm::godanVerb, WordForm::past },
    { u8"いく", u8"いった", WordForm::godanVerb, WordForm::past },
    { u8"問う", u8"問うた", WordForm::godanVerb, WordForm::past },
    { u8"とう", u8"とうた", WordForm::godanVerb, WordForm::past },
    { u8"請う", u8"請うた", WordForm::godanVerb, WordForm::past },
    { u8"こう", u8"こうた", WordForm::godanVerb, WordForm::past },

    //Te
    { u8"る", u8"って", WordForm::godanVerb, WordForm::te },
    { u8"う", u8"って", WordForm::godanVerb, WordForm::te },
    { u8"つ", u8"って", WordForm::godanVerb, WordForm::te },
    { u8"す", u8"して", WordForm::godanVerb, WordForm::te },
    { u8"く", u8"いて", WordForm::godanVerb, WordForm::te },
    { u8"ぐ", u8"いで", WordForm::godanVerb, WordForm::te },
    { u8"ぶ", u8"んで", WordForm::godanVerb, WordForm::te },
    { u8"ぬ", u8"んで", WordForm::godanVerb, WordForm::te },
    { u8"む", u8"んで", WordForm::godanVerb, WordForm::te },
    { u8"る", u8"て", WordForm::ichidanVerb, WordForm::te },
    { u8"くる", u8"きて", WordForm::kuruVerb, WordForm::te },
    { u8"来る", u8"来て", WordForm::kuruVerb, WordForm::te },
    { u8"する", u8"して", WordForm::suruVerb, WordForm::te },
    { u8"為る", u8"為て", WordForm::suruVerb, WordForm::te },
    { u8"行く", u8"行って", WordForm::godanVerb, WordForm::te },
    { u8"いく", u8"いって", WordForm::godanVerb, WordForm::te },
    { u8"問う", u8"問うて", WordForm::godanVerb, WordForm::te },
    { u8"とう", u8"とうて", WordForm::godanVerb, WordForm::te },
    { u8"請う", u8"請うて", WordForm::godanVerb, WordForm::te },
    { u8"こう", u8"こうて", WordForm::godanVerb, WordForm::te },

    //toku
    { u8"る", u8"っとく", WordForm::godanVerb, WordForm::toku },
    { u8"う", u8"っとく", WordForm::godanVerb, WordForm::toku },
    { u8"つ", u8"っとく", WordForm::godanVerb, WordForm::toku },
    { u8"す", u8"しとく", WordForm::godanVerb, WordForm::toku },
    { u8"く", u8"いとく", WordForm::godanVerb, WordForm::toku },
    { u8"ぐ", u8"いどく", WordForm::godanVerb, WordForm::toku },
    { u8"ぶ", u8"んどく", WordForm::godanVerb, WordForm::toku },
    { u8"ぬ", u8"んどく", WordForm::godanVerb, WordForm::toku },
    { u8"む", u8"んどく", WordForm::godanVerb, WordForm::toku },
    { u8"る", u8"とく", WordForm::ichidanVerb, WordForm::toku },
    { u8"くる", u8"きとく", WordForm::kuruVerb, WordForm::toku },
    { u8"来る", u8"来とく", WordForm::kuruVerb, WordForm::toku },
    { u8"する", u8"しとく", WordForm::suruVerb, WordForm::toku },
    { u8"為る", u8"為とく", WordForm::suruVerb, WordForm::toku },
    { u8"行く", u8"行っとく", WordForm::godanVerb, WordForm::toku },
    { u8"問う", u8"問うとく", WordForm::godanVerb, WordForm::toku },
    { u8"請う", u8"請うとく", WordForm::godanVerb, WordForm::toku },

    //Imperative
    { u8"る", u8"れ", WordForm::godanVerb, WordForm::imperative },
    { u8"う", u8"え", WordForm::godanVerb, WordForm::imperative },
    { u8"つ", u8"て", WordForm::godanVerb, WordForm::imperative },
    { u8"す", u8"せ", WordForm::godanVerb, WordForm::imperative },
    { u8"く", u8"け", WordForm::godanVerb, WordForm::imperative },
    { u8"ぐ", u8"げ", WordForm::godanVerb, WordForm::imperative },
    { u8"ぶ", u8"べ", WordForm::godanVerb, WordForm::imperative },
    { u8"む", u8"め", WordForm::godanVerb, WordForm::imperative },
    { u8"ぬ", u8"ね", WordForm::godanVerb, WordForm::imperative },
    { u8"る", u8"ろ", WordForm::ichidanVerb, WordForm::imperative },
    { u8"る", u8"よ", WordForm::ichidanVerb, WordForm::imperative },
    { u8"くる", u8"こい", WordForm::kuruVerb, WordForm::imperative },
    { u8"来る", u8"来い", WordForm::kuruVerb, WordForm::imperative },
    { u8"する", u8"しろ", WordForm::suruVerb, WordForm::imperative },
    { u8"為る", u8"為ろ", WordForm::suruVerb, WordForm::imperative },
    { u8"する", u8"せよ", WordForm::suruVerb, WordForm::imperative },
    { u8"為る", u8"為よ", WordForm::suruVerb, WordForm::imperative },

    //Volitional
    { u8"る", u8"ろう", WordForm::godanVerb, WordForm::volitional },
    { u8"う", u8"おう", WordForm::godanVerb, WordForm::volitional },
    { u8"つ", u8"とう", WordForm::godanVerb, WordForm::volitional },
    { u8"す", u8"そう", WordForm::godanVerb, WordForm::volitional },
    { u8"く", u8"こう", WordForm::godanVerb, WordForm::volitional },
    { u8"ぐ", u8"ごう", WordForm::godanVerb, WordForm::volitional },
    { u8"ぶ", u8"ぼう", WordForm::godanVerb, WordForm::volitional },
    { u8"む", u8"もう", WordForm::godanVerb, WordForm::volitional },
    { u8"ぬ", u8"のう", WordForm::godanVerb, WordForm::volitional },
    { u8"る", u8"よう", WordForm::ichidanVerb, WordForm::volitional },
    { u8"くる", u8"こよう", WordForm::kuruVerb, WordForm::volitional },
    { u8"来る", u8"来よう", WordForm::kuruVerb, WordForm::volitional },
    { u8"する", u8"しよう", WordForm::suruVerb, WordForm::volitional },
    { u8"為る", u8"為よう", WordForm::suruVerb, WordForm::volitional },

    //Passive
    { u8"る", u8"られる", WordForm::godanVerb, WordForm::passive },
    { u8"う", u8"われる", WordForm::godanVerb, WordForm::passive },
    { u8"つ", u8"たれる", WordForm::godanVerb, WordForm::passive },
    { u8"す", u8"される", WordForm::godanVerb, WordForm::passive },
    { u8"く", u8"かれる", WordForm::godanVerb, WordForm::passive },
    { u8"ぐ", u8"がれる", WordForm::godanVerb, WordForm::passive },
    { u8"ぶ", u8"ばれる", WordForm::godanVerb, WordForm::passive },
    { u8"む", u8"まれる", WordForm::godanVerb, WordForm::passive },
    { u8"ぬ", u8"なれる", WordForm::godanVerb, WordForm::passive },
    { u8"る", u8"られる", WordForm::ichidanVerb, WordForm::potentialPassive },
    { u8"くる", u8"こられる", WordForm::kuruVerb, WordForm::potentialPassive },
    { u8"来る", u8"来られる", WordForm::kuruVerb, WordForm::potentialPassive },
    { u8"する", u8"される", WordForm::suruVerb, WordForm::passive },
    { u8"為る", u8"為れる", WordForm::suruVerb, WordForm::passive },

    //Potential
    { u8"る", u8"れる", WordForm::godanVerb, WordForm::potential },
    { u8"う", u8"える", WordForm::godanVerb, WordForm::potential },
    { u8"つ", u8"てる", WordForm::godanVerb, WordForm::potential },
    { u8"す", u8"せる", WordForm::godanVerb, WordForm::potential },
    { u8"く", u8"ける", WordForm::godanVerb, WordForm::potential },
    { u8"ぐ", u8"げる", WordForm::godanVerb, WordForm::potential },
    { u8"ぶ", u8"べる", WordForm::godanVerb, WordForm::potential },
    { u8"む", u8"める", WordForm::godanVerb, WordForm::potential },
    { u8"ぬ", u8"ねる", WordForm::godanVerb, WordForm::potential },
    { u8"る", u8"れる", WordForm::ichidanVerb, WordForm::potential },
    { u8"くる", u8"これる", WordForm::kuruVerb, WordForm::potential },
    { u8"来る", u8"来れる", WordForm::kuruVerb, WordForm::potential },
    { u8"する", u8"できる", WordForm::suruVerb, WordForm::potential },

    //Causative
    { u8"る", u8"らせる", WordForm::godanVerb, WordForm::causative },
    { u8"う", u8"わせる", WordForm::godanVerb, WordForm::causative },
    { u8"つ", u8"たせる", WordForm::godanVerb, WordForm::causative },
    { u8"す", u8"させる", WordForm::godanVerb, WordForm::causative },
    { u8"く", u8"かせる", WordForm::godanVerb, WordForm::causative },
    { u8"ぐ", u8"がせる", WordForm::godanVerb, WordForm::causative },
    { u8"ぶ", u8"ばせる", WordForm::godanVerb, WordForm::causative },
    { u8"む", u8"ませる", WordForm::godanVerb, WordForm::causative },
    { u8"ぬ", u8"なせる", WordForm::godanVerb, WordForm::causative },
    { u8"る", u8"させる", WordForm::ichidanVerb, WordForm::causative },
    { u8"くる", u8"こさせる", WordForm::kuruVerb, WordForm::causative },
    { u8"来る", u8"来させる", WordForm::kuruVerb, WordForm::causative },
    { u8"する", u8"させる", WordForm::suruVerb, WordForm::causative },
    { u8"為る", u8"為せる", WordForm::suruVerb, WordForm::causative },

    //Ba
    { u8"る", u8"れば", WordForm::godanVerb, WordForm::ba },
    { u8"う", u8"えば", WordForm::godanVerb, WordForm::ba },
    { u8"つ", u8"てば", WordForm::godanVerb, WordForm::ba },
    { u8"す", u8"せば", WordForm::godanVerb, WordForm::ba },
    { u8"く", u8"けば", WordForm::godanVerb, WordForm::ba },
    { u8"ぐ", u8"げば", WordForm::godanVerb, WordForm::ba },
    { u8"ぶ", u8"べば", WordForm::godanVerb, WordForm::ba },
    { u8"む", u8"めば", WordForm::godanVerb, WordForm::ba },
    { u8"ぬ", u8"ねば", WordForm::godanVerb, WordForm::ba },
    { u8"る", u8"れば", WordForm::ichidanVerb, WordForm::ba },
    { u8"くる", u8"くれば", WordForm::kuruVerb, WordForm::ba },
    { u8"来る", u8"来れば", WordForm::kuruVerb, WordForm::ba },
    { u8"する", u8"すれば", WordForm::suruVerb, WordForm::ba },
    { u8"為る", u8"為れば", WordForm::suruVerb, WordForm::ba },

    //Zaru
    { u8"る", u8"らざる", WordForm::godanVerb, WordForm::zaru },
    { u8"う", u8"わざる", WordForm::godanVerb, WordForm::zaru },
    { u8"つ", u8"たざる", WordForm::godanVerb, WordForm::zaru },
    { u8"す", u8"さざる", WordForm::godanVerb, WordForm::zaru },
    { u8"く", u8"かざる", WordForm::godanVerb, WordForm::zaru },
    { u8"ぐ", u8"がざる", WordForm::godanVerb, WordForm::zaru },
    { u8"ぶ", u8"ばざる", WordForm::godanVerb, WordForm::zaru },
    { u8"む", u8"まざる", WordForm::godanVerb, WordForm::zaru },
    { u8"ぬ", u8"なざる", WordForm::godanVerb, WordForm::zaru },
    { u8"る", u8"ざる", WordForm::ichidanVerb, WordForm::zaru },
    { u8"くる", u8"こざる", WordForm::kuruVerb, WordForm::zaru },
    { u8"来る", u8"来ざる", WordForm::kuruVerb, WordForm::zaru },
    { u8"する", u8"せざる", WordForm::suruVerb, WordForm::zaru },
    { u8"為る", u8"為ざる", WordForm::suruVerb, WordForm::zaru },

    //Zaru
    { u8"る", u8"らねば", WordForm::godanVerb, WordForm::neba },
    { u8"う", u8"わねば", WordForm::godanVerb, WordForm::neba },
    { u8"つ", u8"たねば", WordForm::godanVerb, WordForm::neba },
    { u8"す", u8"さねば", WordForm::godanVerb, WordForm::neba },
    { u8"く", u8"かねば", WordForm::godanVerb, WordForm::neba },
    { u8"ぐ", u8"がねば", WordForm::godanVerb, WordForm::neba },
    { u8"ぶ", u8"ばねば", WordForm::godanVerb, WordForm::neba },
    { u8"む", u8"まねば", WordForm::godanVerb, WordForm::neba },
    { u8"ぬ", u8"なねば", WordForm::godanVerb, WordForm::neba },
    { u8"る", u8"ねば", WordForm::ichidanVerb, WordForm::neba },
    { u8"くる", u8"こねば", WordForm::kuruVerb, WordForm::neba },
    { u8"来る", u8"来ねば", WordForm::kuruVerb, WordForm::neba },
    { u8"する", u8"せねば", WordForm::suruVerb, WordForm::neba },
    { u8"為る", u8"為ねば", WordForm::suruVerb, WordForm::neba },

    //Zu
    { u8"る", u8"らず", WordForm::godanVerb, WordForm::zu },
    { u8"う", u8"わず", WordForm::godanVerb, WordForm::zu },
    { u8"つ", u8"たず", WordForm::godanVerb, WordForm::zu },
    { u8"す", u8"さず", WordForm::godanVerb, WordForm::zu },
    { u8"く", u8"かず", WordForm::godanVerb, WordForm::zu },
    { u8"ぐ", u8"がず", WordForm::godanVerb, WordForm::zu },
    { u8"ぶ", u8"ばず", WordForm::godanVerb, WordForm::zu },
    { u8"む", u8"まず", WordForm::godanVerb, WordForm::zu },
    { u8"ぬ", u8"なず", WordForm::godanVerb, WordForm::zu },
    { u8"る", u8"ず", WordForm::ichidanVerb, WordForm::zu },
    { u8"くる", u8"こず", WordForm::kuruVerb, WordForm::zu },
    { u8"来る", u8"来ず", WordForm::kuruVerb, WordForm::zu },
    { u8"する", u8"せず", WordForm::suruVerb, WordForm::zu },
    { u8"為る", u8"為ず", WordForm::suruVerb, WordForm::zu },

    //Nu
    { u8"る", u8"らぬ", WordForm::godanVerb, WordForm::nu },
    { u8"う", u8"わぬ", WordForm::godanVerb, WordForm::nu },
    { u8"つ", u8"たぬ", WordForm::godanVerb, WordForm::nu },
    { u8"す", u8"さぬ", WordForm::godanVerb, WordForm::nu },
    { u8"く", u8"かぬ", WordForm::godanVerb, WordForm::nu },
    { u8"ぐ", u8"がぬ", WordForm::godanVerb, WordForm::nu },
    { u8"ぶ", u8"ばぬ", WordForm::godanVerb, WordForm::nu },
    { u8"む", u8"まぬ", WordForm::godanVerb, WordForm::nu },
    { u8"ぬ", u8"なぬ", WordForm::godanVerb, WordForm::nu },
    { u8"る", u8"ぬ", WordForm::ichidanVerb, WordForm::nu },
    { u8"くる", u8"こぬ", WordForm::kuruVerb, WordForm::nu },
    { u8"来る", u8"来ぬ", WordForm::kuruVerb, WordForm::nu },
    { u8"する", u8"せぬ", WordForm::suruVerb, WordForm::nu },
    { u8"為る", u8"為ぬ", WordForm::suruVerb, WordForm::nu },

    //Colloquial Masculine Negative
    { u8"る", u8"らん", WordForm::godanVerb, WordForm::colloquialNegative },
    { u8"う", u8"わん", WordForm::godanVerb, WordForm::colloquialNegative },
    { u8"つ", u8"たん", WordForm::godanVerb, WordForm::colloquialNegative },
    { u8"す", u8"さん", WordForm::godanVerb, WordForm::colloquialNegative },
    { u8"く", u8"かん", WordForm::godanVerb, WordForm::colloquialNegative },
    { u8"ぐ", u8"がん", WordForm::godanVerb, WordForm::colloquialNegative },
    { u8"ぶ", u8"ばん", WordForm::godanVerb, WordForm::colloquialNegative },
    { u8"む", u8"まん", WordForm::godanVerb, WordForm::colloquialNegative },
    { u8"ぬ", u8"なん", WordForm::godanVerb, WordForm::colloquialNegative },
    { u8"る", u8"ん", WordForm::ichidanVerb, WordForm::colloquialNegative },
    { u8"くる", u8"こん", WordForm::kuruVerb, WordForm::colloquialNegative },
    { u8"来る", u8"来ん", WordForm::kuruVerb, WordForm::colloquialNegative },
    { u8"する", u8"せん", WordForm::suruVerb, WordForm::colloquialNegative },
    { u8"為る", u8"為ん", WordForm::suruVerb, WordForm::colloquialNegative },

    //Colloquial provisional Negative
    { u8"る", u8"らなきゃ", WordForm::godanVerb, WordForm::provisionalColloquialNegative },
    { u8"う", u8"わなきゃ", WordForm::godanVerb, WordForm::provisionalColloquialNegative },
    { u8"つ", u8"たなきゃ", WordForm::godanVerb, WordForm::provisionalColloquialNegative },
    { u8"す", u8"さなきゃ", WordForm::godanVerb, WordForm::provisionalColloquialNegative },
    { u8"く", u8"かなきゃ", WordForm::godanVerb, WordForm::provisionalColloquialNegative },
    { u8"ぐ", u8"がなきゃ", WordForm::godanVerb, WordForm::provisionalColloquialNegative },
    { u8"ぶ", u8"ばなきゃ", WordForm::godanVerb, WordForm::provisionalColloquialNegative },
    { u8"む", u8"まなきゃ", WordForm::godanVerb, WordForm::provisionalColloquialNegative },
    { u8"ぬ", u8"ななきゃ", WordForm::godanVerb, WordForm::provisionalColloquialNegative },
    { u8"る", u8"なきゃ", WordForm::ichidanVerb, WordForm::provisionalColloquialNegative },
    { u8"くる", u8"こなきゃ", WordForm::kuruVerb, WordForm::provisionalColloquialNegative },
    { u8"来る", u8"来なきゃ", WordForm::kuruVerb, WordForm::provisionalColloquialNegative },
    { u8"する", u8"しなきゃ", WordForm::suruVerb, WordForm::provisionalColloquialNegative },
    { u8"為る", u8"為なきゃ", WordForm::suruVerb, WordForm::provisionalColloquialNegative },

    //Imperative Negative
    { u8"る", u8"るな", WordForm::godanVerb, WordForm::imperativeNegative },
    { u8"う", u8"うな", WordForm::godanVerb, WordForm::imperativeNegative },
    { u8"つ", u8"つな", WordForm::godanVerb, WordForm::imperativeNegative },
    { u8"す", u8"すな", WordForm::godanVerb, WordForm::imperativeNegative },
    { u8"く", u8"くな", WordForm::godanVerb, WordForm::imperativeNegative },
    { u8"ぐ", u8"ぐな", WordForm::godanVerb, WordForm::imperativeNegative },
    { u8"ぶ", u8"ぶな", WordForm::godanVerb, WordForm::imperativeNegative },
    { u8"む", u8"むな", WordForm::godanVerb, WordForm::imperativeNegative },
    { u8"ぬ", u8"ぬな", WordForm::godanVerb, WordForm::imperativeNegative },
    { u8"る", u8"るな", WordForm::ichidanVerb, WordForm::imperativeNegative },
    { u8"くる", u8"くるな", WordForm::kuruVerb, WordForm::imperativeNegative },
    { u8"来る", u8"来るな", WordForm::kuruVerb, WordForm::imperativeNegative },
    { u8"する", u8"するな", WordForm::suruVerb, WordForm::imperativeNegative },
    { u8"為る", u8"為るな", WordForm::suruVerb, WordForm::imperativeNegative },

    //Tari
    { u8"る", u8"ったり", WordForm::godanVerb, WordForm::tari },
    { u8"う", u8"ったり", WordForm::godanVerb, WordForm::tari },
    { u8"つ", u8"ったり", WordForm::godanVerb, WordForm::tari },
    { u8"す", u8"したり", WordForm::godanVerb, WordForm::tari },
    { u8"く", u8"いたり", WordForm::godanVerb, WordForm::tari },
    { u8"ぐ", u8"いだり", WordForm::godanVerb, WordForm::tari },
    { u8"ぶ", u8"んだり", WordForm::godanVerb, WordForm::tari },
    { u8"む", u8"んだり", WordForm::godanVerb, WordForm::tari },
    { u8"ぬ", u8"んだり", WordForm::godanVerb, WordForm::tari },
    { u8"る", u8"たり", WordForm::ichidanVerb, WordForm::tari },
    { u8"くる", u8"きたり", WordForm::kuruVerb, WordForm::tari },
    { u8"来る", u8"来たり", WordForm::kuruVerb, WordForm::tari },
    { u8"する", u8"したり", WordForm::suruVerb, WordForm::tari },
    { u8"為る", u8"為たり", WordForm::suruVerb, WordForm::tari },
    { u8"行く", u8"行ったり", WordForm::godanVerb, WordForm::tari },
    { u8"問う", u8"問うたり", WordForm::godanVerb, WordForm::tari },
    { u8"請う", u8"請うたり", WordForm::godanVerb, WordForm::tari },

    //Chau
    { u8"る", u8"っちゃう", WordForm::godanVerb, WordForm::chau },
    { u8"う", u8"っちゃう", WordForm::godanVerb, WordForm::chau },
    { u8"つ", u8"っちゃう", WordForm::godanVerb, WordForm::chau },
    { u8"す", u8"しちゃう", WordForm::godanVerb, WordForm::chau },
    { u8"く", u8"いちゃう", WordForm::godanVerb, WordForm::chau },
    { u8"ぐ", u8"いちゃう", WordForm::godanVerb, WordForm::chau },
    { u8"ぶ", u8"んじゃう", WordForm::godanVerb, WordForm::chau },
    { u8"ぬ", u8"んじゃう", WordForm::godanVerb, WordForm::chau },
    { u8"む", u8"んじゃう", WordForm::godanVerb, WordForm::chau },
    { u8"る", u8"ちゃう", WordForm::ichidanVerb, WordForm::chau },
    { u8"くる", u8"きちゃう", WordForm::kuruVerb, WordForm::chau },
    { u8"来る", u8"来ちゃう", WordForm::kuruVerb, WordForm::chau },
    { u8"する", u8"しちゃう", WordForm::suruVerb, WordForm::chau },
    { u8"為る", u8"為ちゃう", WordForm::suruVerb, WordForm::chau },
    { u8"行く", u8"行っちゃう", WordForm::godanVerb, WordForm::chau },
    { u8"問う", u8"問うちゃう", WordForm::godanVerb, WordForm::chau },
    { u8"請う", u8"請うちゃう", WordForm::godanVerb, WordForm::chau },

    //Chimau
    { u8"る", u8"っちまう", WordForm::godanVerb, WordForm::chimau },
    { u8"う", u8"っちまう", WordForm::godanVerb, WordForm::chimau },
    { u8"つ", u8"っちまう", WordForm::godanVerb, WordForm::chimau },
    { u8"す", u8"しちまう", WordForm::godanVerb, WordForm::chimau },
    { u8"く", u8"いちまう", WordForm::godanVerb, WordForm::chimau },
    { u8"ぐ", u8"いちまう", WordForm::godanVerb, WordForm::chimau },
    { u8"ぶ", u8"んじまう", WordForm::godanVerb, WordForm::chimau },
    { u8"ぬ", u8"んじまう", WordForm::godanVerb, WordForm::chimau },
    { u8"む", u8"んじまう", WordForm::godanVerb, WordForm::chimau },
    { u8"る", u8"ちまう", WordForm::ichidanVerb, WordForm::chimau },
    { u8"くる", u8"きちまう", WordForm::kuruVerb, WordForm::chimau },
    { u8"来る", u8"来ちまう", WordForm::kuruVerb, WordForm::chimau },
    { u8"する", u8"しちまう", WordForm::suruVerb, WordForm::chimau },
    { u8"為る", u8"為ちまう", WordForm::suruVerb, WordForm::chimau },
    { u8"行く", u8"行っちまう", WordForm::godanVerb, WordForm::chimau },
    { u8"問う", u8"問うちゃう", WordForm::godanVerb, WordForm::chimau },
    { u8"請う", u8"請うちゃう", WordForm::godanVerb, WordForm::chimau },

    //Continuous
    { u8"で", u8"でいる", WordForm::te, WordForm::continuous },
    { u8"て", u8"ている", WordForm::te, WordForm::continuous },
    { u8"で", u8"でおる", WordForm::te, WordForm::continuous },
    { u8"て", u8"ておる", WordForm::te, WordForm::continuous },
    { u8"で", u8"でる", WordForm::te, WordForm::continuous },
    { u8"て", u8"てる", WordForm::te, WordForm::continuous },
    { u8"て", u8"とる", WordForm::te, WordForm::continuous },

    //Shimau
    { u8"で", u8"でしまう", WordForm::te, WordForm::shimau },
    { u8"て", u8"てしまう", WordForm::te, WordForm::shimau },

    //Adjectives
    { u8"い", u8"くて", WordForm::adjective, WordForm::te },
    { u8"い", u8"く", WordForm::adjective, WordForm::adverbial },
    { u8"い", u8"くない", WordForm::adjective, WordForm::negative },
    { u8"い", u8"かった", WordForm::adjective, WordForm::past },
    { u8"い", u8"ければ", WordForm::adjective, WordForm::ba },
    { u8"い", u8"くなきゃ", WordForm::adjective, WordForm::provisionalColloquialNegative },
    { u8"い", u8"かったら", WordForm::adjective, WordForm::tara },
    { u8"い", u8"さ", WordForm::adjective, WordForm::noun },
    { u8"い", u8"そう", WordForm::adjective, WordForm::sou },
    { u8"い", u8"すぎる", WordForm::adjective, WordForm::sugiru },
    { u8"い", u8"き", WordForm::adjective, WordForm::ki },
    { u8"い", u8"かろう", WordForm::adjective, WordForm::volitional },
    { u8"ない", u8"ねえ", WordForm::adjective, WordForm::e },
    { u8"ない", u8"ねぇ", WordForm::adjective, WordForm::e },
    { u8"ない", u8"ねー", WordForm::adjective, WordForm::e },
    { u8"たい", u8"てえ", WordForm::adjective, WordForm::e },
    { u8"たい", u8"てぇ", WordForm::adjective, WordForm::e },
    { u8"たい", u8"てー", WordForm::adjective, WordForm::e },

    //Conjunctive (Masu stem)
    { u8"る", u8"り", WordForm::godanVerb, WordForm::conjunctive },
    { u8"う", u8"い", WordForm::godanVerb, WordForm::conjunctive },
    { u8"つ", u8"ち", WordForm::godanVerb, WordForm::conjunctive },
    { u8"す", u8"し", WordForm::godanVerb, WordForm::conjunctive },
    { u8"く", u8"き", WordForm::godanVerb, WordForm::conjunctive },
    { u8"ぐ", u8"ぎ", WordForm::godanVerb, WordForm::conjunctive },
    { u8"ぶ", u8"び", WordForm::godanVerb, WordForm::conjunctive },
    { u8"む", u8"み", WordForm::godanVerb, WordForm::conjunctive },
    { u8"ぬ", u8"に", WordForm::godanVerb, WordForm::conjunctive },
    { u8"る", u8"", WordForm::ichidanVerb, WordForm::conjunctive },
    { u8"くる", u8"き", WordForm::kuruVerb, WordForm::conjunctive },
    { u8"来る", u8"来", WordForm::kuruVerb, WordForm::conjunctive },
    { u8"する", u8"し", WordForm::suruVerb, WordForm::conjunctive },
    { u8"為る", u8"為", WordForm::suruVerb, WordForm::conjunctive },

    //Stem forms
    { u8"", u8"ます", WordForm::conjunctive, WordForm::polite },
    { u8"ます", u8"ません", WordForm::polite, WordForm::negative },
    { u8"ます", u8"ました", WordForm::polite, WordForm::past },
    { u8"ます", u8"ましょう", WordForm::polite, WordForm::volitional },
    { u8"せん", u8"せんでした", WordForm::negative, WordForm::past },
    { u8"", u8"たら", WordForm::conjunctive, WordForm::tara },
    { u8"", u8"たい", WordForm::conjunctive, WordForm::tai },
    { u8"", u8"なさい", WordForm::conjunctive, WordForm::nasai },
    { u8"", u8"そう", WordForm::conjunctive, WordForm::sou },
    { u8"", u8"すぎる", WordForm::conjunctive, WordForm::sugiru },
};

static QString wordFormToString(WordForm ruleType)
{
    switch (ruleType)
    {
    case WordForm::godanVerb:
        return "godan verb";
    case WordForm::ichidanVerb:
        return "ichidan verb";
    case WordForm::suruVerb:
        return "suru verb";
    case WordForm::kuruVerb:
        return "kuru verb";
    case WordForm::adjective:
        return "adjective";
    case WordForm::negative:
        return "negative";
    case WordForm::past:
        return "past";
    case WordForm::te:
        return "-te";
    case WordForm::conjunctive:
        return "masu stem";
    case WordForm::volitional:
        return "volitional";
    case WordForm::passive:
        return "passive";
    case WordForm::causative:
        return "causative";
    case WordForm::imperative:
        return "imperative";
    case WordForm::potential:
        return "potential";
    case WordForm::potentialPassive:
        return "potential or passive";
    case WordForm::imperativeNegative:
        return "imperative negative";
    case WordForm::zaru:
        return "-zaru";
    case WordForm::neba:
        return "-neba";
    case WordForm::zu:
        return "-zu";
    case WordForm::nu:
        return "-nu";
    case WordForm::ba:
        return "-ba";
    case WordForm::tari:
        return "-tari";
    case WordForm::shimau:
        return "-shimau";
    case WordForm::chau:
        return "-chau";
    case WordForm::chimau:
        return "-chimau";
    case WordForm::polite:
        return "polite";
    case WordForm::tara:
        return "-tara";
    case WordForm::tai:
        return "-tai";
    case WordForm::nasai:
        return "-nasai";
    case WordForm::sugiru:
        return "-sugiru";
    case WordForm::sou:
        return "-sou";
    case WordForm::e:
        return "-e";
    case WordForm::ki:
        return "-ki";
    case WordForm::toku:
        return "-toku";
    case WordForm::colloquialNegative:
        return "colloquial negative";
    case WordForm::provisionalColloquialNegative:
        return "provisional colloquial negative";
    case WordForm::continuous:
        return "progressive or perfect";
    case WordForm::adverbial:
        return "adv";
    case WordForm::noun:
        return "noun";
    default:
        return "unknown";
    }
}

static bool isTerminalForm(WordForm wordForm)
{
    return wordForm == WordForm::godanVerb ||
        wordForm == WordForm::ichidanVerb ||
        wordForm == WordForm::suruVerb ||
        wordForm == WordForm::kuruVerb ||
        wordForm == WordForm::adjective;
}

static ConjugationInfo createDerivation(
    const ConjugationInfo &parent,
    const Rule &rule)
{
    QList<WordForm> childDerivations(parent.derivations);
    if (childDerivations.isEmpty())
    {
        childDerivations.prepend(rule.conjugatedType);
    }
    childDerivations.prepend(rule.baseType);
    qsizetype replacementStart = parent.base.size() - rule.conjugated.size();
    QString childWord = QString(parent.base)
        .replace(replacementStart, rule.conjugated.size(), rule.base);
    ConjugationInfo childDetails = {
        childWord,
        parent.conjugated,
        childDerivations,
        ""
    };
    return childDetails;
}

static void deconjugateRecursive(
    const ConjugationInfo &info,
    QList<ConjugationInfo> &results)
{
    const QString word = info.base;
    for (const Rule &rule : rules)
    {
        WordForm currentWordForm = info.derivations.empty() ?
            WordForm::any :
            info.derivations[0];
        if (rule.conjugatedType != currentWordForm &&
            currentWordForm != WordForm::any)
        {
            continue;
        }
        if (!word.endsWith(rule.conjugated))
        {
            continue;
        }
        ConjugationInfo childDetails = createDerivation(info, rule);
        if (isTerminalForm(rule.baseType))
        {
            results.emplace_back(childDetails);
            for (const Rule &silentRule : silentRules)
            {
                if (silentRule.conjugatedType != rule.baseType)
                {
                    continue;
                }
                if (!childDetails.base.endsWith(silentRule.base))
                {
                    continue;
                }
                Rule derivedRule = {
                    rule.base,
                    rule.conjugated,
                    silentRule.baseType,
                    rule.conjugatedType
                };
                ConjugationInfo derivedDetails = createDerivation(
                    info,
                    derivedRule
                );
                deconjugateRecursive(derivedDetails, results);
            }
        }
        else
        {
            deconjugateRecursive(childDetails, results);
        }
    }
}

static QString formatDerivation(QList<WordForm> derivations)
{
    QString result;
    QList<WordForm> displayRules;
    std::copy_if(
        derivations.begin(),
        derivations.end(),
        std::back_inserter(displayRules),
        [] (WordForm ruleType)
        {
            if (ruleType == WordForm::conjunctive)
            {
                return false;
            }
            else if (isTerminalForm(ruleType))
            {
                return false;
            }
            return true;
        }
    );
    if (derivations.size() > 0 && derivations.back() == WordForm::conjunctive)
    {
        displayRules.emplace_back(WordForm::conjunctive);
    }

    for (int i = 0; i < displayRules.size(); i++)
    {
        result.append(wordFormToString(displayRules[i]));
        if (i < displayRules.size() - 1)
        {
            result.append(" « ");
        }
    }
    return result;
}

QList<ConjugationInfo> deconjugate(const QString query, bool sentenceMode)
{
    QList<ConjugationInfo> results;
    if (sentenceMode)
    {
        QString word = query;
        while (!word.isEmpty())
        {
            ConjugationInfo detail = { word, word, QList<WordForm>(), "" };
            deconjugateRecursive(detail, results);
            word.chop(1);
        }
    }
    else
    {
        ConjugationInfo detail = { query, query, QList<WordForm>(), ""};
        deconjugateRecursive(detail, results);
    }

    for (int i = 0; i < results.size(); i++)
    {
        results[i].derivationDisplay = formatDerivation(results[i].derivations);
    }

    return results;
}
