////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2026 Ripose
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

#include "dict/data/kanjidefinition.h"

KanjiDefinition::KanjiDefinition(QObject *parent) : QObject(parent)
{

}

KanjiDefinition::~KanjiDefinition()
{

}

KanjiDefinition *KanjiDefinition::clone(QObject *parent) const
{
	KanjiDefinition *copy = new KanjiDefinition(parent);
	copy->setDictionaryInfo(dictionaryInfo()->clone(copy));
	copy->setOnyomi(onyomi());
	copy->setKunyomi(kunyomi());
	copy->setGlossary(glossary());
	for (Tag *tag : tags())
	{
		copy->appendTags(tag->clone(copy));
	}
	for (Tag *tag : stats())
	{
		copy->appendStats(tag->clone(copy));
	}
	for (Tag *tag : classes())
	{
		copy->appendClasses(tag->clone(copy));
	}
	for (Tag *tag : codes())
	{
		copy->appendCodes(tag->clone(copy));
	}
	for (Tag *tag : indices())
	{
		copy->appendIndices(tag->clone(copy));
	}
	return copy;
}

DictionaryInfo *KanjiDefinition::dictionaryInfo() const noexcept
{
	return m_dictionaryInfo;
}

void KanjiDefinition::setDictionaryInfo(DictionaryInfo *value)
{
	if (m_dictionaryInfo == value)
	{
		return;
	}
	if (value)
	{
		value->setParent(this);
	}
	std::swap(m_dictionaryInfo, value);
	emit dictionaryInfoChanged(m_dictionaryInfo);
	if (value)
	{
		value->deleteLater();
	}
}

const QStringList &KanjiDefinition::onyomi() const noexcept
{
	return m_onyomi;
}

void KanjiDefinition::setOnyomi(const QStringList &value)
{
	if (m_onyomi == value)
	{
		return;
	}
	m_onyomi = value;
	emit onyomiChanged();
}

const QStringList &KanjiDefinition::kunyomi() const noexcept
{
	return m_kunyomi;
}

void KanjiDefinition::setKunyomi(const QStringList &value)
{
	if (m_kunyomi == value)
	{
		return;
	}
	m_kunyomi = value;
	emit kunyomiChanged();
}

const QStringList &KanjiDefinition::glossary() const noexcept
{
	return m_glossary;
}

void KanjiDefinition::setGlossary(const QStringList &value)
{
	if (m_glossary == value)
	{
		return;
	}
	m_glossary = value;
	emit glossaryChanged();
}

QQmlListProperty<Tag> KanjiDefinition::tagsQml()
{
	return QQmlListProperty<Tag>(this, &m_tags);
}

const QList<Tag *> &KanjiDefinition::tags() const noexcept
{
	return m_tags;
}

void KanjiDefinition::setTags(const QList<Tag *> &value)
{
	qDeleteAll(m_tags);
	m_tags.clear();
	for (Tag *t : value)
	{
		t->setParent(this);
		m_tags.emplaceBack(t);
	}
	emit tagsChanged();
}

void KanjiDefinition::appendTags(Tag *value)
{
    value->setParent(this);
    m_tags.emplaceBack(value);
    emit tagsChanged();
}

QQmlListProperty<Tag> KanjiDefinition::statsQml()
{
	return QQmlListProperty<Tag>(this, &m_stats);
}

const QList<Tag *> &KanjiDefinition::stats() const noexcept
{
	return m_stats;
}

void KanjiDefinition::setStats(const QList<Tag *> &value)
{
	qDeleteAll(m_stats);
	m_stats.clear();
	for (Tag *t : value)
	{
		t->setParent(this);
		m_stats.emplaceBack(t);
	}
	emit statsChanged();
}

void KanjiDefinition::appendStats(Tag *value)
{
    value->setParent(this);
    m_stats.emplaceBack(value);
    emit statsChanged();
}

QQmlListProperty<Tag> KanjiDefinition::classesQml()
{
	return QQmlListProperty<Tag>(this, &m_classes);
}

const QList<Tag *> &KanjiDefinition::classes() const noexcept
{
	return m_classes;
}

void KanjiDefinition::setClasses(const QList<Tag *> &value)
{
	qDeleteAll(m_classes);
	m_classes.clear();
	for (Tag *t : value)
	{
		t->setParent(this);
		m_classes.emplaceBack(t);
	}
	emit classesChanged();
}

void KanjiDefinition::appendClasses(Tag *value)
{
    value->setParent(this);
    m_classes.emplaceBack(value);
    emit classesChanged();
}

QQmlListProperty<Tag> KanjiDefinition::codesQml()
{
	return QQmlListProperty<Tag>(this, &m_codes);
}

const QList<Tag *> &KanjiDefinition::codes() const noexcept
{
	return m_codes;
}

void KanjiDefinition::setCodes(const QList<Tag *> &value)
{
	qDeleteAll(m_codes);
	m_codes.clear();
	for (Tag *t : value)
	{
		t->setParent(this);
		m_codes.emplaceBack(t);
	}
	emit codesChanged();
}

void KanjiDefinition::appendCodes(Tag *value)
{
    value->setParent(this);
    m_codes.emplaceBack(value);
    emit codesChanged();
}

QQmlListProperty<Tag> KanjiDefinition::indicesQml()
{
	return QQmlListProperty<Tag>(this, &m_indices);
}

const QList<Tag *> &KanjiDefinition::indices() const noexcept
{
	return m_indices;
}

void KanjiDefinition::setIndices(const QList<Tag *> &value)
{
	qDeleteAll(m_indices);
	m_indices.clear();
	for (Tag *t : value)
	{
		t->setParent(this);
		m_indices.emplaceBack(t);
	}
	emit indicesChanged();
}

void KanjiDefinition::appendIndices(Tag *value)
{
    value->setParent(this);
    m_indices.emplaceBack(value);
    emit indicesChanged();
}
