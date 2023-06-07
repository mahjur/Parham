
#include "parham.hxx"

Polymorphic::Polymorphic ()
{
}

Polymorphic::Polymorphic (ExecutionDeclaration *decl)
{
	list.add (decl);
}

int Polymorphic::add (ExecutionDeclaration *decl)
{
	int i;

	for (i = 0; i < list.count; ++i)
	{
		if (list.array[i]->type->equal (decl->type))
		{
			if (decl->body)
			{
				if (list.array[i]->body)
					decl->error ("redefinition of the %s (previous one at %d)", decl->identifier->value, list.array[i]->line);
				else
					list.array[i] = decl;
			}
			return i + 1;
		}
	}
	list.add (decl);
	return list.count;
}

Declaration *Polymorphic::lookup (ExpressionList *argList)
{
	int i;
	Declaration *ret;

	if (!argList)
		return NULL;

	ret = NULL;
	for (i = 0; i < list.count; ++i)
		if (((ExecutionType *) list.array[i]->type)->accept (argList))
		{
			if (ret)
			{
				argList->error ("ambigious call");
				break;
			}
			ret = list.array[i];
		}
	return ret;
}

ExecutionDeclaration *Polymorphic::lookup (ExecutionType *methodType)
{
	int i;

	for (i = 0; i < list.count; ++i)
		if (list.array[i]->type->equal (methodType))
			return (MethodDeclaration *) list.array[i];
	return NULL;
}

/*
ExecutionDeclaration *Polymorphic::lookupUniqueMethod ()
{
//	ExecutionDeclaration *ex;

	if (list.count != 1)
		return NULL;
	ex = list.array[0];
	if (ex->modifier->has (MOD_EXTERN))
		return NULL;
	return list.array[0];
}
*/

// Symbol

Symbol::Symbol (SymbolType _which, Identifier *_identifier)
//Symbol::Symbol (int _which, Identifier *_identifier)
{
	INIT (which);
	INIT (identifier);
}

bool Symbol::match (Identifier *id)
{
	return identifier->match (id);
}

bool Symbol::match (Symbol *symbol)
{
	return identifier->match (symbol->identifier);
}

Declaration *Symbol::lookup (ExpressionList *argList)
{
	return NULL;
}

/* MHJ
ExecutionDeclaration *Symbol::lookup (ExecutionType *methodType)
{
	return NULL;
}
*/

// SimpleSymbol

SimpleSymbol::SimpleSymbol (Declaration *_declaration)
	: Symbol (SYM_SIMPLE, _declaration->identifier)
{
	INIT (declaration);
}

Declaration *SimpleSymbol::lookup (ExpressionList *argList)
{
	return declaration;
}

// PolymorphicSymbol

PolymorphicSymbol::PolymorphicSymbol (ExecutionDeclaration *decl)
	: Symbol (SYM_POLYMORPHIC, decl->identifier), poly (decl)
{
//	list.add (decl);
}

Declaration *PolymorphicSymbol::lookup (ExpressionList *argList)
{
	return poly.lookup (argList);
}

int PolymorphicSymbol::add (ExecutionDeclaration *decl)
{
	return poly.add (decl);
}

/*
ExecutionDeclaration *PolymorphicSymbol::lookup (ExecutionType *methodType)
{
	return poly.lookup (methodType);
}
*/

// TypeSymbol

TypeSymbol::TypeSymbol (NamedType *_type)
	: Symbol (SYM_TYPE, _type->identifier)
{
	INIT (type);
}

Declaration *TypeSymbol::lookup (ExpressionList *argList)
{
	identifier->error ("%s does not denote a declaration (it is a type)", identifier->value);
	return NULL;
}

// NameSpace

SimpleSymbol *NameSpace::addSimple (Declaration *decl)
{
	SimpleSymbol *ds;

	ds = new SimpleSymbol (decl);
	if (list.add (ds))
		return ds;
	delete ds;
	return NULL;
}

// BlockSpace

BlockSpace::BlockSpace (BlockSpace *_parent)
{
	INIT (parent);
}

Declaration *BlockSpace::lookup (Identifier *identifier)
{
	int i;

	for (i = 0; i < list.array.count; ++i)
		if (list.array.array[i]->match (identifier))
			return ((SimpleSymbol *)list.array.array[i])->declaration;
	if (parent)
		return parent->lookup (identifier);
	return NULL;
}

// ContainerSpace

