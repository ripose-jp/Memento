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

#include "dict/data/term.h"

Term::Term(QObject *parent) : Expression(parent)
{

}

Term::~Term()
{

}

Term *Term::clone(QObject *parent) const
{
	Term *copy = new Term(parent);

	/* Copy Expression base fields */
	copy->setTitle(title());
	copy->setSubtitle(subtitle());
	copy->setStartTime(startTime());
	copy->setEndTime(endTime());
	copy->setSubtitle2(subtitle2());
	copy->setContextStartTime(contextStartTime());
	copy->setContextEndTime(contextEndTime());
	copy->setContext(context());
	copy->setContext2(context2());
	copy->setClipboard(clipboard());
	copy->setClozeBody(clozeBody());
	copy->setClozePrefix(clozePrefix());
	copy->setClozeSuffix(clozeSuffix());
	for (Frequency *f : frequencies())
	{
		copy->appendFrequencies(f->clone(copy));
	}
	copy->setAnkiChecked(ankiChecked());
	copy->setAnkiAdding(ankiAdding());

	/* Copy Term specific fields */
	copy->setExpression(expression());
	copy->setReading(reading());
	copy->setConjugationExplanation(conjugationExplanation());
	copy->setReadingAsExpression(readingAsExpression());
	for (Tag *t : tags())
	{
		copy->appendTags(t->clone(copy));
	}
	for (Pitch *p : pitches())
	{
		copy->appendPitches(p->clone(copy));
	}
	for (TermDefinition *d : definitions())
	{
		copy->appendDefinitions(d->clone(copy));
	}
	copy->setSelection(selection());
	copy->setScore(score());
	copy->setAudioSourceName(audioSourceName());
	copy->setAudioUrl(audioUrl());
	copy->setAudioSkipHash(audioSkipHash());
	copy->setCollapsed(collapsed());
	copy->setAutoPlay(autoPlay());
	copy->setAddableExpression(addableExpression());
	copy->setAddableReading(addableReading());

	return copy;
}

const QString &Term::expression() const noexcept
{
	return m_expression;
}

void Term::setExpression(const QString &value)
{
	if (m_expression == value)
	{
		return;
	}
	m_expression = value;
	emit expressionChanged(m_expression);
}

const QString &Term::reading() const noexcept
{
	return m_reading;
}

void Term::setReading(const QString &value)
{
	if (m_reading == value)
	{
		return;
	}
	m_reading = value;
	emit readingChanged(m_reading);
}

const QString &Term::conjugationExplanation() const noexcept
{
	return m_conjugationExplanation;
}

void Term::setConjugationExplanation(const QString &value)
{
	if (m_conjugationExplanation == value)
	{
		return;
	}
	m_conjugationExplanation = value;
	emit conjugationExplanationChanged(m_conjugationExplanation);
}

bool Term::readingAsExpression() const noexcept
{
	return m_readingAsExpression;
}

void Term::setReadingAsExpression(bool value)
{
	if (m_readingAsExpression == value)
	{
		return;
	}
	m_readingAsExpression = value;
	emit readingAsExpressionChanged(m_readingAsExpression);
}

QQmlListProperty<Tag> Term::tagsQml()
{
	return QQmlListProperty<Tag>(this, &m_tags);
}

const QList<Tag *> &Term::tags() const noexcept
{
	return m_tags;
}

void Term::setTags(const QList<Tag *> &value)
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

void Term::appendTags(Tag *value)
{
    value->setParent(this);
    m_tags.emplaceBack(value);
    emit tagsChanged();
}

QQmlListProperty<Pitch> Term::pitchesQml()
{
	return QQmlListProperty<Pitch>(this, &m_pitches);
}

const QList<Pitch *> &Term::pitches() const noexcept
{
	return m_pitches;
}

void Term::setPitches(const QList<Pitch *> &value)
{
	qDeleteAll(m_pitches);
	m_pitches.clear();
	for (Pitch *p : value)
	{
		p->setParent(this);
		m_pitches.emplaceBack(p);
	}
	emit pitchesChanged();
}

void Term::appendPitches(Pitch *value)
{
    value->setParent(this);
    m_pitches.emplaceBack(value);
    emit pitchesChanged();
}

QQmlListProperty<TermDefinition> Term::definitionsQml()
{
	return QQmlListProperty<TermDefinition>(this, &m_definitions);
}

const QList<TermDefinition *> &Term::definitions() const noexcept
{
	return m_definitions;
}

void Term::setDefinitions(const QList<TermDefinition *> &value)
{
	qDeleteAll(m_definitions);
	m_definitions.clear();
	for (TermDefinition *d : value)
	{
		d->setParent(this);
		m_definitions.emplaceBack(d);
	}
	emit definitionsChanged();
}

void Term::appendDefinitions(TermDefinition *value)
{
    value->setParent(this);
    m_definitions.emplaceBack(value);
    emit definitionsChanged();
}

const QStringList &Term::selection() const noexcept
{
	return m_selection;
}

void Term::setSelection(const QStringList &value)
{
	if (m_selection == value)
	{
		return;
	}
	m_selection = value;
	emit selectionChanged();
}

int Term::score() const noexcept
{
	return m_score;
}

void Term::setScore(int value)
{
	if (m_score == value)
	{
		return;
	}
	m_score = value;
	emit scoreChanged(m_score);
}

const QString &Term::audioSourceName() const noexcept
{
	return m_audioSourceName;
}

void Term::setAudioSourceName(const QString &value)
{
	if (m_audioSourceName == value)
	{
		return;
	}
	m_audioSourceName = value;
	emit audioSourceNameChanged(m_audioSourceName);
}

const QString &Term::audioUrl() const noexcept
{
	return m_audioUrl;
}

void Term::setAudioUrl(const QString &value)
{
	if (m_audioUrl == value)
	{
		return;
	}
	m_audioUrl = value;
	emit audioUrlChanged(m_audioUrl);
}

const QString &Term::audioSkipHash() const noexcept
{
	return m_audioSkipHash;
}

void Term::setAudioSkipHash(const QString &value)
{
	if (m_audioSkipHash == value)
	{
		return;
	}
	m_audioSkipHash = value;
	emit audioSkipHashChanged(m_audioSkipHash);
}

bool Term::collapsed() const noexcept
{
	return m_collapsed;
}

void Term::setCollapsed(bool value)
{
	if (m_collapsed == value)
	{
		return;
	}
	m_collapsed = value;
	emit collapsedChanged(m_collapsed);
}

bool Term::autoPlay() const noexcept
{
	return m_autoPlay;
}

void Term::setAutoPlay(bool value)
{
	if (m_autoPlay == value)
	{
		return;
	}
	m_autoPlay = value;
	emit autoPlayChanged(m_autoPlay);
}

bool Term::addableExpression() const noexcept
{
	return m_addableExpression;
}

void Term::setAddableExpression(bool value)
{
	if (m_addableExpression == value)
	{
		return;
	}
	m_addableExpression = value;
	emit addableExpressionChanged(m_addableExpression);
}

bool Term::addableReading() const noexcept
{
	return m_addableReading;
}

void Term::setAddableReading(bool value)
{
	if (m_addableReading == value)
	{
		return;
	}
	m_addableReading = value;
	emit addableReadingChanged(m_addableReading);
}
