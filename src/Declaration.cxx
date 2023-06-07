
#include "parham.hxx"

/***********
   Modifier
************/

Modifier::Modifier (ComponentType *_component, int _line)
	: NT (_component, _line)
{
	value = 0;
}

void Modifier::add (unsigned int mod)
{
	value |= mod;
}

void Modifier::add (Modifier *modifier)
{
	value |= modifier->value;
}

bool Modifier::has (int v)
{
	return value & v;
}

Designator::Designator (ComponentType *_component, int _line)
	: NT (_component, _line)
{
}

ExpressionDesignator::ExpressionDesignator (Expression *_expression)
	: Designator (_expression->component, _expression->line)
{
	INIT (expression);
}

IdentifierDesignator::IdentifierDesignator (Identifier *_identifier)
	: Designator (_identifier->component, _identifier->line)
{
	INIT (identifier);
}

Initializer::Initializer (ComponentType *_component, int _line)
	: NT (_component, _line)
{
}

void Initializer::setDesignation (Array<Designator> *_designator)
{
	INIT (designator);
}

ExpressionInitializer::ExpressionInitializer (Expression *_expression)
	: Initializer (_expression->component, _expression->line)
{
	INIT (expression);
}

ArrayInitializer::ArrayInitializer (Array<Initializer> *_list, ComponentType *_component, int _line)
	: Initializer (_component, _line)
{
	INIT (list);
}

InitializedIdentifier::InitializedIdentifier (Identifier *_identifier, Initializer *_initializer)
{
	INIT (identifier);
	INIT (initializer);
}

/*
   Declaration
   */

Declaration::Declaration (DeclarationType _which, bool _isProperty, Modifier *_modifier, Type *_type, bool _isRef, Identifier *_identifier, ComponentType *_component, int _line)
	: NT (_component, _line)
{
	INIT (which);
	INIT (isProperty);
	INIT (identifier);
	INIT (modifier);
	INIT (type);
//	INIT (initializer);
	INIT (isRef);
	pname = NULL;
	if (identifier)
		pname = identifier->value;
}

void Declaration::setModifier (Modifier *_modifier)
{
	INIT (modifier);
}

void Declaration::setIdentifier (Identifier *_identifier)
{
	INIT (identifier);
	if (identifier)
		pname = identifier->value;
}

bool Declaration::satisfied (ComponentType *component, Array<ClassType> *list)
{
	if (this->component != component)
		return true;
	if (isRef)
		return true;
	return type->attrSatisfied (list);
}

/*
ValueDeclaration::ValueDeclaration (Identifier *_identifier, LiteralExpression *_literal)
	: Declaration (DECL_VALUE, false, new Modifier (_identifier->component, _identifier->line), NULL, false, _identifier, _identifier->component, _identifier->line)
{
	INIT (identifier);
	INIT (literal);
}

void ValueDeclaration::formSet ()
{
	if (!component->ns.addSingle (this))
		error ("duplicate identifier: %s", identifier->value);
	literal->analysis (NULL, NULL, NULL);
	if (literal->type)
		type = literal->type;
}
*/

void Declaration::publish (FILE *f)
{
	type->publishPre (true, isRef, f);
	fprintf (f, " ");
	if (identifier)
		identifier->publish (f);
	type->publishPost (f);
	if (!isRef && isProperty && type->which == Type::TYPE_CLASS)
		fprintf (f, "[1]");
}

void Declaration::setType (Type *_type)
{
	INIT (type);
}

void Declaration::publishHeader (FILE *f)
{
}

void Declaration::publishSource (FILE *f)
{
}

PropertyDeclaration::PropertyDeclaration (DeclarationType _which, ClassType *_container, Modifier *_modifier, Type *_type, bool _isRef, Identifier *_identifier, ComponentType *_component, int _line)
	: Declaration (_which, true, _modifier, _type, _isRef, _identifier, _component, _line)
{
	INIT (container);
}

ComponentDeclaration::ComponentDeclaration (DeclarationType _which, ClassType *_container, Modifier *_modifier, Type *_type, bool _isRef, Identifier *_identifier, ComponentType *_component, int _line)
	: PropertyDeclaration (_which, _container, _modifier, _type, _isRef, _identifier, _component, _line)
{
}

ArgumentDeclaration::ArgumentDeclaration (Type *_type, Identifier *_identifier, ComponentType *_component, int _line)
	: Declaration (DECL_ARGUMENT, false, new Modifier (_component, _line), _type, true, _identifier, _component, _line)
{
}

ArgumentDeclarationList::ArgumentDeclarationList (ComponentType *_component, int _line)
	: NT (_component, _line)
{
}

void ArgumentDeclarationList::add (ArgumentDeclaration *arg)
{
	list.add (arg);
}

void ArgumentDeclarationList::addFront (ArgumentDeclaration *_self)
{
	list.addFront (_self);
}

AttributeDeclaration::AttributeDeclaration (Modifier *_modifier, Type *_type, InitializedIdentifier *ii, ComponentType *_component, int _line)
	: PropertyDeclaration (DECL_ATTRIBUTE, NULL, _modifier, _type, false, ii->identifier, _component, _line)
{
	initializer = ii->initializer;
}

