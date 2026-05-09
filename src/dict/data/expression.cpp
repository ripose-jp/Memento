////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2021 Ripose
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

#include "dict/data/expression.h"

Expression::Expression(QObject *parent) : QObject(parent)
{

}

Expression::~Expression()
{

}

Expression *Expression::clone(QObject *parent) const
{
	Expression *copy = new Expression(parent);
	copy->setTitle(title());
	copy->setSubtitle2(subtitle());
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
	return copy;
}

const QString &Expression::title() const noexcept
{
	return m_title;
}

void Expression::setTitle(const QString &value)
{
	if (m_title == value)
	{
		return;
	}
	m_title = value;
	emit titleChanged(m_title);
}

const QString &Expression::subtitle() const noexcept
{
	return m_subtitle;
}

void Expression::setSubtitle(const QString &value)
{
	if (m_subtitle == value)
	{
		return;
	}
	m_subtitle = value;
	emit subtitleChanged(m_subtitle);
}

double Expression::startTime() const noexcept
{
	return m_startTime;
}

void Expression::setStartTime(double value)
{
	if (m_startTime == value)
	{
		return;
	}
	m_startTime = value;
	emit startTimeChanged(m_startTime);
}

double Expression::endTime() const noexcept
{
	return m_endTime;
}

void Expression::setEndTime(double value)
{
	if (m_endTime == value)
	{
		return;
	}
	m_endTime = value;
	emit endTimeChanged(m_endTime);
}

const QString &Expression::subtitle2() const noexcept
{
	return m_subtitle2;
}

void Expression::setSubtitle2(const QString &value)
{
	if (m_subtitle2 == value)
	{
		return;
	}
	m_subtitle2 = value;
	emit subtitle2Changed(m_subtitle2);
}

double Expression::contextStartTime() const noexcept
{
	return m_contextStartTime;
}

void Expression::setContextStartTime(double value)
{
	if (m_contextStartTime == value)
	{
		return;
	}
	m_contextStartTime = value;
	emit contextStartTimeChanged(m_contextStartTime);
}

double Expression::contextEndTime() const noexcept
{
	return m_contextEndTime;
}

void Expression::setContextEndTime(double value)
{
	if (m_contextEndTime == value)
	{
		return;
	}
	m_contextEndTime = value;
	emit contextEndTimeChanged(m_contextEndTime);
}

const QString &Expression::context() const noexcept
{
	return m_context;
}

void Expression::setContext(const QString &value)
{
	if (m_context == value)
	{
		return;
	}
	m_context = value;
	emit contextChanged(m_context);
}

const QString &Expression::context2() const noexcept
{
	return m_context2;
}

void Expression::setContext2(const QString &value)
{
	if (m_context2 == value)
	{
		return;
	}
	m_context2 = value;
	emit context2Changed(m_context2);
}

const QString &Expression::clipboard() const noexcept
{
	return m_clipboard;
}

void Expression::setClipboard(const QString &value)
{
	if (m_clipboard == value)
	{
		return;
	}
	m_clipboard = value;
	emit clipboardChanged(m_clipboard);
}

const QString &Expression::clozeBody() const noexcept
{
	return m_clozeBody;
}

void Expression::setClozeBody(const QString &value)
{
	if (m_clozeBody == value)
	{
		return;
	}
	m_clozeBody = value;
	emit clozeBodyChanged(m_clozeBody);
}

const QString &Expression::clozePrefix() const noexcept
{
	return m_clozePrefix;
}

void Expression::setClozePrefix(const QString &value)
{
	if (m_clozePrefix == value)
	{
		return;
	}
	m_clozePrefix = value;
	emit clozePrefixChanged(m_clozePrefix);
}

const QString &Expression::clozeSuffix() const noexcept
{
	return m_clozeSuffix;
}

void Expression::setClozeSuffix(const QString &value)
{
	if (m_clozeSuffix == value)
	{
		return;
	}
	m_clozeSuffix = value;
	emit clozeSuffixChanged(m_clozeSuffix);
}

QQmlListProperty<Frequency> Expression::frequenciesQml()
{
	return QQmlListProperty<Frequency>(this, &m_frequencies);
}

const QList<Frequency *> &Expression::frequencies() const noexcept
{
	return m_frequencies;
}

void Expression::setFrequencies(const QList<Frequency *> &value)
{
	qDeleteAll(m_frequencies);
	m_frequencies.clear();
	for (Frequency *f : value)
	{
		f->setParent(this);
		m_frequencies.emplaceBack(f);
	}
	emit frequenciesChanged();
}

void Expression::appendFrequencies(Frequency *value)
{
    value->setParent(this);
    m_frequencies.emplaceBack(value);
    emit frequenciesChanged();
}

bool Expression::ankiChecked() const noexcept
{
	return m_ankiChecked;
}

void Expression::setAnkiChecked(bool value)
{
	if (m_ankiChecked == value)
	{
		return;
	}
	m_ankiChecked = value;
	emit ankiCheckedChanged(m_ankiChecked);
}

bool Expression::ankiAdding() const noexcept
{
	return m_ankiAdding;
}

void Expression::setAnkiAdding(bool value)
{
	if (m_ankiAdding == value)
	{
		return;
	}
	m_ankiAdding = value;
	emit ankiAddingChanged(m_ankiAdding);
}