int ContainerSpace::addPolymorphic (MethodDeclaration *decl)
{
	PolymorphicSymbol *md;
	Symbol *symbol;

	symbol = list.lookup (decl->identifier);
	if (symbol)
	{
		if (symbol->which == Symbol::SYM_POLYMORPHIC)
			return ((PolymorphicSymbol *) symbol)->add (decl);
		return 0;
	}
	md = new PolymorphicSymbol (decl);
	if (list.add (md))
		return 1;
	delete md;
	return 0;
}

// ClassSpace

ClassSpace::ClassSpace ()
{
	base = NULL;
}

void ClassSpace::setBase (ClassSpace *base)
{
	this->base = base;
}

void ClassSpace::addSuper (SuperDeclaration *decl)
{
	super.add (decl);
}

void ClassSpace::addExtension (MapDeclaration *decl)
{
	extension.add (decl);
}

/* base added */
PropertyDeclaration *ClassSpace::lookup (Identifier *identifier, ExpressionList *argList, Array<ComponentDeclaration> *path)
{
	int i;
	Declaration *ret, *tmp;

	for (i = 0; i < list.array.count; ++i)
		if (list.array.array[i]->match (identifier))
		{
			ret = list.array.array[i]->lookup (argList);
			if (ret)
				return (PropertyDeclaration *) ret;
		}

	ret = NULL;
	if (base)
		ret = base->lookup (identifier, argList, path);
	for (i = 0; i < super.count; ++i)
	{
		tmp = ((ClassType *) super.array[i]->type)->rm.ns.lookup (identifier, argList, path);
		if (tmp)
		{
			if (ret)
			{
				identifier->erAmbigious ();
//				identifier->error ("ambigious identifier: %s", identifier->value);
				return (PropertyDeclaration *) ret;
			}
			ret = tmp;
			path->addFront (super.array[i]);
		}
	}

	for (i = 0; i < extension.count; ++i)
	{
		tmp = ((ClassType *)extension.array[i]->type)->rm.ns.lookup (identifier, argList, path);
		if (tmp)
		{
			if (ret)
			{
				identifier->erAmbigious ();
//				identifier->error ("ambigious identifier: %s", identifier->value);
				return (PropertyDeclaration *) ret;
			}
			ret = tmp;
			path->addFront (extension.array[i]);
		}
	}
	return (PropertyDeclaration *) ret;
}

Polymorphic *ClassSpace::lookupMethod (Identifier *identifier)
{
	Symbol *s;
	PolymorphicSymbol *ms;

	s = list.lookup (identifier);
	if (s)
	{
		if (s->which == Symbol::SYM_POLYMORPHIC)
		{
			ms = (PolymorphicSymbol *)s;
			return &ms->poly;
		}
		else
			return NULL;
	}
	if (base)
		return base->lookupMethod (identifier);
	return NULL;
}

/*
ExecutionDeclaration *ClassSpace::lookupUniqueMethod (Identifier *identifier, Array<ComponentDeclaration> *path)
{
	Symbol *s;
	PolymorphicSymbol *ms;
	Polymorphic *ret, *tmp;
	int i;

	s = list.lookup (identifier);
	if (s)
	{
		if (s->which == SYM_POLYMORPHIC)
		{
			ms = (PolymorphicSymbol *)s;
			return ms->poly.lookupUniqueMethod ();
		}
		else
		{
			identifier->error ("%s does not refer to a method");
			return NULL;
		}
	}

	ret = NULL;
	for (i = 0; i < super.count; ++i)
	{
		tmp = ((ClassType *)super.array[i]->type)->ns.lookupMethodxx (identifier, path);
		if (tmp)
		{
			if (ret)
			{
				identifier->erAmbigious ();
				return ret->lookupUniqueMethod ();
			}
		}
		ret = tmp;
		path->addFront (super.array[i]);
	}
	for (i = 0; i < extension.count; ++i)
	{
		tmp = ((ClassType *)extension.array[i]->type)->ns.lookupMethodx (identifier, path);
		if (tmp)
		{
			if (ret)
			{
				identifier->erAmbigious ();
				return ret;
			}
		}
		ret = tmp;
		path->addFront (extension.array[i]);
	}
	return ret;
}
*/