ExecutionDeclaration::ExecutionDeclaration (DeclarationType _which, Modifier *_modifier, Type *_retType, Identifier *_identifier, ArgumentDeclarationList *_argList)
	: PropertyDeclaration (_which, NULL, _modifier, new ExecutionType (_retType, _argList), false, _identifier, _identifier->component, _identifier->line)//, ns (NULL)
{
	INIT (retType);
	INIT (argList);
	body = NULL;
	thisType = NULL;
}

void ExecutionDeclaration::setBody (BlockStatement *_body)
{
	INIT (body);
}

ConstructorDeclaration::ConstructorDeclaration (ClassType *_set, ArgumentDeclarationList *_argList)
	: ExecutionDeclaration (DECL_CONSTRUCTOR, new Modifier (_set->component, _set->line), new VoidType (_set->component, _set->line), _set->identifier, _argList)
{
	callList = NULL;
	setContainer (_set);
}

void ConstructorDeclaration::setCallList (Array<ConstructorCall> *_callList)
{
	INIT (callList);
}

MethodDeclaration::MethodDeclaration (Modifier *_modifier, Type *_retType, Identifier *_identifier, ArgumentDeclarationList *_argList)
	: ExecutionDeclaration (DECL_METHOD, _modifier, _retType, _identifier, _argList)
{
}

void AttributeDeclaration::formSet ()
{
	// FIXME: It seems that container is never null.
//	if (container)
		container->addAttribute (this);
//	else
//		component->addAttribute (this);
}

void ExecutionDeclaration::formSet ()
{
	char buffer[1024];
	int index;

//	argList->formSet (&ns);

	if (!container && modifier->has (MOD_REQUIRES))
		error ("requires methods can be only members of classes");
	index = addToContainer ();

	if (modifier->has (MOD_EXTERN))
		pname = identifier->value;
	else
	{
		if (container)
			sprintf (buffer, "%s_%s_%d", container->pname, identifier->value, index);
		else
			sprintf (buffer, "%s_%s_%d", component->pname, identifier->value, index);
		pname = strdup (buffer);
	}
}

int MethodDeclaration::addToContainer ()
{
	if (container)
		return container->addMethod (this);
	return component->addMethod (this);
}

int ConstructorDeclaration::addToContainer ()
{
	return container->addConstructor (this);
}

void ExecutionDeclaration::publishSignature (FILE *f)
{
	retType->publishPre (false, true, f);
	fprintf (f, " %s ", pname);
	argList->publish (f, thisType? thisType: container, which == DECL_CONSTRUCTOR);
}

void MethodDeclaration::publishVT (FILE *f)
{
	if (modifier->has (MOD_REQUIRES | MOD_VIRTUAL))
	{
		fprintf (f, "\t");
		retType->publishPre (false, true, f);
		fprintf (f, " (* %s) ", pname);
		argList->publish (f, thisType? thisType: container, false);
		fprintf (f, ";\n");
	}
}

void ExecutionDeclaration::publishSource (FILE *f)
{
	if (!body)
		return;

	publishSignature (f);
	fprintf (f, "\n");
	body->publish (0, 0, f);
}

void AssociationDeclaration::formSet ()
{
	container->addAttribute (this);
}

InstanceDeclaration::InstanceDeclaration (Modifier *_modifier, QualifiedIdentifier *_qid, Array<MapDeclaration> *_mapList, Array<IntegerExpression> *_dimension, ComponentType *_component, int _line)
	: Declaration (DECL_INSTANCE, false, _modifier, NULL, false, NULL, _component, _line)
{
	int i;

	INIT (qid);
	INIT (mapList);
	INIT (dimension);
	delegateList = NULL;
	for (i = 0; i < mapList->count; ++i)
		mapList->array[i]->setInstance (this);
}

//Delegate::Delegate (Identifier *_left, Identifier *_right)
Delegate::Delegate (Identifier *_set, Identifier *_left, Identifier *_right)
	: NT (_left->component, _left->line)
{
	INIT (set);
	INIT (left);
	INIT (right);
	instance = NULL;
	instanceSet = NULL;
}

Delegate::Delegate (Identifier *_set, Identifier *_left, Identifier *_instance, Identifier *_instanceSet, Identifier *_right)
//Delegate::Delegate (Identifier *_set, Identifier *_left, Identifier *_right, Identifier *_instance)
	: NT (_left->component, _left->line)
{
	INIT (set);
	INIT (left);
	INIT (right);
	INIT (instance);
	INIT (instanceSet);
}

bool Delegate::match (Delegate *delegate)
{
	return left->match (delegate->left);
}

void InstanceDeclaration::setDelegate (Array<Delegate> *_delegateList)
{
	INIT (delegateList);
}

