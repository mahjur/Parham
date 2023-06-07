
#include "parham.hxx"

Type::Type (WhichType _which, ComponentType *_component, int _line)
	: NT (_component, _line)
{
	INIT (which);
}

ArrayType::ArrayType (Type *_base, IntegerExpression *_index)
	: Type (TYPE_ARRAY, _base->component, _base->line)
{
	INIT (base);
	INIT (index);
}

NamedType::NamedType (WhichType _which, Identifier *_identifier)
	: Type (_which, _identifier->component, _identifier->line)
{
	INIT (identifier);
}

VoidType::VoidType (ComponentType *_component, int _line)
	: NamedType (TYPE_VOID, new Identifier ("void", _component, _line))
{
}

IntegerType::IntegerType (const char *_name, ComponentType *_component, int _line)
	: NamedType  (TYPE_INTEGER, new Identifier (_name, _component, _line))
{
}

FloatType::FloatType (const char *_name, ComponentType *_component, int _line)
	: NamedType  (TYPE_FLOAT, new Identifier (_name, _component, _line))
{
}

SetDelegate::SetDelegate (InstanceDeclaration *_instance, Identifier *_left, Identifier *_right)
{
	INIT (instance);
	INIT (left);
	INIT (right);
}

ClassType::ClassType (bool _isInternal, Identifier *_identifier)
	: ContainerType (TYPE_CLASS, _identifier, &rm.ns), rm (this), cm (this), /*mapper (this),*/ vtable (this, _identifier->component, _identifier->line)
{
	INIT (isInternal);
	modifier = new Modifier (_identifier->component, _identifier->line);
	pname = NULL;
	unificationList = NULL;
	indexName = 0;
	isState = false;
	isAbstract = false;
	mainSet = false;

	setPName ();
}

ClassType::ClassType (bool _isInternal, Modifier *_modifier, Identifier *_identifier)
	: ContainerType (TYPE_CLASS, _identifier, &rm.ns), rm (this), cm (this), /*mapper (this),*/ vtable (this, _identifier->component, _identifier->line)
{
	INIT (isInternal);
	INIT (modifier);
	pname = NULL;
	unificationList = NULL;
	indexName = 0;
	isState = false;
	isAbstract = false;
	mainSet = false;
}

Identifier *ClassType::makeName (ComponentType *component, int line)
{
	char buffer[1024];

	sprintf (buffer, "parham%d", indexName++);
	return new Identifier (strdup (buffer), component, line);
}

void ClassType::addUnification (UnificationList *_unificationList)
{
	if (unificationList)
		unificationList->error ("duplicate unification list for %s", identifier->value);
	else
		INIT (unificationList);
}

InternalType::InternalType (Identifier *_identifier)
	: ClassType (true, _identifier)
{
	vt = NULL;
	base = NULL;
}

InternalType::InternalType (Modifier *_modifier, Identifier *_identifier)
	: ClassType (true, _modifier, _identifier)
{
	vt = NULL;
	base = NULL;
}

void InternalType::setBase (InternalType *base)
{
	this->base = base;
	rm.ns.setBase (&base->rm.ns);
}

InterfaceType::InterfaceType (Identifier *_identifier, UniqueArray<Identifier> *_stateList)
	: ClassType (false, _identifier)
{
	int i;

	INIT (stateList);
	for (i = 0; i < stateList->array.count; ++i)
		component->addInternal (new InternalType (stateList->array.array[i]));
}

InterfaceType::InterfaceType (Modifier *_modifier, Identifier *_identifier, UniqueArray<Identifier> *_stateList)
	: ClassType (false, _modifier, _identifier)
{
	int i;

	INIT (stateList);
	for (i = 0; i < stateList->array.count; ++i)
		component->addInternal (new InternalType (stateList->array.array[i]));
}

TransitionType::TransitionType (ClassType *_from, ClassType *_to)
	: Type (TYPE_TRANSITION, _from->component, _from->line)
{
	INIT (from);
	INIT (to);
}

TypedefDeclaration::TypedefDeclaration (Identifier *_identifier, Type *_type)
	: NamedType (TYPE_TYPEDEF, _identifier)
{
	INIT (type);
}

EnumerationItem::EnumerationItem (Identifier *_identifier, IntegerExpression *_literal)
	: Declaration (DECL_ENUMERATION, false, new Modifier (_identifier->component, _identifier->line), NULL, false, _identifier, _identifier->component, _identifier->line)
{
	INIT (literal);
}

