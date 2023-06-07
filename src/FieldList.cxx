
#include "parham.hxx"
#include "FieldList.hxx"

/*
   FieldListItem
   */

FieldListItem::FieldListItem (AttributeDeclaration *attribute)
{
	INITHIS (attribute);
	status = FS_FIELD;
	if (!attribute->type)
		makeType ();
}

bool FieldListItem::match (FieldListItem *fli)
{
	return attribute->identifier->match (fli->attribute->identifier);
}

void FieldListItem::markDuplicate ()
{
	switch (status)
	{
		case FS_FIELD:
			status = FS_DUPLICATE;
		case FS_DUPLICATE:
			return;
		case FS_TYPE:
			attribute->error ("%s is declared in more than one class. Therefore, it cannot be used as class replacement", attribute->identifier->value);
	}
}

InternalType *FieldListItem::makeType ()
{
	InternalType *it;
	ComponentType *component;
	ClassType *st;

	switch (status)
	{
		case FS_DUPLICATE:
			attribute->error ("%s is declared in more than one class. Therefore, it cannot be used as class replacement", attribute->identifier->value);
			return NULL;
		case FS_TYPE:
			return (InternalType *) attribute->type;
		case FS_FIELD:
			break;
	}

	component = attribute->component;
	it = new InternalType (attribute->identifier);
	if (attribute->type)
		switch (attribute->type->which)
		{
			case Type::TYPE_CLASS:
				st = (ClassType *)attribute->type;
				if (st->isInternal)
				{
					it->setBase((InternalType *) st);
					break;
				}
			default:
				attribute->error ("%s: The type of the attribute should be internal class in order to be used as a class replacement", attribute->identifier->value);
				return NULL;
		}
	attribute->type = it;
	status = FS_TYPE;
	component->addInternal (it);
	return it;
}

bool FieldListItem::isType ()
{
	return status == FieldListItem::FS_TYPE;
}

/*
   FieldList
   */

FieldListItem *FieldList::find (Identifier *identifier)
{
	int i;

	for (i = 0; i < list.count; ++i)
		if (identifier->match (list.array[i]->attribute->identifier))
			return list.array[i];
	return NULL;
}

void FieldList::flAdd (AttributeDeclaration *attribute)
{
	FieldListItem *fli;

	fli = find (attribute->identifier);
	if (fli)
		fli->markDuplicate ();
	list.add (new FieldListItem (attribute));
}

InternalType *FieldList::flMakeType (Identifier *identifier)
{
	FieldListItem *fli;

	fli = find (identifier);
	if (fli)
		return fli->makeType ();
	identifier->error ("%s: There is no field with this name", identifier->value);
	return NULL;
}

Type *FieldList::flLookup (Identifier *identifier)
{
	FieldListItem *fli;

	fli = find (identifier);
	if (fli)
		return fli->attribute->type;
	return NULL;
}

bool FieldList::flIsType (Identifier *identifier)
{
	FieldListItem *fli;

	fli = find (identifier);
	if (!fli)
		return false;
	return fli->isType ();
}