ExplicitDeclaration::ExplicitDeclaration (ClassType *_container, Array<PropertyDeclaration> *_propertyList, ComponentType *_component, int _line)
	: Declaration (DECL_EXPLICIT, false, new Modifier (_component, _line), NULL, false, NULL, _component, _line)
{
	int i;

	INIT (propertyList);
	for (i = 0; i < propertyList->count; ++i)
		propertyList->array[i]->setContainer (_container);
}

MapDeclaration::MapDeclaration (ClassType *_setType, Array<ClassType> *_stateMap, ComponentType *_component, int _line)
	: ComponentDeclaration (DECL_MAP, _setType, NULL, NULL, false, NULL, _component, _line)
{
	INIT (stateMap);
	instance = NULL;
}

void ExplicitDeclaration::formSet ()
{
	int i;

	for (i = 0; i < propertyList->count; ++i)
		propertyList->array[i]->formSet (); 
}

AssociationDeclaration *AssociationDeclaration::clone ()
{
	return new AssociationDeclaration (container, modifier, type);
}

AssociationDeclaration::AssociationDeclaration (ClassType *_container, Modifier *_modifier, Type *_type)
	: PropertyDeclaration (DECL_ASSOCIATION, _container, _modifier, _type, true, NULL, _container->component, _container->line)
{
}

void InstanceDeclaration::formSet ()
{
	char buffer[1024];
	ComponentType *mi;
	int i;

	qid->flat (buffer, '.');
	mi = program.front (buffer);
	if (!mi)
	{
		error ("error in compling component %s", buffer);
		return;
	}
	setType (mi);
	if (!mi->accept (this))
	{
		error ("the component instance does not match the component declaration: %s", buffer);
		return;
	}
	for (i = 0; i < mapList->count; ++i)
		mapList->array[i]->formSet (mi->interface.array[i]);
	component->addInstance (this);

	/*
	if (delegateList)
		for (i = 0; i < delegateList->count; ++i)
			delegateList->array[i]->set->addDelegate (this, delegateList->array[i]);
			*/
}

void InstanceDeclaration::publishHeader (FILE *f)
{
	char buffer[1024];

	qid->flat (buffer, '.');
	fprintf (f, "#include \"%s.h\"\n", buffer);
}

Disambiguation::Disambiguation (Statement *_statement, ComponentType *_component, int _line)
	: NT (_component, _line)
{
	INIT (statement);
}

StateInitializer::StateInitializer (ClassType *_set, Expression *_expression)
	: NT (_set->component, _set->line)
{
	INIT (set);
	INIT (expression);
}

InclusionBodyItem::InclusionBodyItem (ComponentType *_component, int _line)
	: NT (_component, _line)
{
}

StateAttributeDeclaration::StateAttributeDeclaration (Type *_type, Identifier *_identifier, UniqueArray<StateInitializer> *_initializer)
	: InclusionBodyItem (_type->component, _type->line)
{
	INIT (type);
	INIT (identifier);
	INIT (initializer);
}

TransitionState::TransitionState (ClassType *_set, ComponentType *_component, int _line)
	: NT (_component, _line)
{
	INIT (set);
}

TransitionDeclaration::TransitionDeclaration (TransitionState *_state)
	: InclusionBodyItem (_state->component, _state->line)
{
	list.add (_state);
}

void TransitionDeclaration::add (TransitionState *_state)
{
	list.add (_state);
}

InclusionBody::InclusionBody (Array<InclusionBodyItem> *_body, Disambiguation *_disambiguation, ComponentType *_component, int _line)
	: NT (_component, _line)
{
	INIT (body);
	INIT (disambiguation);
}

InclusionDeclaration::InclusionDeclaration (ClassType *_left, UniqueArray<ClassType> *_inclusion, Array<InclusionBodyItem> *_body, Disambiguation *_disambiguation)
	: Declaration (DECL_INCLUSION, false, new Modifier (_left->component, _left->line), _left, false, NULL, _left->component, _left->line)
{
	INIT (left);
	INIT (inclusion);
	INIT (body);
	INIT (disambiguation);
	if (!inclusion->array.array[inclusion->array.count - 1])
		inclusion->array.count--;
	else
		left->isAbstract = true;
}

bool StateInitializer::match (StateInitializer *st)
{
	return set->match (st->set);
}

void InclusionDeclaration::formSet ()
{
	int i;

	/*
	   the analysis should reveal whether it is a state or superset.
	   If it is a superset, the following code is correct.
	   However, if it is a state, we should use a union here
	   */
	left->addSub (this);
	for (i = 0; i < inclusion->array.count; ++i)
		inclusion->array.array[i]->addSuper (this);
}

void MapDeclaration::formSet (ClassType *_type)
{
	setType (_type);
	container->addMap (this);
}

/*
void ValueDeclaration::publish (FILE *f)
{
}

void ValueDeclaration::publishSource (FILE *f)
{
	fprintf (f, "#define ");
	identifier->publish (f);
	fprintf (f, " ");
	literal->publish (f);
	fprintf (f, "\n");
}
*/
void ExecutionDeclaration::publishHeader (FILE *f)
{
	publishSignature (f);
	fprintf (f, ";\n");
}

