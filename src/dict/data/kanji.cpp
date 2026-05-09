#include "dict/data/kanji.h"

Kanji::Kanji(QObject *parent) : Expression(parent)
{

}

Kanji::~Kanji()
{

}

Kanji *Kanji::clone(QObject *parent) const
{
	Kanji *copy = new Kanji(parent);

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

	/* Copy Kanji specific */
	copy->setCharacter(character());
	for (KanjiDefinition *d : definitions())
	{
		copy->appendDefinitions(d->clone(copy));
	}
	copy->setAddable(addable());

	return copy;
}

const QString &Kanji::character() const noexcept
{
	return m_character;
}

void Kanji::setCharacter(const QString &value)
{
	if (m_character == value)
	{
		return;
	}
	m_character = value;
	emit characterChanged(m_character);
}

QQmlListProperty<KanjiDefinition> Kanji::definitionsQml()
{
	return QQmlListProperty<KanjiDefinition>(this, &m_definitions);
}

const QList<KanjiDefinition *> &Kanji::definitions() const noexcept
{
	return m_definitions;
}

void Kanji::setDefinitions(const QList<KanjiDefinition *> &value)
{
	qDeleteAll(m_definitions);
	m_definitions.clear();
	for (KanjiDefinition *d : value)
	{
		d->setParent(this);
		m_definitions.emplaceBack(d);
	}
	emit definitionsChanged();
}

void Kanji::appendDefinitions(KanjiDefinition *value)
{
    value->setParent(this);
    m_definitions.emplaceBack(value);
    emit frequenciesChanged();
}

bool Kanji::addable() const noexcept
{
	return m_addable;
}

void Kanji::setAddable(bool value)
{
	if (m_addable == value)
	{
		return;
	}
	m_addable = value;
	emit addableChanged(value);
}