EnumerationType::EnumerationType (Identifier *_identifier, UniqueArray<EnumerationItem> *_list, ComponentType *_component, int _line)
	: NamedType (TYPE_ENUMERATION, _identifier)
{
	int i;
	unsigned value;
	char buffer[64];

	INIT (list);

	value = 0;
	for (i = 0; i < list->array.count; ++i)
	{
		if (list->array.array[i]->literal)
			value = atoi (list->array.array[i]->literal->value) + 1;
		else
		{
			sprintf (buffer, "%u", value++);
			list->array.array[i]->literal = new IntegerExpression (strdup (buffer), list->array.array[i]->component, list->array.array[i]->line);
		}
	}
}

ExecutionType::ExecutionType (Type *_retType, ArgumentDeclarationList *_argList)
	: Type (TYPE_EXECUTION, _retType->component, _retType->line)
{
	INIT (retType);
	INIT (argList);
}

UnificationList::UnificationList (UniqueArray<ClassType> *_list, ComponentType *_component, int _line)
	: NT (_component, _line)
{
	INIT (list);
}

/*
bool ClassType::matchx (ClassType *set)
{
	return mapper.target->identifier->match (set->mapper.target->identifier);
}

bool ClassType::matchx (Identifier *identifier2)
{
	return mapper.target->identifier->match (identifier2);
}
*/

bool EnumerationItem::match (EnumerationItem *item)
{
	return identifier->match (item->identifier);
}

void ClassType::addAttribute (PropertyDeclaration *prop)
{
	if (!prop->identifier)
	{
		prop->setIdentifier (makeName (component, line));
		switch (prop->which)
		{
			case Declaration::DECL_SUPER:
				rm.ns.addSuper ((SuperDeclaration *) prop);
				break;
			default:
				rm.ns.addExtension ((MapDeclaration *) prop);
		}
	}
	else if (!rm.ns.addSimple (prop))
	{
		prop->error ("duplicate identifier: %s", prop->identifier->value);
		return;
	}
	attributeList.add (prop);
	propertyList.add (prop);
}

void ClassType::publishHeader (FILE *f)
{
	int i;

	if (modifier->has (MOD_EXTERN))
		return;

	// set class
	if (!mainSet)
	{
		fprintf (f, "struct %s\n", pname);
		fprintf (f, "{\n");
	}
	for (i = 0; i < attributeList.count; ++i)
		attributeList.array[i]->publishAttribute (f, true);
	vtable.publishEntry (f);
	if (!mainSet)
		fprintf (f, "};\n");
	fprintf (f, "\n");

	// virtual table
	vtable.publishClass (f);

	cm.publishHeader (f);
	/*
	for (i = 0; i < constructorList.list.count; ++i)
		constructorList.list.array[i]->publishHeader (f);
	*/
	for (i = 0; i < methodList.count; ++i)
		methodList.array[i]->publishHeader (f);
}

void ClassType::publishName (FILE *f)
{
	if (modifier->has (MOD_EXTERN))
		return;

	fprintf (f, "struct %s;\n", pname);
	vtable.publishPrototype (f);
}

void ClassType::publishSource (FILE *f)
{
	int i;

	if (modifier->has (MOD_EXTERN))
		return;

	if (mainSet)
		for (i = 0; i < attributeList.count; ++i)
			attributeList.array[i]->publishAttribute (f, false);

	for (i = 0; i < methodList.count; ++i)
		methodList.array[i]->publishSource (f);
	cm.publishSource (f);
	/*
	for (i = 0; i < constructorList.list.count; ++i)
		constructorList.list.array[i]->publishSource (f);
		*/
}

void ClassType::publishVT (FILE *f)
{
}

void InternalType::publishVT (FILE *f)
{
	if (!isAbstract && vt)
		vt->publish (f);
}

void ClassType::setPName ()
{
	char buffer[1024];

	if (modifier->has (MOD_EXTERN))
		pname = identifier->value;
	else
	{
		sprintf (buffer, "%s_%s", component->pname, identifier->value);
		pname = strdup (buffer);
	}
}

void Type::publishPre (bool withPost, bool asRef, FILE *f)
{
}

void NamedType::publishPre (bool withPost, bool asRef, FILE *f)
{
	fprintf (f, "%s", identifier->value);
}