void Declaration::publishAttribute (FILE *f, bool header)
{
	if (header)
	{
		if (isProperty)
		{
			PropertyDeclaration *pd;

			pd = (PropertyDeclaration *)this;
			if (pd->container && pd->container->mainSet)
			{
				fprintf (f, "extern ");
				publish (f);
				fprintf (f, ";\n");
				return;
			}
		}
		fprintf (f, "\t");
		publish (f);
		fprintf (f, ";\n");
		return;
	}
	if (isProperty)
	{
		PropertyDeclaration *pd;

		pd = (PropertyDeclaration *)this;
		if (pd->container && pd->container->mainSet)
		{
			publish (f);
			fprintf (f, ";\n");
		}
	}
}

void ArgumentDeclarationList::analysis (BlockSpace *ns)
{
	int i;

	for (i = 0; i < list.count; ++i)
		list.array[i]->analysis (ns);
}

void ArgumentDeclaration::analysis (BlockSpace *ns)
{
	if (!identifier)
		return;
	if (!ns->addSimple (this))
		identifier->error ("duplicate identifier: %s", identifier->value);
}

void ArgumentDeclarationList::publish (FILE *f, ClassType *container, bool needVT)
{
	int i;

	fprintf (f, "(");
	if (container && !container->mainSet)
	{
		ArgumentDeclaration ad (container, new Identifier (THIS_PTR, container->component, container->line), container->component, container->line);
		ad.publish (f);

		if (needVT && container->vtable.hasEntry ())
		{
			fprintf (f, ", ");
			container->vtable.publishArgument (f);
#if 0
			fprintf (f, "struct %s_vt *parhamvt", container->pname);
#endif
		}
	}

	for (i = 0; i < list.count; ++i)
	{
//		if (i || (container && !container->mainSet) || needVT)
		if (i || (container && !container->mainSet))
			fprintf (f, ", ");
		list.array[i]->publish (f);
	}
	fprintf (f, ")");
}

bool ArgumentDeclarationList::accept (ExpressionList *argList)
{
	int i;

#warning support of variable size arguments should be added
	if (list.count != argList->list.count)
		return false;
	for (i = 0; i < list.count; ++i)
		if (!list.array[i]->type->accept (argList->list.array[i]))
			return false;
	return true;
}

bool ArgumentDeclarationList::match (ArgumentDeclarationList *argList)
{
	int i;


	if (list.count != argList->list.count)
		return false;
	for (i = 0; i < list.count; ++i)
		if (!list.array[i]->type->match (argList->list.array[i]->type))
			return false;
	return true;
}

SuperDeclaration::SuperDeclaration (ClassType *_container, InclusionDeclaration *_inclusion)
	: ComponentDeclaration (DECL_SUPER, _container, new Modifier (_inclusion->component, _inclusion->line), _inclusion->left, false, NULL, _inclusion->component, _inclusion->line)
{
	INIT (inclusion);
}

void Declaration::publishReference (FILE *f)
{
	if (modifier->has (MOD_REQUIRES | MOD_VIRTUAL))
		fprintf (f, "parhamvt->%s", pname);
	else
		fprintf (f, "%s", pname);
}

/*
MethodDeclaration *MethodDeclaration::resolveVTx (Array<ComponentDeclaration> *path, Array<ComponentDeclaration> *upPath, Array<ComponentDeclaration> *downPath)
{
	bool passInstance;
	MethodDeclaration *ret;
	ClassType *set;

	passInstance = false;
	ret = false;
	for (i = path->count; i-- > 0; )
	{
		ret = path->array[i]->type->ns.lookupMethod (this);
		switch (path->array[i]->which)
		{
			case DECL_SUPER:
				break;
			case DECL_MAP:
				if (ret && !passInstance)
					return ret;
				break;
			default:
				break;
		}
		// When a cross occurs, there are two cases.
		//	if there is an implementation, there is no need to pass the cross.
		//	The exception is that there is a "requires" clause to mark the implementation as default.
	}
	// do the same on the container of the last one
}
*/

MethodDeclaration *MethodDeclaration::resolveVT (Array<ComponentDeclaration> *path, Array<ComponentDeclaration> *upPath, Array<ComponentDeclaration> *downPath)
{
	int i;
	MethodDeclaration *cur, *ret, *tmp;
	int retCount;

	upPath->count = 0;
	downPath->count = 0;
	ret = NULL;
	if (body)
		ret = this;
	cur = this;
	for (i = path->count; i-- > 0; )
	{
		upPath->addFront (path->array[i]);
		tmp = path->array[i]->resolveVT (cur, upPath, downPath);
		if (tmp)
		{
//			upPath->add (path->array[i]);
			cur = tmp;
			if (tmp->body)
			{
				ret = tmp;
				retCount = upPath->count;
			}
		}
	}
	while (upPath->count > retCount)
		upPath->removeFront ();
	return ret;
}

