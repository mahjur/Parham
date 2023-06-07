
#include "parham.hxx"

ComponentType *curComponent;

ComponentType::ComponentType (const char *_fname)
	: ContainerType (TYPE_COMPONENT, new Identifier ("", this, 1), &rm)
{
	INIT (fname);
	package = NULL;
	error = false;
	blockStatementCount = 0;
}

void ComponentType::setPackage (QualifiedIdentifier *_package)
{
	INIT (package);
}

void ComponentType::addImport (QualifiedIdentifier *import)
{
	importList.add (import);
}

void ComponentType::addInterface (InterfaceType *intf)
{
	if (rm.addType (intf))
		interface.add (intf);
	else
		intf->identifier->erDuplicate ();
}

void ComponentType::addInternal (InternalType *inter)
{
	if (rm.addType (inter))
		internal.add (inter);
	else
		inter->identifier->erDuplicate ();
}

void ComponentType::setName (QualifiedIdentifier *_qname)
{
	char buffer[1024];

	INIT (qname);
	qname->flat (buffer, '.');
	if (strcmp (buffer, fname))
		qname->error ("the component name does not match the name of the file: %s", fname);
	qname->flat (buffer, '_');
	pname = strdup (buffer);
}

bool ComponentType::publish (const char *mainfunction)
{
	return publishHeader () && publishSource (mainfunction);
}

bool ComponentType::publishHeader ()
{
	FILE *f;
	char file[1024];
	int i;

	sprintf (file, "%s.h", fname);
	f = fopen (file, "w");
	if (!f)
		return false;

	fprintf (f, "\n");
	fprintf (f, "#ifndef __%s__h__\n", fname);
	fprintf (f, "#define __%s__h__\n", fname);
	fprintf (f, "\n");

	for (i = 0; i < ordered.count; ++i)
		ordered.array[i]->publishName (f);
	if (ordered.count > 0)
		fprintf (f, "\n");

	for (i = 0; i < item.count; ++i)
		item.array[i]->publishHeader (f);
	fprintf (f, "\n");

	for (i = 0; i < ordered.count; ++i)
		ordered.array[i]->publishHeader (f);

	fprintf (f, "#endif\n");
	fprintf (f, "\n");
	fclose (f);
	return true;
}

bool ComponentType::publishSource (const char *mainfunction)
{
	FILE *f;
	char file[1024];
	int i;

	/**/
	sprintf (file, "%s.c", fname);
	f = fopen (file, "w");
	if (!f)
		return false;

	fprintf (f, "\n");
	fprintf (f, "#include <stdio.h>\n");
	fprintf (f, "#include <stdlib.h>\n");
	fprintf (f, "#include \"%s.h\"\n", fname);
	fprintf (f, "\n");
	fprintf (f, "#define PARHAMUP(type,field,ptr)  ((parhamptr=(ptr)),((parhamptr)?(type *)((long int)(parhamptr) - (long int)(&((type *)NULL)->field)):NULL))\n\n");

	for (i = 0; i < ordered.count; ++i)
		ordered.array[i]->publishVT (f);
	for (i = 0; i < ordered.count; ++i)
		ordered.array[i]->publishSource (f);
	for (i = 0; i < item.count; ++i)
		item.array[i]->publishSource (f);

	publishMainFunction (f, mainfunction);

	fclose (f);
	return true;
}

void ComponentType::publishMainFunction (FILE *f, const char *mainfunction)
{
	ClassType *mainClass;
	ExecutionDeclaration *constructor;
	int i;

	if (interface.count != 1)
		return;
	if (!interface.array[0]->mainSet)
		return;
	mainClass = interface.array[0];
	if (mainClass->cm.constructorList.list.count != 1)
	{
		mainClass->error ("main class must have just one constructor");
		return;
	}
	constructor = mainClass->cm.constructorList.list.array[0];

	// making main function to call constructor of the main class
	fprintf (f, "void %s ", mainfunction);
	constructor->argList->publish (f, mainClass, false);
	fprintf (f, "\n{\n");

	fprintf (f, "\t%s (", constructor->pname);
	for (i = 0; i < constructor->argList->list.count; ++i)
	{
		if (i)
			fprintf (f, ", ");
		fprintf (f, "%s", constructor->argList->list.array[i]->identifier->value);
	}
	fprintf (f, ");\n");
//	fprintf (f, "\treturn 0;\n");
	fprintf (f, "}\n");
	fprintf (f, "\n");
}

void ComponentType::addItem (Declaration *mi)
{
	item.add (mi);
}

void ComponentType::formSet ()
{
	int i;

	/*
	   // This was moved to ClassType constructor.
	for (i = 0; i < interface.count; ++i)
		interface.array[i]->setPName ();
	for (i = 0; i < internal.count; ++i)
		internal.array[i]->setPName ();
	*/

	for (i = 0; i < item.count; ++i)
		item.array[i]->formSet ();
	reorder ();
	for (i = 0; i < ordered.count; ++i)
		ordered.array[i]->markVirtual ();

	for (i = 0; i < interface.count; ++i)
		interface.array[i]->formSet ();
	for (i = 0; i < internal.count; ++i)
		internal.array[i]->formSet ();
}

void ComponentType::mapSet ()
{
	int i;

	for (i = 0; i < item.count; ++i)
		item.array[i]->mapSet ();
}

void ComponentType::analysis ()
{
	int i;

	for (i = 0; i < ordered.count; ++i)
		ordered.array[i]->analysis ();
	for (i = 0; i < propertyList.count; ++i)
		propertyList.array[i]->analysis ();
}

