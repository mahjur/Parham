
#include "parham.hxx"

Identifier::Identifier (const char *_value, ComponentType *_component, int _line)
	: NT (_component, _line)
{
	INIT (value);
}

QualifiedIdentifier::QualifiedIdentifier (Identifier *identifier)
	: NT (identifier->component, identifier->line)
{
	list.add (identifier);
}

void QualifiedIdentifier::add (Identifier *identifier)
{
	list.add (identifier);
}

bool Identifier::match (Identifier *identifier)
{
	return !strcmp (value, identifier->value);
}

void Identifier::publish (FILE *f)
{
	fprintf (f, "%s", value);
}

void QualifiedIdentifier::flat (char *buffer, int count, char sep)
{
	int i; char *point;

	if (list.count < count)
		count = list.count;

	point = buffer;
	for (i = 0; i < count; ++i)
	{
		if (i)
			*(point++) = sep;
		point += sprintf (point, "%s", list.array[i]->value);
	}
}

void QualifiedIdentifier::flat (char *buffer, char sep)
{
	flat (buffer, list.count, sep);
	/*
	int i; char *point;

	point = buffer;
	for (i = 0; i < list.count; ++i)
	{
		if (i)
			*(point++) = sep;
		point += sprintf (point, "%s", list.array[i]->value);
	}
	*/
}

bool QualifiedIdentifier::match (QualifiedIdentifier *qid, int count)
{
	int i;

	if (list.count < count)
		return false;
	if (qid->list.count < count)
		return false;
	for (i = 0; i < count; ++i)
		if (!list.array[i]->match (qid->list.array[i]))
			return false;
	return true;
}

Identifier::Identifier (Identifier *identifier)
	: NT (identifier)
{
	value = identifier->value;
}

bool QualifiedIdentifier::same (QualifiedIdentifier *qid)
{
	int i;

	if (list.count != qid->list.count)
		return false;
	for (i = 0; i < list.count; ++i)
		if (!list.array[i]->match (qid->list.array[i]))
			return false;
	return true;
}

bool QualifiedIdentifier::match (Identifier *identifier)
{
	if (list.count != 1)
		return false;
	return list.array[0]->match (identifier);
}

void Identifier::erUndefined ()
{
	error ("%s: undefined identifier", value);
}

void Identifier::erAmbigious ()
{
	error ("%s: ambigious identifier", value);
}

void Identifier::erDuplicate ()
{
	error ("%s: duplicate identifier", value);
}