/*
MethodDeclaration *MethodDeclaration::resolveVT (Array<ComponentDeclaration> *path, int *point)
{
	int i;
	MethodDeclaration *cur, *ret, *tmp;

	ret = NULL;
	if (body)
		ret = this;
	cur = this;
	for (i = path->count; i-- > 0; )
	{
		tmp = path->array[i]->resolveVT (cur);
		if (tmp)
		{
			cur = tmp;
			if (tmp->body)
			{
				ret = tmp;
				*point = i;
			}
		}
	}
	return ret;
}
*/

/*
MethodDeclaration *MethodDeclaration::resolveVT (Array<ComponentDeclaration> *path, int *point)
{
	MethodDeclaration *method, *tmp;
	int i;
	ClassType *set;

	set = container;
	method = NULL;
	for (i = path->count; ; )
	{
		// MHJ. Here I should add the line to lookup the delegation list.
		tmp = set->ns.lookupMethod (this);
		if (tmp)
		{
			if (tmp->body)
			{
				method = tmp;
				*point = i;
			}
		}
		// do the job for set
		if (i-- <= 0)
			break;
		set = (ClassType *) path->array[i]->container;
	}
	return method;
}
*/

void PropertyDeclaration::setContainer (ClassType *_container)
{
	INIT (container);
}

void Declaration::formSet ()
{
}

void ExecutionDeclaration::setContainer (ClassType *_container)
{
	PropertyDeclaration::setContainer (_container);
}

void MethodDeclaration::analysis ()
{
	int i;
	ClassType *set;
	BlockSpace ns (NULL);

	if (modifier->has (MOD_REQUIRES))
	{
		for (i = 0; i < argList->list.count; ++i)
			if (argList->list.array[i]->type->which == Type::TYPE_CLASS)
			{
				set = (ClassType *) argList->list.array[i]->type;
				if (set->isInternal)
					error ("the type of an argument of a requries method cannot be an internal class (%s).", set->pname);
			}
	}
	if (body)
	{
		argList->analysis (&ns);
		body->analysis (container, &ns, retType);
		body->list->addFront (new ExpressionStatement (new StringExpression ("void * parhamptr", component, line)));
	}
}

void ConstructorDeclaration::analysis ()
{
	int i;
	Array<Statement> *sl;
	Statement *vtAssignment;
	BlockSpace ns (NULL);

	if (!body)
		return;

	argList->analysis (&ns);
//	container->initConstructorCounters ();
	container->cm.initCounters ();
	if (callList)
		for (i = 0; i < callList->count; ++i)
			callList->array[i]->analysis (container, &ns);
//	container->checkConstructorCounters (this);
	container->cm.checkCounters (this);

	body->analysis (container, &ns, new VoidType (component, line));

	// Here, I should set the VT entry
	vtAssignment = container->vtable.generateAssignment ();

	if (!(callList || vtAssignment))
		return;

	if (body->list->count > 0)
	{
		sl = new Array<Statement>;
		if (callList)
			for (i = 0; i < callList->count; ++i)
				sl->add (new ExpressionStatement (callList->array[i]->constructorCall));
		if (vtAssignment)
			sl->add (vtAssignment);
		sl->add (body);
		body = new BlockStatement (sl, component, line);
	}
	else
	{
		if (callList)
			for (i = 0; i < callList->count; ++i)
				if (callList->array[i]->constructorCall)
					body->list->add (new ExpressionStatement (callList->array[i]->constructorCall));
		if (vtAssignment)
			body->list->add (vtAssignment);
	}
	body->list->addFront (new ExpressionStatement (new StringExpression ("void * parhamptr", component, line)));
}

bool SuperDeclaration::hasLeft (Array<ClassType> *list)
{
	int i;

	for (i = 0; i < list->count; ++i)
		if (list->array[i]->same (inclusion->left))
			return true;
	return false;
}

void Declaration::analysis ()
{
//	if (initializer)
//		initializer->analysis ();
}

void Initializer::analysis ()
{
	UNIMPLEMENTED ();
}

void MapDeclaration::setInstance (InstanceDeclaration *_instance)
{
	INIT (instance);
	modifier->add (instance->modifier);
}

void Declaration::mapSet ()
{
}

void InstanceDeclaration::mapSet ()
{
	int i;

	for (i = 0; i < mapList->count; ++i)
		mapList->array[i]->map ();
	((ComponentType *) type)->mapSet ();
	if (delegateList)
		for (i = 0; i < delegateList->count; ++i)
			applyDelegate (delegateList->array[i]);
}

void InstanceDeclaration::applyDelegate (Delegate *delegate)
{
	MapDeclaration *map;

	map = findMapByType (delegate->set);
	if (!map)
	{
		error ("%s is not an interface class of the instantiated component", delegate->set->value);
		return;
	}
	map->addDelegate (delegate);
}

/*
void MapDeclaration::addDelegatex (Delegate *delegate)
{
	// left side should be extern method of the instantiating component
	// right side is a method of anything.
	//	it can be specified directly
	//	or via specifying another instance
}
*/