void ClassType::publishPre (bool withPost, bool asRef, FILE *f)
{
	if (modifier->has (MOD_EXTERN))
		fprintf (f, "%s", pname);
	else
		fprintf (f, "struct %s", pname);
	if (asRef)
		fprintf (f, " *");
}

void EnumerationType::publishPre (bool withPost, bool asRef, FILE *f)
{
	fprintf (f, "unsigned");
}

void Type::analysis ()
{
}

void Type::publishPost (FILE *f)
{
}

void ArrayType::publishPost (FILE *f)
{
	if (index)
	{
		fprintf (f, "[");
		index->publish (f);
		fprintf (f, "]");
	}
	base->publishPost (f);
	/*
	if (dimension)
		dimension->publishPost (f);
	*/
}

/*
void Dimension::publishPost (FILE *f)
{
	int i;

	for (i = 0; i < list.count; ++i)
		if (list.array[i])
		{
			fprintf (f, "[");
			list.array[i]->publish (f);
			fprintf (f, "]");
		}
}
*/
void ArrayType::publishPre (bool withPost, bool asRef, FILE *f)
{
	base->publishPre (withPost, asRef, f);
	fprintf (f, " ");
	if (!withPost)
		fprintf (f, "*");
	else if (!index)
		fprintf (f, "*");
//	dimension->publishPre (withPost, f);
}

/*
void Dimension::publishPre (bool withPost, FILE *f)
{
	int i;

	for (i = 0; i < list.count; ++i)
		if (!withPost)
			fprintf (f, "*");
		else if (!list.array[i])
			fprintf (f, "*");
}
*/

void ClassType::addSuper (InclusionDeclaration *incDecl)
{
	SuperDeclaration *sd;

	sd = new SuperDeclaration (this, incDecl);
	superList.add (sd);
	addAttribute (sd);
}

void ClassType::addSub (InclusionDeclaration *incDecl)
{
	subList.add (incDecl);
}

NullType::NullType (ComponentType *_component, int _line)
	: Type (TYPE_NULL, _component, _line)
{
}

/*
Mapper::Mapper (ClassType *_target)
{
	INIT (target);
}
*/

void ClassType::map (MapDeclaration *map)
{
	rm.mapper.init (map);
}

ContainerType::ContainerType (WhichType _which, Identifier *_identifier, ContainerSpace *_nsp)
	: NamedType (_which, _identifier)
{
	INIT (nsp);
}

// bool Type::accept (Type *type, Cast *castPath) variants

bool Type::accept (Type *type, Cast *castPath)
{
	return which == type->which;
}

bool EnumerationType::accept (Type *type, Cast *castPath)
{
	EnumerationType *et;

	if (which != type->which)
		return false;
	et = (EnumerationType *) type;
	return identifier->match (et->identifier);
}

bool ExecutionType::accept (Type *type, Cast *castPath)
{
	error ("comparing methods is illegal");
	return false;
}

bool ArrayType::accept (Type *type, Cast *castPath)
{
	ArrayType *at;

	if (index)
	{
		error ("Cannot assign to an array");
		return false;
	}
	if (type->which == TYPE_NULL)
		return true;

	if (type->which != which)
		return false;
	at = (ArrayType *) type;
	// FIXME: NULL is wrong
	if (base->accept (at->base, NULL))
		return true;
	return base->equal (at->base);
}

bool IntegerType::accept (Type *type, Cast *castPath)
{
	switch (type->which)
	{
		case TYPE_INTEGER:
		case TYPE_FLOAT:
		case TYPE_ENUMERATION:
			return true;
		default:
			return false;
	}
}

bool FloatType::accept (Type *type, Cast *castPath)
{
	switch (type->which)
	{
		case TYPE_INTEGER:
		case TYPE_FLOAT:
		case TYPE_ENUMERATION:
			return true;
		default:
			return false;
	}
}

bool ClassType::same (ClassType *set)
{
	if (!identifier->match (set->identifier))
		return false;
	return component->same (set->component);
}