bool ComponentType::reorder ()
{
	bool *interfaceFlag;
	bool *internalFlag;
	int i;
	bool redo;
	bool ret;

	interfaceFlag = (bool *) malloc (interface.count * sizeof (bool));
	internalFlag = (bool *) malloc (internal.count * sizeof (bool));

	for (i = 0; i < interface.count; ++i)
		interfaceFlag[i] = false;
	for (i = 0; i < internal.count; ++i)
		internalFlag[i] = false;

	for (redo = true; redo; )
	{
		redo = false;
		for (i = 0; i < interface.count; ++i)
		{
			if (!interfaceFlag[i])
			{
				if (interface.array[i]->satisfied (&ordered))
				{
					ordered.add (interface.array[i]);
					redo = true;
					interfaceFlag[i] = true;
				}
			}
		}
		for (i = 0; i < internal.count; ++i)
		{
			if (!internalFlag[i])
			{
				if (internal.array[i]->satisfied (&ordered))
				{
					ordered.add (internal.array[i]);
					redo = true;
					internalFlag[i] = true;
				}
			}
		}
	}

	ret = true;
	for (i = 0; i < interface.count; ++i)
		if (!interfaceFlag[i])
		{
			interface.array[i]->error ("Cycle detected in the inclusion relation declaration for %s", interface.array[i]->identifier->value);
			ret = false;
		}
	for (i = 0; i < internal.count; ++i)
		if (!internalFlag[i])
		{
			internal.array[i]->error ("Cycle detected in the inclusion relation declaration for %s", internal.array[i]->identifier->value);
			ret = false;
		}
	free (interfaceFlag);
	free (internalFlag);
	return ret;
}

Type *ComponentType::lookupType (Identifier *identifier)
{
	return rm.lookupType (identifier);
}

bool ComponentType::accept (InstanceDeclaration *mi)
{
	int i;

	if (interface.count != mi->mapList->count)
	{
		mi->error ("number of interface sets does not match the number of interface sets of the component: %s", fname);
		return false;
	}
	for (i = 0; i < interface.count; ++i)
		if (interface.array[i]->stateList->array.count != mi->mapList->array[i]->stateMap->count)
		{
			mi->error ("number of states defined for the %dth interface set is wrong in the component instance of %s", i + 1, fname);
			return false;
		}
	return true;
}

void ComponentType::addType (NamedType *type)
{
	rm.addType (type);
}

bool ComponentType::addAttribute (Declaration *attrib)
{
	return rm.addAttribute (attrib);
	/*
	if (!attrib->identifier)
		rm.ns.addExtension ((InstanceDeclaration *) attrib);
	else if (!rm.ns.addSimple (attrib))
	{
		attrib->identifier->erDuplicate ();
		return false;
	}
	return true;
	*/
}

/*
bool ComponentType::accept (Type *type, ConvertPath *castPath)
{
	if (which != type->which)
		return false;
	return interface.count == ((ComponentType *) type)->interface.count;
}
*/

void ComponentType::addInstance (InstanceDeclaration *instance)
{
	addAttribute (instance);
	instanceList.add (instance);
}

/*
InstanceDeclaration *ComponentType::findInstance (QualifiedIdentifier *qid, int count)
{
	int i;
	InstanceDeclaration *ret;

	ret = NULL;
	for (i = 0; i < instanceList.count; ++i)
	{
		if (instanceList.array[i]->qid->match (qid, count))
		{
#ifdef DEBUG
			{
				char b1[1024], b2[1024];
				qid->flat (b1, '.');
				instanceList.array[i]->qid->flat (b2, '.');
				fprintf (stderr, "matching (%s) (index=%d) with the given name (%s)\n", b2, i, b1);
			}
#endif
			if (ret)
			{
				char b1[1024], b2[1024];
				qid->flat (b1, '.');
				instanceList.array[i]->qid->flat (b2, '.');
				error ("multiple instances are matched (%s) with the given name (%s)", b2, b1);
				return ret;
			}
			ret = instanceList.array[i];
		}
	}

	if (count == 1)
	{
		for (i = 0; i < instanceList.count; ++i)
		{
			if (instanceList.array[i]->identifier && instanceList.array[i]->identifier->match (qid->list.array[0]))
			{
				if (ret)
				{
					error ("multiple instances are matched with the given name");
					return ret;
				}
				ret = instanceList.array[i];
			}
		}
	}
	return ret;
}
*/

void ComponentType::addVirtual (MethodDeclaration *method)
{
	method->error ("%s: components cannot have requires methods", method->pname);
}

bool ComponentType::same (ComponentType *component)
{
	return qname->same (component->qname);
}

// FIXME: move to ResolveManager
InstanceDeclaration *ComponentType::lookupInstance (Identifier *identifier)
{
	int i;
	InstanceDeclaration *ret;

	ret = NULL;
	for (i = 0; i < instanceList.count; ++i)
	{
		if (instanceList.array[i]->matchIdentifier (identifier))
		{
			if (ret)
				identifier->error ("%s: More than one instances match the given identifier");
			ret = instanceList.array[i];
		}
	}
	if (ret)
		return ret;
	for (i = 0; i < instanceList.count; ++i)
	{
		if (instanceList.array[i]->matchComponent (identifier))
		{
			if (ret)
				identifier->error ("%s: More than one instances match the given identifier");
			ret = instanceList.array[i];
		}
	}
	return ret;
}

bool ComponentType::hasInclusion (ClassType *super, ClassType *sub)
{
	int i;

	for (i = 0; i < item.count; ++i)
		if (item.array[i]->which == Declaration::DECL_INCLUSION)
			if (((InclusionDeclaration *)item.array[i])->hasInclusion (super, sub))
				return true;
	return false;
}
void ComponentType::enterBlock ()
{
	blockStatementCount++;
}

void ComponentType::leaveBlock ()
{
	blockStatementCount--;
}

bool ComponentType::isInBlock ()
{
	return blockStatementCount > 0;
}