// FIXME: move to resolve manager
void MapDeclaration::addDelegate (Delegate *delegate)
{
	ClassType *set;
	Polymorphic *poly;
	MethodDeclaration *leftMethod, *rightMethod;
	InstanceDeclaration *instancePtr;
	MapDeclaration *map;
	Array<ComponentDeclaration> *rightPath;

	rightPath = new Array<ComponentDeclaration>;

	set = (ClassType *) type;
	poly = set->rm.ns.lookupMethod (delegate->left);
	if (!poly)
	{
		delegate->left->error ("There is no method with the given name: %s", delegate->left->value);
		return;
	}
	if (poly->list.count != 1)
	{
		delegate->left->error ("The method is ambigious: %s", delegate->left->value);
		return;
	}
	leftMethod = (MethodDeclaration *) poly->list.array[0];

	if (delegate->instance)
	{
		// check the component name space to find it.
		instancePtr = component->lookupInstance (delegate->instance);
		if (!instancePtr)
		{
			delegate->instance->error ("%s does not refer to any instance declaration", delegate->instance->value);
			return;
		}
		map = instancePtr->findMapByType (delegate->instanceSet);
		if (!map)
		{
			delegate->instanceSet->error ("%s: There is no interface class with this name", delegate->instanceSet->value);
			return;
		}
		set = (ClassType *) map->type;
		rightPath->add (map);
	}
	else
		set = (ClassType *) container;

	poly = set->rm.ns.lookupMethod (delegate->right, rightPath);
	if (!poly)
	{
		delegate->right->error ("There is no method with the given name: %s", delegate->right->value);
		return;
	}
	if (poly->list.count != 1)
	{
		delegate->right->error ("The method is ambigious: %s", delegate->right->value);
		return;
	}
	rightMethod = (MethodDeclaration *) poly->list.array[0];

	delegateList.add (new MapDelegate (leftMethod, rightMethod, rightPath));
}

void MapDeclaration::map ()
{
	((ClassType *) type)->map (this);
}

MapDeclaration *InstanceDeclaration::findMapByContainer (ClassType *s)
{
	int i;

	for (i = 0; i < mapList->count; ++i)
		if (mapList->array[i]->container == s)
			return mapList->array[i];
	return NULL;
}

MapDeclaration *InstanceDeclaration::findMapByType (ClassType *s)
{
	int i;

	for (i = 0; i < mapList->count; ++i)
		if (mapList->array[i]->type == s)
			return mapList->array[i];
	return NULL;
}

MapDeclaration *InstanceDeclaration::findMapByType (Identifier *id)
{
	int i;

	for (i = 0; i < mapList->count; ++i)
		if (((ClassType *) mapList->array[i]->type)->identifier->match (id))
			return mapList->array[i];
	return NULL;
}

void ConstructorDeclaration::addConstructorCall (ConstructorCall *cc)
{
	if (!callList)
		callList = new Array<ConstructorCall>;
	callList->add (cc);
}

// FIXME: move to resolve manager
MethodDeclaration *MethodDeclaration::castFrom (MethodDeclaration *sig, Array<ComponentDeclaration> *upPath, Array<ComponentDeclaration> *downPath, ClassType *owner)
{
	ArgumentDeclarationList *newArgList;
	int i, j;
	ExpressionList *elist;
	IdentifierExpression *id;
	static int index = 0;

	IdentifierExpression *method;
	char buffer[1024];
	MethodDeclaration *ret;
	Statement *statement;
	Array<Statement> *statementList;
	CallExpression *expression;
	ClassType *s;
	ComponentDeclaration *nextComponent;

#ifdef DEBUG
	fprintf (stderr, "%s.castFrom (%s)\n" pname, sig->pname);
#endif
	// 1. make the function prototype
	newArgList = new ArgumentDeclarationList (sig->component, sig->line);
	for (i = 0; i < sig->argList->list.count; ++i)
		newArgList->add (new ArgumentDeclaration (sig->argList->list.array[i]->type, argList->list.array[i]->identifier, sig->component, sig->line));

	// 2. make the function body (call expression)

	// 2.1. make the function call arguments
	elist = new ExpressionList (component, line);
	for (i = 0; i < sig->argList->list.count; ++i)
	{
		id = new IdentifierExpression (argList->list.array[i]->identifier);
		id->type = sig->argList->list.array[i]->type;
		elist->add (id);

		id->rm.castPathx->follow ((PropertyDeclaration *) newArgList->list.array[i]);
		if (id->type->which == Type::TYPE_CLASS)
		{
			s = (ClassType *) id->type;
//			for (j = 0; j < upPath->count; ++j)
			for (j = upPath->count; j--; )
			{
				if (upPath->array[j]->which == DECL_SUPER)
				{
					continue;
				}
				nextComponent = upPath->array[j]->findUp (s);
				if (nextComponent)
				{
					id->rm.castPathx->castUp (nextComponent);
					s = nextComponent->container;
				}
				else
					error ("mapping %s on %s: the set is not mapped %s in the instantiation", pname, sig->pname, s->identifier->value);
			}
			for (j = 0; j < downPath->count; ++j)
			{
				if (upPath->array[j]->which == DECL_SUPER)
				{
					continue;
				}
				nextComponent = downPath->array[j]->findDown (s);
				if (nextComponent)
				{
					id->rm.castPathx->follow (nextComponent);
					s = nextComponent->container;
				}
				else
					error ("mapping %s on %s: the set is not mapped %s in the instantiation", pname, sig->pname, s->identifier->value);
			}
		}
	}

	// 2.2. make the function name to call
	method = new IdentifierExpression (identifier);
	method->type = type;

	// 2.3. make this argument
	method->object = new ThisExpression (component, line);
//	for (i = 0; i < upPath->count; ++i)
	for (i = upPath->count; i-- > 0; )
		method->rm.castPathx->castUp (upPath->array[i]);
	for (i = 0; i < downPath->count; ++i)
		method->rm.castPathx->follow (downPath->array[i]);
	/*
	for (i = upPath->count; i-- > 0; )
		method->castPathx->castUp (upPath->array[i]);
	*/
	method->rm.castPathx->follow (this);

	// 2.4. make the call expression
	expression = new CallExpression (method, elist);

	// 2.5. make the statement
	if (sig->retType->which == Type::TYPE_VOID)
		statement = new ExpressionStatement (expression);
	else
		statement = new ReturnStatement (expression, component, line);
	statementList = new Array<Statement>;
	statementList->add (new ExpressionStatement (new StringExpression ("void * parhamptr", component, line)));
	statementList->add (statement);

	// 2.6. make the function
	ret = new MethodDeclaration (new Modifier (sig->component, sig->line), sig->retType, sig->identifier, newArgList);
	ret->setBody (new BlockStatement (statementList, sig->component, sig->line));
	ret->setContainer (owner);
	sprintf (buffer, "%s_%s__%d", owner->pname, ret->identifier->value, ++index);
	ret->pname = strdup (buffer);
	ret->thisType = sig->container;
	owner->methodList.add (ret); // for being published

	return ret;
}