//bool ClassType::accept2 (ClassType *type, Cast *castPath)
bool ClassType::accept2 (ClassType *type, Array<PropertyDeclaration> *path)
{
	bool found;
	int i;

	if (same (type))
		return true;
	found = false;
	for (i = 0; i < type->superList.count; ++i)
		if (accept2 ((ClassType *) type->superList.array[i]->type, path))
		{
			if (found)
			{
				error ("ambigious call");
				return true;
			}
			found = true;
			path->add (type->superList.array[i]);
			// FIXME: this is a bug I think
//			castPath->follow (type->superList.array[i]);
		}
	for (i = 0; i < type->mapAttributes.count; ++i)
		if (accept2 ((ClassType *) type->mapAttributes.array[i]->type, path))
		{
			if (found)
			{
				error ("ambigious call");
				return true;
			}
			found = true;
			path->add (type->superList.array[i]);
			// FIXME: this is a bug I think
//			castPath->follow (type->mapAttributes.array[i]);
		}
	return found;
}

bool ClassType::accept (Type *type, Cast *castPath)
{
	ClassType *sx;//, *m;
//	bool found;
	int i;
	 Array<PropertyDeclaration> path;

	// array case passes castPath=NULL and array of sets is not assignable
	if (!castPath)
		return false;

	switch (type->which)
	{
		case TYPE_CLASS:
			sx = (ClassType *) type;
			castPath->getup (&sx->rm.mapper);
			if (rm.mapper.target->accept2 (sx->rm.mapper.target, &path))
			{
				for (i = path.count; i-- > 0; )
					castPath->follow (path.array[i]);
				castPath->getdown (&rm.mapper);
				return true;
			}
			return false;
			/*
			m = sx->mapper.target;
			if (same (m))
			{
//				castPath->getdown (&sx->mapper);
				castPath->getup (&sx->mapper);
				return true;
			}
			found = false;
			for (i = 0; i < m->superList.count; ++i)
				if (accept (m->superList.array[i]->type, castPath))
				{
					if (found)
					{
						error ("ambigious call");
						return true;
					}
					found = true;
//					convertPath->add (CONVERT_DOWN, s->superList.array[i]);
					castPath->follow (m->superList.array[i]);
				}
			for (i = 0; i < m->mapAttributes.count; ++i)
				if (accept (m->mapAttributes.array[i]->type, castPath))
				{
					if (found)
					{
						error ("ambigious call");
						return true;
					}
					found = true;
					castPath->follow (m->mapAttributes.array[i]);
				}
			if (found)
				castPath->getdown (&sx->mapper);
			return found;
			*/
		case TYPE_NULL:
			return true;
		default:
			return false;
	}
}

bool TransitionType::accept (Type *type, Cast *castPath)
{
	if (type->which == TYPE_CLASS)
		return from->accept (type, castPath);
	return false;
}

bool TypedefDeclaration::accept (Type *type, Cast *castPath)
{
	if (which != type->which)
		return false;
	return identifier->match (((TypedefDeclaration *) type)->identifier);
}

// match (Type *type) variants

bool Type::match (Type *type)
{
	return which == type->which;
}

bool ClassType::match (Type *type)
{
	ClassType *stype;

//	if (this == type)
//		return true;
	if (component == type->component)
		return this == type;
	if (type->which != TYPE_CLASS)
		return false;
	stype = (ClassType *)  type;
	return rm.mapper.target->match (stype->rm.mapper.target);
}

bool ArrayType::match (Type *type)
{
	if (type->which != TYPE_ARRAY)
		return false;
	return base->match (((ArrayType *) type)->base);
}

bool TransitionType::match (Type *type)
{
	if (type->which != TYPE_CLASS)
		return false;
	return from->match (type);
}

bool TypedefDeclaration::match (Type *t)
{
	if (t->which != which)
		return false;
	return type->match (((TypedefDeclaration *) t)->type);
}

bool EnumerationType::match (Type *t)
{
	if (t->which != which)
		return false;
	return identifier->match (((EnumerationType *) t)->identifier);
}

bool ExecutionType::match (Type *type)
{
	if (type->which != which)
		return false;
	return argList->match (((ExecutionType *)type)->argList);
}

void ClassType::markVirtual ()
{
	int i;

	for (i = 0; i < methodList.count; ++i)
		markVirtual (methodList.array[i]);
}

void ClassType::markVirtual (MethodDeclaration *m)
{
	int i;

	for (i = 0; i < superList.count; ++i)
		superList.array[i]->inclusion->left->markVirtual2 (m);
}