/* base added */
Polymorphic *ClassSpace::lookupMethod (Identifier *identifier, Array<ComponentDeclaration> *path)
{
	Symbol *s;
	PolymorphicSymbol *ms;
	Polymorphic *ret, *tmp;
	int i;

	s = list.lookup (identifier);
	if (s)
	{
		if (s->which == Symbol::SYM_POLYMORPHIC)
		{
			ms = (PolymorphicSymbol *)s;
			return &ms->poly;
		}
		else
		{
			identifier->error ("%s does not refer to a method");
			return NULL;
		}
	}

	ret = NULL;
	if (base)
		ret = base->lookupMethod (identifier, path);
	for (i = 0; i < super.count; ++i)
	{
		tmp = ((ClassType *)super.array[i]->type)->rm.ns.lookupMethod (identifier, path);
		if (tmp)
		{
			if (ret)
			{
				identifier->erAmbigious ();
				return ret;
			}
		}
		ret = tmp;
		path->addFront (super.array[i]);
	}
	for (i = 0; i < extension.count; ++i)
	{
		tmp = ((ClassType *)extension.array[i]->type)->rm.ns.lookupMethod (identifier, path);
		if (tmp)
		{
			if (ret)
			{
				identifier->erAmbigious ();
				return ret;
			}
		}
		ret = tmp;
		path->addFront (extension.array[i]);
	}
	return ret;
}

/* base added */
MethodDeclaration *ClassSpace::lookupNonExternMethod (MethodDeclaration *method, Array<ComponentDeclaration> *downPath)
{
	Symbol *s;
	PolymorphicSymbol *ms;
	int i;
	MethodDeclaration *ret, *tmp;

	s = list.lookup (method->identifier);
	if (s)
	{
		if (s->which == Symbol::SYM_POLYMORPHIC)
		{
			ms = (PolymorphicSymbol *)s;
			ret = (MethodDeclaration *) ms->poly.lookup ((ExecutionType *) method->type);
			if (ret->modifier->has (MOD_REQUIRES))
			{
//				method->identifier->error ("%s refers to an extern method", method->identifier->value);
				return NULL;
			}
			return ret;
		}
		else
		{
			method->identifier->error ("%s does not refer to a method", method->identifier->value);
			return NULL;
		}
	}

	ret = NULL;
	if (base)
		ret = base->lookupNonExternMethod (method, downPath);
	for (i = 0; i < super.count; ++i)
	{
		tmp = ((ClassType *)super.array[i]->type)->rm.ns.lookupNonExternMethod (method, downPath);
		if (tmp)
		{
			if (ret)
			{
				method->identifier->erAmbigious ();
				return ret;
			}
			ret = tmp;
			downPath->add (super.array[i]);
		}
	}
	for (i = 0; i < extension.count; ++i)
	{
		tmp = ((ClassType *)extension.array[i]->type)->rm.ns.lookupNonExternMethod (method, downPath);
		if (tmp)
		{
			if (ret)
			{
				method->identifier->erAmbigious ();
				return ret;
			}
			ret = tmp;
			downPath->add (extension.array[i]);
		}
	}
	return ret;
}

/*
MethodDeclaration *ClassSpace::lookupMethodx (MethodDeclaration *method, Array<ComponentDeclaration> *path)
{
	Polymorphic *poly;

	poly = lookupMethodxx (method->identifier, path);
	if (!poly)
		return NULL;
	return (MethodDeclaration *) poly->lookup ((ExecutionType *) method->type);
}
*/

/* base added */
MethodDeclaration *ClassSpace::lookupMethod (MethodDeclaration *method)
{
	Polymorphic *poly;

	poly = lookupMethod (method->identifier);
	if (poly)
		return (MethodDeclaration *) poly->lookup ((ExecutionType *) method->type);
	return NULL;
}

// ComponentSpace

TypeSymbol *ComponentSpace::addType (NamedType *type)
{
	TypeSymbol *ts;

	ts = new TypeSymbol (type);
	if (list.add (ts))
		return ts;
	delete ts;
	return NULL;
}

void ComponentSpace::addExtension (InstanceDeclaration *decl)
{
	extension.add (decl);
}

Type *ComponentSpace::lookupType (Identifier *identifier)
{
	Symbol *sym;

	sym = list.lookup (identifier);
	if (!sym)
		return NULL;
	if (sym->which != Symbol::SYM_TYPE)
		return NULL;
	return ((TypeSymbol *)sym)->type;
}

Declaration *ComponentSpace::lookup (Identifier *identifier, ExpressionList *argList)
{
	int i;
	Declaration *decl;
	bool found;

	for (i = 0; i < list.array.count; ++i)
		if (list.array.array[i]->match (identifier))
		{
			decl = list.array.array[i]->lookup (argList);
			if (decl)
				return decl;
		}

	decl = NULL;
	found = false;
	for (i = 0; i < extension.count; ++i)
	{
		decl = ((ComponentType *)extension.array[i]->type)->rm.lookup (identifier, argList);
		if (decl)
		{
			if (found)
			{
				identifier->error ("ambigious identifier: %s", identifier->value);
				return decl;
			}
			found = true;
		}
	}
	return decl;
}