#if 0
MethodDeclaration *MethodDeclaration::castTo (MethodDeclaration *sig, Array<ComponentDeclaration> *path, int point, ClassType *owner)
{
	MethodDeclaration *ret;
	Array<Statement> *statementList;
	static int index = 0;
	char buffer[1024];
	ArgumentDeclarationList *newArgList;
	CallExpression *expression;
	Statement *statement;
	ExpressionList *elist;
	int i;
	IdentifierExpression *id;
	IdentifierExpression *method;

#ifdef DEBUG
	fprintf (stderr, "%s.castTo (%s)\n", pname, sig->pname);
#endif

	newArgList = new ArgumentDeclarationList (sig->component, sig->line);
	for (i = 0; i < sig->argList->list.count; ++i)
		newArgList->add (new ArgumentDeclaration (sig->argList->list.array[i]->type, argList->list.array[i]->identifier, sig->component, sig->line));

#ifdef DEBUG
	fprintf (stderr, "casting %d arguments\n", sig->argList->list.count);
#endif
	elist = new ExpressionList (component, line);
	for (i = 0; i < sig->argList->list.count; ++i)
	{
		id = new IdentifierExpression (argList->list.array[i]->identifier);
		id->type = sig->argList->list.array[i]->type;
		elist->add (id);

//		id->convertPath.add (CONVERT_DOWN, (PropertyDeclaration *) argList->list.array[i]);
		id->castPathx->follow ((PropertyDeclaration *) argList->list.array[i]);
		if (id->type->which == TYPE_SET)
		{
			ClassType *s;
			int j;

			s = (ClassType *) id->type;
			if (!s->modifier->has (MOD_FOREIGN))
			{
//				for (j = point; j < path->count; ++j)
				for (j = path->count; j-- > point; )
				{
					ComponentDeclaration *tmp;

					tmp = path->array[j]->findUp (s);
					if (!tmp)
						error ("the set is not mapped %s in the instantiation", s->identifier->value);
					else
					{
//						if (tmp == path->array[j])
//							continue;
//						s = (ClassType *) tmp->type;
						s = tmp->container;
						id->castPathx->castUp (tmp);
					}
				}
			}
		}
	}

#ifdef DEBUG
	fprintf (stderr, "Casting method %s\n", identifier->value);
#endif
	method = new IdentifierExpression (identifier);
	method->type = type;
	method->object = new ThisExpression (component, line);
//	for (i = point; i < path->count; ++i)
	for (i = path->count; i-- > point; )
		method->castPathx->castUp (path->array[i]);
	method->castPathx->follow (this);
	expression = new CallExpression (method, elist);

	if (sig->retType->which == TYPE_VOID)
		statement = new ExpressionStatement (expression);
	else
		statement = new ReturnStatement (expression, component, line);
	statementList = new Array<Statement>;
	statementList->add (new ExpressionStatement (new StringExpression ("void * parhamptr", component, line)));
	statementList->add (statement);

	ret = new MethodDeclaration (new Modifier (sig->component, sig->line), sig->retType, sig->identifier, newArgList);
	ret->setBody (new BlockStatement (statementList, sig->component, sig->line));
	/*
	ret->setContainer (container);
	sprintf (buffer, "%s_%s__%d", container->pname, ret->identifier->value, ++index);
	ret->pname = strdup (buffer);
	ret->thisType = sig->container;
	container->methodList.add (ret); // for being published
	*/
	ret->setContainer (owner);
	sprintf (buffer, "%s_%s__%d", owner->pname, ret->identifier->value, ++index);
	ret->pname = strdup (buffer);
	ret->thisType = sig->container;
	owner->methodList.add (ret); // for being published

	return ret;
}
#endif