void ClassType::markVirtual2 (MethodDeclaration *m)
{
	int i;

	for (i = 0; i < methodList.count; ++i)
		if (methodList.array[i]->identifier->match (m->identifier))
			if (methodList.array[i]->type->match (m->type))
			{
				methodList.array[i]->modifier->add (MOD_VIRTUAL);
				addVirtual (methodList.array[i]);
			}
	markVirtual (m);
}

void ExecutionType::addFront (ArgumentDeclaration *arg)
{
	argList->addFront (arg);
}

bool ClassType::satisfied (Array<ClassType> *list)
{
	int i;

	for (i = 0; i < superList.count; ++i)
		if (!superList.array[i]->hasLeft (list))
			return false;
	for (i = 0; i < attributeList.count; ++i)
		if (!attributeList.array[i]->satisfied (component, list))
			return false;
	/*
	{
		if (attributeList.array[i]->type->which != TYPE_CLASS
		for (j = 0; j < list->count; ++j)
			if (list->array[j]->identifier->match (attribute)
				goto found;
		return false;
found:
	}
	*/
	return true;
}

void ClassType::analysis ()
{
	int i;

	for (i = 0; i < propertyList.count; ++i)
		propertyList.array[i]->analysis ();
}

void InternalType::analysis ()
{
	Array<ComponentDeclaration> resolvePath;

	ClassType::analysis ();
	if (!isAbstract)
		vt = vtable.fill (pname, &resolvePath, this);
}

void ClassType::formSet ()
{
	ConstructorDeclaration *cd;
	int i;

	if (cm.constructorList.list.count == 0)
	{
		cd = new ConstructorDeclaration (this, new ArgumentDeclarationList (component, line));
		cd->setBody (new BlockStatement (new Array<Statement>, component, line));
//		cd->setContainer (this);
		cd->formSet ();
	}
	for (i = 0; i < attributeList.count; ++i)
		attributeList.array[i]->removeDuplicate ();
}

bool ClassType::canInstantiate ()
{
	return false;
}

bool InternalType::canInstantiate ()
{
	return !isAbstract;
}

int ContainerType::addMethod (MethodDeclaration *method)
{
	int ret;

	if (!(ret = nsp->addPolymorphic (method)))
	{
		method->error ("duplicate method: %s", method->identifier->value);
		return 0;
	}
	propertyList.add (method);
	if (method->modifier->has (MOD_REQUIRES | MOD_VIRTUAL))
		addVirtual (method);
	return ret;
}

int ClassType::addMethod (MethodDeclaration *method)
{
	int ret;

	ret = ContainerType::addMethod (method);
	if (!ret)
		return ret;
	methodList.add (method);
	return ret;
}

int ClassType::addConstructor (ConstructorDeclaration *constructor)
{
	propertyList.add (constructor);
	return cm.add (constructor);
}

bool ExecutionType::accept (ExpressionList *argList2)
{
	return argList->accept (argList2);
}

bool Type::accept (Expression *exp)
{
	return accept (exp->type, exp->rm.castPathx);
}

// FIXME: move to resolve manager
void ClassType::up (Cast *castPath)
{
	int i;

	for (i = 0; i < rm.mapper.pathx.count; ++i)
		castPath->castUp (rm.mapper.pathx.array[i]);
}

// FIXME: move to resolve manager
void ClassType::down (Cast *castPath)
{
	int i;

	for (i = rm.mapper.pathx.count; i-- > 0; )
		castPath->follow (rm.mapper.pathx.array[i]);
}

void ClassType::addMap (MapDeclaration *mapDecl)
{
	mapAttributes.add (mapDecl);
	addAttribute (mapDecl);
}

void ClassType::addVirtual (MethodDeclaration *method)
{
	vtable.add (method);
}

VTableType::VTableType (ClassType *_set, ComponentType *_component, int _line)
	: NT (_component, _line)
{
	INIT (set);
}

void VTableType::add (MethodDeclaration *method)
{
	int i;

	for (i = 0; i < methodList.count; ++i)
		if (methodList.array[i]->identifier->match (method->identifier))
			if (methodList.array[i]->argList->match (method->argList))
				return;
	methodList.add (method);
}

Statement *VTableType::generateAssignment ()
{
	char buffer[1024];

	if (hasEntry ())
	{
		sprintf (buffer, "%s->parhamvt = parhamvt", THIS_PTR);
		return new ExpressionStatement (new StringExpression (strdup (buffer), component, line));
	}
	return NULL;
}

void VTableType::publishEntry (FILE *f)
{
	publishEntry (f, "*parhamvt");
}

void VTableType::publishEntry (FILE *f, const char *name)
{
	if (hasEntry ())
		fprintf (f, "\tstruct %s_vt %s;\n", set->pname, name);
}

void VTableType::publishPrototype (FILE *f)
{
	if (hasEntry ())
		fprintf (f, "struct %s_vt;\n", set->pname);
}

void VTableType::publishClass (FILE *f)
{
	ClassType *set2;
	int i;

	if (hasEntry ())
	{
		fprintf (f, "struct %s_vt\n", set->pname);
		fprintf (f, "{\n");
		for (i = 0; i < set->rm.ns.super.count; ++i)
		{
			set2 = (ClassType *) set->rm.ns.super.array[i]->type;
			set2->vtable.publishEntry (f, set->rm.ns.super.array[i]->identifier->value);
		}
		for (i = 0; i < set->rm.ns.extension.count; ++i)
		{
			set2 = (ClassType *) set->rm.ns.extension.array[i]->type;
			set2->vtable.publishEntry (f, set->rm.ns.extension.array[i]->identifier->value);
		}
		for (i = 0; i < methodList.count; ++i)
			methodList.array[i]->publishVT (f);
		fprintf (f, "};\n");
		fprintf (f, "\n");
	}
}

void VTableType::publishArgument (FILE *f)
{
	fprintf (f, "struct %s_vt *parhamvt", set->pname);
}

bool VTableType::hasEntry ()
{
	int i;

	if (methodList.count > 0)
		return true;
	for (i = 0; i < set->rm.ns.super.count; ++i)
		if (((ClassType *) set->rm.ns.super.array[i]->type)->vtable.hasEntry ())
			return true;
	for (i = 0; i < set->rm.ns.extension.count; ++i)
		if (((ClassType *) set->rm.ns.extension.array[i]->type)->vtable.hasEntry ())
			return true;
	return false;
}

Expression *VTableType::makeCallArgument (PropertyDeclaration *pd)
{
	char buffer[1024];

	if (!hasEntry ())
		return NULL;
	if (pd)
	{
		switch (pd->which)
		{
			case  Declaration::DECL_ATTRIBUTE:
				sprintf (buffer, "&%s_vt", set->pname);				// passing the whole VT of a set
				break;
			default:
				sprintf (buffer, "&parhamvt->%s", pd->identifier->value);	// passing VT of a superset or mapset
		}
	}
	else
		sprintf (buffer, "&%s_vt", set->pname);				// passing the whole VT of a set
	return new StringExpression (strdup (buffer), component, line);
}

VTValue::VTValue (const char *_entry)
{
	INIT (entry);
}

VTDeclaration::VTDeclaration (const char *_entry, Declaration *_value)
	: VTValue (_entry)
{
	INIT (value);
}

VTable::VTable (const char *_entry)
	: VTValue (_entry)
{
}

/*
VTable *VTableType::fillVirtual (const char *name, Array<MapDeclaration> *mapPath, ClassType *owner)
{
	VTable *ret;
	int i;

	if (!hasEntry ())
		return NULL;
	ret = new VTable (name);

	for (i = 0; i < methodList.count; ++i)
	{
		method = methodList.array[i]->resolveVT (mapPath, &upPath, &downPath);
		if (method)
		{
		}
		else
			methodList.array[i]->error ("Class %s has no implementation for virtual/requires method: %s", owner->pname, methodList.array[i]->pname);
	}
}

VTable *VTableType::fillInstance (const char *name, Array<MapDeclaration> *mapPath, ClassType *owner)
{
	VTable *ret;
	int i;
	MapDeclaration *map;

	if (!hasEntry ())
		return NULL;
	ret = new VTable (name);
	fillVirtual (name, mapPath, owner);
	for (i = 0; i < set->mapAttributes.count; ++i)
	{
		map = set->mapAttributes.array[i];
		tmp = (ClassType *) map->type;
		mapPath->add (map);
		item = tmp->vtable.fillInstance (map->pname, path, owner);
		path->count--;
		if (item)
			ret->list.add (item);
	}
	return ret;
}
*/