ComponentDeclaration *MapDeclaration::findUp (ClassType *s)
{
//	return instance->findMapByContainer (s);
	return instance->findMapByType (s);
}

ComponentDeclaration *MapDeclaration::findDown (ClassType *s)
{
	return instance->findMapByContainer (s);
}

ComponentDeclaration *SuperDeclaration::findUp (ClassType *s)
{
	return this;
}

ComponentDeclaration *SuperDeclaration::findDown (ClassType *s)
{
	return this;
}

void AttributeDeclaration::analysis ()
{
	if (initializer)
		initializer->analysis ();
}

#ifdef DEBUG
void Declaration::dbg_print ()
{
	fprintf (stderr, "Declaration (%s, %s)\n", pname?pname:"null", identifier?identifier->value:"null");
}
#endif

SingleInitializedIdentifier::SingleInitializedIdentifier (ClassType *_container, Identifier *_identifier, Initializer *_initializer)
	: InitializedIdentifier (_identifier, _initializer)
{
	INIT (container);
}

// FIXME: move to resolve manager
MethodDeclaration *SuperDeclaration::resolveVT (MethodDeclaration *method, Array<ComponentDeclaration> *upPath, Array<ComponentDeclaration> *downPath)
{
	return container->rm.ns.lookupMethod (method);
}

// FIXME: move to resolve manager
MethodDeclaration *MapDeclaration::resolveVT (MethodDeclaration *method, Array<ComponentDeclaration> *upPath, Array<ComponentDeclaration> *downPath)
{
	MethodDeclaration *right, *externMethod;
	int i;
	ClassType *set;

	set = (ClassType *)type;
	externMethod = set->rm.ns.lookupMethod (method);
	if (!externMethod)
		return NULL;
	if (!externMethod->modifier->has (MOD_REQUIRES))
		return NULL;

	// check the delegate list here
	for (i = 0; i < delegateList.count; ++i)
	{
		right = delegateList.array[i]->resolve (externMethod, downPath);
		if (right)
			return right;
	}
	return container->rm.ns.lookupNonExternMethod (method, downPath);
}

MethodDeclaration *MapDelegate::resolve (MethodDeclaration *md, Array<ComponentDeclaration> *downPath)
{
	if (!left->identifier->match (md->identifier))
		return NULL;

	if (left->type->equal (md->type))
	{
		downPath->append (rightPath);
		return right;
	}
	return NULL;
}

MapDelegate::MapDelegate (MethodDeclaration *_left, MethodDeclaration *_right, Array<ComponentDeclaration> *_rightPath)
//MapDelegate::MapDelegate (MethodDeclaration *_left, MethodDeclaration *_right)
{
	INIT (left);
	INIT (right);
	INIT (rightPath);
}

bool Declaration::matchIdentifier (Identifier *id)
{
	if (!identifier)
		return NULL;
	return identifier->match (id);
}

bool InstanceDeclaration::matchComponent (Identifier *identifier)
{
	return qid->match (identifier);
}

void PropertyDeclaration::removeDuplicate ()
{
}

void SuperDeclaration::removeDuplicate ()
{
	// here if there is an instance declaration containing the class and its super
	//	and if it cotains the same inclusion relation
	//	this superDeclaration is removed from the propertyList of this set.
	int i;

	for (i = 0; i < container->attributeList.count; ++i)
		if (container->attributeList.array[i]->containInclusion (this))
			container->removeProperty (this);
}

bool PropertyDeclaration::containInclusion (SuperDeclaration *)
{
	return false;
}

bool MapDeclaration::containInclusion (SuperDeclaration *super)
{
	MapDeclaration *superMap;

	// now, I should check the last point.
	// Is there any inclusion relation between these sets in the instance?

	// For this purpose, I should check the following.
	// the instance has mapped both of the sets: superSet and subSet.
	// Clearly it has mapped the subSet.
	// Therefore, only the super set should be checked.
	superMap = instance->findMapByContainer ((ClassType *)super->type);
	if (!superMap)
		return false;
	return ((ComponentType *)instance->type)->hasInclusion ((ClassType *) superMap->type, (ClassType *) type);
}

bool InclusionDeclaration::hasInclusion (ClassType *super, ClassType *sub)
{
	int i;

	if (!left->same (super))
		return false;
	for (i = 0; i < inclusion->array.count; ++i)
		if (inclusion->array.array[i]->same (sub))
			return true;
	return false;
}

/*
bool SuperDeclaration::hasInclusion (ClassType *super, ClassType *sub)
{
	if (!container->same (sub))
		return false;
	return ((ClassType *)type)->same (super);
}
*/