VTable *VTableType::fill (const char *name, Array<ComponentDeclaration> *path, ClassType *owner)
{
	VTable *ret, *item;
	int i;
	MethodDeclaration *method;
	ClassType *tmp;
	ComponentDeclaration *cd;
	Array<ComponentDeclaration> upPath, downPath;

	if (!hasEntry ())
		return NULL;

	ret = new VTable (name);

	for (i = 0; i < methodList.count; ++i)
	{
		upPath.count = 0;
		downPath.count = 0;
		method = methodList.array[i]->resolveVT (path, &upPath, &downPath);
		if (method)
		{
			if (method->container != set)
				method = method->castFrom (methodList.array[i], &upPath, &downPath, owner);
			ret->list.add (new VTDeclaration (methodList.array[i]->pname, method));
		}
		else
			methodList.array[i]->error ("%s: there is no implementation for the virtual/requires method in class '%s'", methodList.array[i]->pname, owner->identifier->value);//identifier->value);
	}
	for (i = 0; i < set->mapAttributes.count; ++i)
	{
		cd = set->mapAttributes.array[i];
		tmp = (ClassType *) cd->type;
		path->add (cd);
		item = tmp->vtable.fill (cd->pname, path, owner);
		path->count--;
		if (item)
			ret->list.add (item);
	}
	for (i = 0; i < set->superList.count; ++i)
	{
		cd = set->superList.array[i];
		tmp = (ClassType *) cd->type;
		path->add (cd);
		item = tmp->vtable.fill (cd->pname, path, owner);
		path->count--;
		if (item)
			ret->list.add (item);
	}

//	path->count--;
	return ret;
}

void VTable::publish (FILE *f)
{
	int i;

	fprintf (f, "struct %s_vt %s_vt =\n", entry, entry);
	fprintf (f, "{\n");

	for (i = 0; i < list.count; ++i)
		list.array[i]->publish (f, 1);
	fprintf (f, "};\n");
	fprintf (f, "\n");
}

void VTDeclaration::publish (FILE *f, int indent)
{
	NT::putIndent (f, indent);
	fprintf (f, ".%s = %s,\n", entry, value->pname);
}

void VTable::publish (FILE *f, int indent)
{
	int i;

	NT::putIndent (f, indent);
	fprintf (f, ".%s =\n", entry);
	NT::putIndent (f, indent);
	fprintf (f, "{\n");

	for (i = 0; i < list.count; ++i)
		list.array[i]->publish (f, indent + 1);

	NT::putIndent (f, indent);
	fprintf (f, "},\n");
}

Type *ArrayType::make (Type *base, Array<IntegerExpression> *dimension)
{
	Type *ret;
	int i;

	ret = base;
	for (i = dimension->count; i-- > 0; )
//	for (i = 0; i < dimension->count; ++i)
		ret = new ArrayType (ret, dimension->array[i]);
	return ret;
}

bool TransitionType::equal (Type *type)
{
	TransitionType *ttype;

	if (which != type->which)
		return false;
	ttype = (TransitionType *)type;
	if (!from->equal (ttype->from))
		return false;
	return to->equal (ttype->to);
}

bool ExecutionType::equal (Type *type)
{
	if (type->which != which)
		return false;
	return argList->match (((ExecutionType *)type)->argList);
}

bool NullType::equal (Type *type)
{
	return which == type->which;
}

bool ArrayType::equal (Type *type)
{
	ArrayType *atype;

	if (which != type->which)
		return false;
	atype = (ArrayType *)type;
	if (!base->equal (atype->base))
		return false;
	if (index == NULL)
		return atype->index == NULL;
	if (atype->index == NULL)
		return index == NULL;
	// FIXME: I don't check the equality of integer values
	return true;
}

bool NamedType::equal (Type *type)
{
	NamedType *ntype;

	if (which != type->which)
		return false;
	ntype = (NamedType *)type;
	return identifier->match (ntype->identifier);
}

void ClassType::removeProperty (PropertyDeclaration *prop)
{
	attributeList.remove (prop);
}

bool Type::attrSatisfied (Array<ClassType> *list)
{
	return true;
}

bool ArrayType::attrSatisfied (Array<ClassType> *list)
{
	return base->attrSatisfied (list);
}

bool InternalType::attrSatisfied (Array<ClassType> *list)
{
	int i;

	for (i = 0; i < list->count; ++i)
		if (list->array[i]->identifier->match (identifier))
			return true;
	return false;
}

void EnumerationItem::publish (FILE *f)
{
	literal->publish (f);
}

