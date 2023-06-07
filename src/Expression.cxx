
#include "parham.hxx"

Expression::Expression (ComponentType *_component, int _line)
	: NT (_component, _line)
{
	type = NULL;
//	castUpType = NULL;
}

ExpressionList::ExpressionList (ComponentType *_component, int _line)
	: NT (_component, _line)
{
}

void ExpressionList::add (Expression *exp)
{
	list.add (exp);
}

void ExpressionList::addFront (Expression *exp)
{
	list.addFront (exp);
}

AssignExpression::AssignExpression (Expression *_left, Expression *_right)
	: Expression (_left->component, _left->line)
{
	INIT (left);
	INIT (right);
}

ArrayExpression::ArrayExpression (Expression *_base, Expression *_index)
	: Expression (_base->component, _base->line)
{
	INIT (base);
	INIT (index);
}

CallExpression::CallExpression (Expression *_method, ExpressionList *_argList)
	: Expression (_method->component, _method->line)
{
	INIT (method);
	INIT (argList);
}

UnaryExpression::UnaryExpression (int _op, Expression *_expression)
	: Expression (_expression->component, _expression->line)
{
	INIT (op);
	INIT (expression);
}

BinaryExpression::BinaryExpression (int _op, Expression *_left, Expression *_right)
	: Expression (_left->component, _left->line)
{
	INIT (op);
	INIT (left);
	INIT (right);
}

ConditionalExpression::ConditionalExpression (Expression *_condition, Expression *_expression0, Expression *_expression1)
	: Expression (_condition->component, _condition->line)
{
	INIT (condition);
	INIT (expression0);
	INIT (expression1);
}

NewExpression::NewExpression (Type *_type, ExpressionList *_argList, ComponentType *_component, int _line)
	: ConstructorCallExpression (_type, _argList, _component, _line)
{
}

LiteralExpression::LiteralExpression (const char *_value, ComponentType *_component, int _line)
	: Expression (_component, _line)
{
	INIT (value);
}

FloatExpression::FloatExpression (const char *_value, ComponentType *_component, int _line)
	: LiteralExpression (_value, _component, _line)
{
}

IntegerExpression::IntegerExpression (const char *_value, ComponentType *_component, int _line)
	: LiteralExpression (_value, _component, _line)
{
}

StringExpression::StringExpression (const char *_value, ComponentType *_component, int _line)
	: LiteralExpression (_value, _component, _line)
{
}

NullExpression::NullExpression (ComponentType *_component, int _line)
	: LiteralExpression ("NULL", _component, _line)
{
}

ThisExpression::ThisExpression (ComponentType *_component, int _line)
	: LiteralExpression (THIS_PTR, _component, _line)
{
}

CastExpression::CastExpression (Type *_type, Expression *_expression)
	: Expression (_expression->component, _expression->line)
{
	INIT (type);
	INIT (expression);
}

PrimitiveCastExpression::PrimitiveCastExpression (Type *_type, Expression *_expression)
	: CastExpression (_type, _expression)
{
}

SetCastExpression::SetCastExpression (Type *_type, Expression *_expression)
	: CastExpression (_type, _expression)
{
}

/*
   analysis methods
   */

Type *Expression::analysis (ClassType *set, BlockSpace *ns, ExpressionList *argList)
{
	return type = analysis2 (set, ns, argList);
}

Type *AssignExpression::analysis2 (ClassType *set, BlockSpace *ns, ExpressionList *argList)
{
	// FIXME: there is a problem here.
	//	In assignment, the left side should not get up.
	//	In addition, the right side should match with the left.
	//	In other words, the assignment happens in the context of the instance that contains the field.
	left->analysis (set, ns, NULL);
	right->analysis (set, ns, NULL);
	if (!(left->type && right->type))
		return NULL;
	if (left->type->accept (right))
	{
		if (right->type->which == Type::TYPE_NULL)
			return right->type;
		return left->type;
	}
	error ("the left and right sides of the assignment are not compatible");
	return NULL;
}

Type *ArrayExpression::analysis2 (ClassType *set, BlockSpace *ns, ExpressionList *argList)
{
	base->analysis (set, ns, NULL);
	index->analysis (set, ns, NULL);
	if (base->type && index->type)
	{
		if (base->type->which == Type::TYPE_ARRAY)
		{
			return ((ArrayType *) base->type)->base;
		}
		error ("array type expected");
	}
	return NULL;
}

Type *CallExpression::analysis2 (ClassType *set, BlockSpace *ns, ExpressionList *argList2)
{
	if (!argList->analysis (set, ns))
		return NULL;
	if (!method->analysis (set, ns, argList))
		return NULL;
	switch (method->type->which)
	{
		case Type::TYPE_EXECUTION:
			break;
		default:
			method->error ("the expression does not denote a method");
			return NULL;
	}
	return ((ExecutionType *) method->type)->retType;
}

bool ExpressionList::analysis (ClassType *set, BlockSpace *ns)
{
	int i;
	bool ret;

	ret = true;
	for (i = 0; i < list.count; ++i)
		ret = list.array[i]->analysis (set, ns, NULL) && ret;
	return ret;
}

Type *UnaryExpression::analysis2 (ClassType *set, BlockSpace *ns, ExpressionList *argList)
{
	if (!expression->analysis (set, ns, NULL))
		return NULL;
	switch (op)
	{
		case OP_POST_INC:
		case OP_POST_DEC:
		case OP_PRE_INC:
		case OP_PRE_DEC:
		case OP_PLUS:
		case OP_MINUS:
		case OP_TILDA:
			switch (expression->type->which)
			{
				case Type::TYPE_INTEGER:
				case Type::TYPE_FLOAT:
					return expression->type;
				default:
					error ("the operator is not applicable on the given argument");
					return NULL;
			}
		case OP_PAR:
			return expression->type;
		case OP_NOT:
			switch (expression->type->which)
			{
				case Type::TYPE_INTEGER:
				case Type::TYPE_FLOAT:
				case Type::TYPE_CLASS:
				case Type::TYPE_ARRAY:
				case Type::TYPE_NULL:
				case Type::TYPE_ENUMERATION:
				case Type::TYPE_TYPEDEF:
					return new IntegerType ("bool", component, line);
				default:
					error ("operator is not applicable on the given argument");
					return NULL;
			}
		case OP_DELETE:
			switch (expression->type->which)
			{
				case Type::TYPE_CLASS:
					set = (ClassType *) (expression->type);
					if (set->isInternal)
						return new VoidType (component, line);
				default:
					break;
			}
			error ("Only objects of internal sets can be deleted");
			return NULL;
		default:
			error ("internal error: invalid unary operator");
			return NULL;
	}
}

Type *BinaryExpression::analysis2 (ClassType *set, BlockSpace *ns, ExpressionList *argList)
{
	left->analysis (set, ns, NULL);
	right->analysis (set, ns, NULL);
	if (!left->type || !right->type)
		return NULL;
	switch (op)
	{
		case OP_MUL_ASSIGN:
		case OP_DIV_ASSIGN:
		case OP_MOD_ASSIGN:
		case OP_LEFT_ASSIGN:
		case OP_RIGHT_ASSIGN:
		case OP_AND_ASSIGN:
		case OP_XOR_ASSIGN:
		case OP_OR_ASSIGN:
		case OP_MUL:
		case OP_DIV:
		case OP_MOD:
		case OP_SHIFT_LEFT:
		case OP_SHIFT_RIGHT:
		case OP_LESS:
		case OP_GREATER:
		case OP_LE:
		case OP_GE:
		case OP_BIT_AND:
		case OP_XOR:
		case OP_BIT_OR:
			switch (left->type->which)
			{
				case Type::TYPE_INTEGER:
				case Type::TYPE_FLOAT:
					break;
				default:
					error ("invalid operand");
					return NULL;
			}
			switch (right->type->which)
			{
				case Type::TYPE_INTEGER:
				case Type::TYPE_FLOAT:
					break;
				default:
					error ("invalid operand");
					return NULL;
			}
			return left->type;

		case OP_ADD_ASSIGN:
		case OP_ADD:
		case OP_SUB:
		case OP_SUB_ASSIGN:
			switch (left->type->which)
			{
				case Type::TYPE_INTEGER:
				case Type::TYPE_FLOAT:
					break;
				case Type::TYPE_ARRAY:
					if (right->type->which == Type::TYPE_INTEGER)
						return left->type;
				default:
					error ("invalid operand");
					return NULL;
			}
			switch (right->type->which)
			{
				case Type::TYPE_INTEGER:
				case Type::TYPE_FLOAT:
					break;
				default:
					error ("invalid operand");
					return NULL;
			}
			return left->type;

		case OP_LOG_OR:
		case OP_LOG_AND:
			return new IntegerType ("bool", component, line);
		case OP_NE:
		case OP_EQ:
			if (left->type->accept (right))
				return new IntegerType ("bool", component, line);
			if (right->type->accept (left))
				return new IntegerType ("bool", component, line);
			error ("the expressions cannot be compared");
			return NULL;
		default:
			internalError ("invalid operator");
			return NULL;
	}
}

Type *ConditionalExpression::analysis2 (ClassType *set, BlockSpace *ns, ExpressionList *argList)
{
	condition->analysis (set, ns, NULL);
	expression0->analysis (set, ns, NULL);
	expression1->analysis (set, ns, NULL);
	if (!condition->type || !expression0->type || !expression1->type)
		return NULL;
	switch (condition->type->which)
	{
		case Type::TYPE_INTEGER:
		case Type::TYPE_FLOAT:
		case Type::TYPE_CLASS:
		case Type::TYPE_TYPEDEF:
		case Type::TYPE_ENUMERATION:
		case Type::TYPE_NULL:
			break;
		default:
			error ("invalid conditional operand type");
			return NULL;
	}
	if (expression0->type->accept (expression1))
		return expression0->type;
	if (expression1->type->accept (expression0))
		return expression1->type;
	error ("the subexpressions of the conditional operator are not compatible");
	return NULL;
}

Type *ThisExpression::analysis2 (ClassType *set, BlockSpace *ns, ExpressionList *argList)
{
	if (!set)
		error ("'this' keyword can be used only within an expression");
	return set;
}

Type *NullExpression::analysis2 (ClassType *set, BlockSpace *ns, ExpressionList *argList)
{
	return new NullType (component, line);
}

Type *IntegerExpression::analysis2 (ClassType *set, BlockSpace *ns, ExpressionList *argList)
{
	return new IntegerType ("int", component, line);
}

Type *FloatExpression::analysis2 (ClassType *set, BlockSpace *ns, ExpressionList *argList)
{
	return new FloatType ("float", component, line);
}

Type *NewExpression::analysis2 (ClassType *container, BlockSpace *ns, ExpressionList *argList2)
{
	/*
	   type checking rules:
		the set is an internal set
		it is not abstract
		it is not state
		the arguments are correct
	  */
//	char buffer[1024];

	switch (type->which)
	{
		/*
		case TYPE_INTEGER:
			return TYPE_INTEGER;
		case TYPE_FLOAT:
			return TYPE_FLOAT;
			*/
		case Type::TYPE_CLASS:
			if (!((ClassType *) type)->canInstantiate ())
			{
				error ("Cannot instantiate abstract or interface sets");
				return NULL;
			}
			if (!argList->analysis (container, ns))
				return NULL;
			return analysis (ns, NULL);

			/*

		case TYPE_ARRAY:
			if (!index)
			{
				error ("Cannot allocate an array whose size is unkonwn");
				return NULL;
			}
#warning this code should be revised;
			return true;
			break;
			*/

		default:
			UNIMPLEMENTED ();
			return NULL;
	}
}

Type *StringExpression::analysis2 (ClassType *set, BlockSpace *ns, ExpressionList *argList)
{
	/*
	Dimension *dim;

	dim = new Dimension (component, line);
	dim->add (NULL);

	return new ArrayType (new IntegerType ("char", component, line), dim);
	*/
	return new ArrayType (new IntegerType ("char", component, line), NULL);
}

Type *PrimitiveCastExpression::analysis2 (ClassType *set, BlockSpace *ns, ExpressionList *argList)
{
	if (!expression->analysis (set, ns, argList))
		return NULL;
	switch (expression->type->which)
	{
		case Type::TYPE_INTEGER:
		case Type::TYPE_FLOAT:
		case Type::TYPE_TYPEDEF:
		case Type::TYPE_ENUMERATION:
			return type;
		default:
			error ("the cast expreession is invalid");
			return NULL;
	}
}

Type *SetCastExpression::analysis2 (ClassType *set, BlockSpace *ns, ExpressionList *argList)
{
	UNIMPLEMENTED ();
	return NULL;
}
/*
   publish methods
   */

void NewExpression::publish2 (FILE *f)
{
	ClassType *set;

	switch (type->which)
	{
		case Type::TYPE_INTEGER:
		case Type::TYPE_FLOAT:
			UNIMPLEMENTED ();
//			fprintf (f, "malloc (sizeof (struct %s))", set->pname);
			break;
		case Type::TYPE_CLASS:
			set = (ClassType *) type;
			fprintf (f, "((parhamptr = malloc (sizeof (struct %s)))? (", set->pname);
			ConstructorCallExpression::publish (f);
			fprintf (f, ", (struct %s *)parhamptr): 0)", set->pname);
			break;
		case Type::TYPE_ARRAY:
			UNIMPLEMENTED ();
			break;
		default:
			break;
	}
}

void ConstructorCallExpression::publish (FILE *f)
{
	constructorCall->publish (f);
}

void ArrayExpression::publish2 (FILE *f)
{
	switch (type->which)
	{
		case Type::TYPE_CLASS:
			fprintf (f, "(");
			base->publish (f);
			fprintf (f, " + (");
			index->publish (f);
			fprintf (f, "))");
			break;
		default:
			base->publish (f);
			fprintf (f, "[");
			index->publish (f);
			fprintf (f, "]");
	}
}

void AssignExpression::publish2 (FILE *f)
{
	left->publish (f);
	fprintf (f, " = ");
	right->publish (f);
}

void CallExpression::publish2 (FILE *f)
{
	bool first;

	first = method->publishLast (f);
	fprintf (f, " (");
	method->publishExceptLast (f);
	argList->publishx (f, first);
	fprintf (f, ")");
}

void ExpressionList::publishx (FILE *f, bool comma)
{
	int i;

	for (i = 0; i < list.count; ++i)
	{
		if (i || comma)
			fprintf (f, ", ");
		list.array[i]->publish (f);
	}
}

void LiteralExpression::publish2 (FILE *f)
{
	fprintf (f, "%s", value);
}

void ConditionalExpression::publish2 (FILE *f)
{
	condition->publish (f);
	fprintf (f, "? ");
	expression0->publish (f);
	fprintf (f, ": ");
	expression1->publish (f);
}

void BinaryExpression::publish2 (FILE *f)
{
	left->publish (f);
	fprintf (f, " ");
	switch (op)
	{
		case OP_ADD:
			fprintf (f, "+");
			break;
		case OP_ADD_ASSIGN:
			fprintf (f, "+=");
			break;
		case OP_AND_ASSIGN:
			fprintf (f, "&=");
			break;
		case OP_BIT_AND:
			fprintf (f, "&");
			break;
		case OP_BIT_OR:
			fprintf (f, "|");
			break;
		case OP_DIV:
			fprintf (f, "/");
			break;
		case OP_DIV_ASSIGN:
			fprintf (f, "/=");
			break;
		case OP_EQ:
			fprintf (f, "==");
			break;
		case OP_GE:
			fprintf (f, ">=");
			break;
		case OP_GREATER:
			fprintf (f, ">");
			break;
		case OP_LE:
			fprintf (f, "<=");
			break;
		case OP_LEFT_ASSIGN:
			fprintf (f, "<<=");
			break;
		case OP_LESS:
			fprintf (f, "<");
			break;
		case OP_LOG_AND:
			fprintf (f, "&&");
			break;
		case OP_LOG_OR:
			fprintf (f, "||");
			break;
		case OP_MOD:
			fprintf (f, "%%");
			break;
		case OP_MOD_ASSIGN:
			fprintf (f, "%%=");
			break;
		case OP_MUL:
			fprintf (f, "*");
			break;
		case OP_MUL_ASSIGN:
			fprintf (f, "*=");
			break;
		case OP_NE:
			fprintf (f, "!=");
			break;
		case OP_OR_ASSIGN:
			fprintf (f, "|=");
			break;
		case OP_RIGHT_ASSIGN:
			fprintf (f, ">>=");
			break;
		case OP_SHIFT_LEFT:
			fprintf (f, "<<");
			break;
		case OP_SHIFT_RIGHT:
			fprintf (f, ">>");
			break;
		case OP_SUB:
			fprintf (f, "-");
			break;
		case OP_SUB_ASSIGN:
			fprintf (f, "-=");
			break;
		case OP_XOR:
			fprintf (f, "^");
			break;
		case OP_XOR_ASSIGN:
			fprintf (f, "^=");
			break;
	}
	fprintf (f, " ");
	right->publish (f);
}

void UnaryExpression::publish2 (FILE *f)
{
	switch (op)
	{
		case OP_PRE_INC:
			fprintf (f, "++");
			expression->publish (f);
			break;
		case OP_PRE_DEC:
			fprintf (f, "--");
			expression->publish (f);
			break;
		case OP_PLUS:
			fprintf (f, "+");
			expression->publish (f);
			break;
		case OP_MINUS:
			fprintf (f, "-");
			expression->publish (f);
			break;
		case OP_TILDA:
			fprintf (f, "~");
			expression->publish (f);
			break;
		case OP_NOT:
			fprintf (f, "!");
			expression->publish (f);
			break;
		case OP_POST_INC:
			expression->publish (f);
			fprintf (f, "++");
			break;
		case OP_POST_DEC:
			expression->publish (f);
			fprintf (f, "--");
			break;
		case OP_PAR:
			fprintf (f, "(");
			expression->publish (f);
			fprintf (f, ")");
			break;
		case OP_DELETE:
			fprintf (f, "free (");
			expression->publish (f);
			fprintf (f, ")");
	}
}

void CastExpression::publish2 (FILE *f)
{
	fprintf (f, "(");
	type->publishPre (false, false, f);
	fprintf (f, ") ");
	expression->publish (f);
}

bool Expression::publishLast (FILE *f)
{
	Declaration *decl;

	/*
	if (convertPath.path.count == 0)
		return false;
	decl = convertPath.path.array[convertPath.path.count - 1]->attrib;
	*/
	decl = rm.castPathx->getLast ();
	if (!decl)
		return false;
	if (decl->isProperty && ((PropertyDeclaration *) decl)->container && decl->modifier->has (MOD_VIRTUAL | MOD_REQUIRES))
	{
		publishExceptLast (f);
		fprintf (f, "->");
	}
	decl->publishReference (f);
	return decl->isProperty && ((PropertyDeclaration *)decl)->container;
}

void Expression::publishExceptLast (FILE *f)
{
	rm.castPathx->publish (f, false, this);
}

void Expression::publish (FILE *f)
{
	rm.castPathx->publish (f, true, this);
}

ConstructorCallExpression::ConstructorCallExpression (Type *_type, ExpressionList *_argList, ComponentType *_component, int _line)
	: Expression (_component, _line)
{
	INIT (type);
	INIT (argList);
	constructorCall = NULL;
}

ConstructorCall::ConstructorCall (ExpressionList *_argList, ComponentType *_component, int _line)
	: ConstructorCallExpression (NULL, _argList, _component, _line)
{
}

ConstructorCallByID::ConstructorCallByID (Identifier *_identifier, ExpressionList *_argList)
	: ConstructorCall (_argList, _identifier->component, _identifier->line)
{
	INIT (identifier);
}

ConstructorCallByQID::ConstructorCallByQID (QualifiedIdentifier *_qid, ExpressionList *_argList)
	: ConstructorCall (_argList, _qid->component, _qid->line)
{
	INIT (qid);
}

ConstructorCallBySet::ConstructorCallBySet (ClassType *_set, ExpressionList *_argList, ComponentType *_component, int _line)
	: ConstructorCall (_argList, _component, _line)
{
	INIT (set);
}

void ConstructorCallExpression::makeCall (PropertyDeclaration *pd, ConstructorDeclaration *constructor)
//void ConstructorCallExpression::makeCall (const char *name, ConstructorDeclaration *constructor)
{
	ClassType *set;
	Expression *constructorID;
	Expression *vtableArg;
	char buffer[1024];

	set = constructor->container;

	if (pd)
	{
		if (pd->container->mainSet)
			sprintf (buffer, "%s", pd->identifier->value);
		else
			sprintf (buffer, "%s->%s", THIS_PTR, pd->identifier->value);
	}
	else
		sprintf (buffer, "(struct %s *) parhamptr", ((ClassType *) type)->pname);

	// making the call expression containing object name (thisPtr) and constructor name
	constructorID = new FieldExpression (
			new StringExpression (strdup (buffer), component, line),
			new Identifier ("", component, line)
			);
//	constructorID->convertPath.add (CONVERT_DOWN, constructor);
	constructorID->rm.castPathx->follow (constructor);
	constructorID->type = constructor->type;

	// adding VT pointer
	vtableArg = set->vtable.makeCallArgument (pd);
	if (vtableArg)
		argList->addFront (vtableArg);

	constructorCall = new CallExpression (constructorID, argList);
}

Type *ConstructorCallExpression::analysis (BlockSpace *ns, PropertyDeclaration *pd)
{
	ConstructorDeclaration *constructor;
	ClassType *set;

	set = (ClassType *)type;
	constructor = set->cm.findConstructor (argList);
	if (!constructor)
	{
		error ("there is no constructor with the given arguments");
		return NULL;
	}

	makeCall (pd, constructor);

	/*
	// making the call expression containing object name (thisPtr) and constructor name
	constructorID = new FieldExpression2 (
			new StringExpression (strdup (thisPtr), component, line),
			new Identifier ("", component, line)
			);
	constructorID->convertPath.add (CONVERT_DOWN, constructor);
	constructorID->type = constructor->type;

	// adding VT pointer
	if (set->needVT ())
	{
		sprintf (buffer, "&%s_vt", set->pname);
		argList->addFront (new StringExpression (strdup (buffer), component, line));
	}

	constructorCall = new CallExpression (constructorID, argList);
	*/
	return type;
}

Declaration *ConstructorCall::analysis (ClassType *set, BlockSpace *ns)
{
	PropertyDeclaration *decl;

	if (!argList->analysis (set, ns))
		return NULL;
	decl = findSet (set, ns);
	if (!decl)
	{
		error ("the constructor name does not match any constructor");
		return NULL;
	}
	type = decl->type;
	if (ConstructorCallExpression::analysis (ns, decl))
	{
		if (decl->cm.incCounter () > 1)
//		if (decl->constructorCount > 0)
			error ("more than one constructor is defined for %s whose type is %s", decl->identifier->value, ((ClassType *) decl->type)->pname);
//		decl->constructorCount++;
		return decl;
	}
	return NULL;
}

PropertyDeclaration *ConstructorCallByID::findSet (ClassType *set, BlockSpace *ns)
{
	int i;

	for (i = 0; i < set->attributeList.count; ++i)
		if (set->attributeList.array[i]->identifier->match (identifier))
		{
			switch (set->attributeList.array[i]->type->which)
			{
				case Type::TYPE_CLASS:
					if (set->attributeList.array[i]->isRef)
						continue;
					return set->attributeList.array[i];
				default:
					break;
			}
		}
	/*
	for (i = 0; i < set->mapAttributes.count; ++i)
		if (set->mapAttributes.array[i]->identifier->match (identifier))
			return set->mapAttributes.array[i];
			*/
	error ("%s is not a field of the class %s", identifier->value, set->identifier->value);
	return NULL;
}

PropertyDeclaration *ConstructorCallByQID::findSet (ClassType *set, BlockSpace *ns)
{
	int i;
	ComponentDeclaration *ret;
	InstanceDeclaration *instance;

	ret = NULL;
	if (qid->list.count == 2)
	{
		for (i = 0; i < set->mapAttributes.count; ++i)
		{
			instance = set->mapAttributes.array[i]->instance;
			if (!instance->identifier)
				continue;
			if (!instance->identifier->match (qid->list.array[0]))
				continue;
			if (!((ClassType *)set->mapAttributes.array[i]->type)->identifier->match (qid->list.array[1]))
				continue;
			if (ret)
			{
				char b1[1024];
				qid->flat (b1, '.');
				error ("multiple instances are matched with the given name (%s)", b1);
				return ret;
			}
			ret = set->mapAttributes.array[i];
		}
	}
	if (ret)
		return ret;

	for (i = 0; i < set->mapAttributes.count; ++i)
	{
		instance = set->mapAttributes.array[i]->instance;
		if (instance->qid->match (qid, qid->list.count - 1))
		{
			if (ret)
			{
				char b1[1024];
				qid->flat (b1, '.');
				error ("multiple instances are matched with the given name (%s)", b1);
				return ret;
			}
			ret = set->mapAttributes.array[i];
		}
	}
	return ret;

	/*
	InstanceDeclaration *instance;
	ComponentDeclaration *ret;

	if (qid->list.count < 2)
	{
		error ("the qualified identifier does not specify a set");
		return NULL;
	}
	instance = component->findInstance (qid, qid->list.count - 1);
	if (!instance)
	{
		error ("there is no component with the name %s", qid->list.array[0]->value);
		return NULL;
	}
	ret = instance->findMapByType (qid->list.array[qid->list.count - 1]);
	if (!ret)
	{
		char buffer[1024];

		qid->flat (buffer, qid->list.count - 1, '.');
		error ("there is no interface set with the given name: %s in the component %s", qid->list.array[qid->list.count - 1]->value, buffer);
		return NULL;
	}
	return ret;
	*/
}

PropertyDeclaration *ConstructorCallBySet::findSet (ClassType *set2, BlockSpace *ns)
{
	int i;
	char buffer[1024];

	for (i = 0; i < set2->superList.count; ++i)
		if (set2->superList.array[i]->inclusion->left->match (set))
		{
			sprintf (buffer, "&%s->%s", THIS_PTR, set2->superList.array[i]->identifier->value);
			return set2->superList.array[i];
		}
	error ("%s is not a super class of %s", set->identifier->value, set2->identifier->value);
	return NULL;
}

Type *ConstructorCall::analysis2 (ClassType *set, BlockSpace *ns, ExpressionList *argList)
{
	return NULL;
}

void ConstructorCall::publish2 (FILE *f)
{
}

/*
ConvertPathItem::ConvertPathItem (int _direction, PropertyDeclaration *_attrib)
{
	INIT (direction);
	INIT (attrib);
}

void ConvertPath::add (int direction, PropertyDeclaration *attrib)
{
	if ((path.count > 0) && (path.array[path.count - 1]->direction != direction) && (path.array[path.count - 1]->attrib == attrib))
		path.count--;
	else
		path.add (new ConvertPathItem (direction, attrib));
}
*/
bool Expression::needPublish ()
{
	return true;
}

ConstructorCallImplicit::ConstructorCallImplicit (ComponentType *_component, int _line)
	: ConstructorCall (new ExpressionList (_component, _line), _component, _line)
{
}

PropertyDeclaration *ConstructorCallImplicit::findSet (ClassType *set, BlockSpace *ns)
{
	return NULL;
}

FieldExpression::FieldExpression (Expression *_object, Identifier *_identifier)
	: Expression (_identifier->component, _identifier->line)
{
	INIT (object);
	INIT (identifier);
	enumeration = NULL;
}

Type *FieldExpression::analysis2 (ClassType *set, BlockSpace *ns, ExpressionList *argList)
{
	ClassType *s;

	object->analysis (set, ns, NULL);
	if (!object->type)
		return NULL;
	switch (object->type->which)
	{
		case Type::TYPE_CLASS:
			s = (ClassType *) object->type;
			/*
			if (convertPath.path.count > 0)
			{
				if (convertPath.path.array[0]->attrib->which == DECL_ASSOCIATION)
				{
					s->up (&convertPath);
					return lookup (s->mapper.target, NULL, argList, &convertPath);
				}
			}
			*/
			return lookup (NULL, s, NULL, argList);
		case Type::TYPE_COMPONENT:
			return lookup ((ComponentType *) object->type, NULL, NULL, argList);
		default:
			error ("the type of an expression must be class or component");
			return NULL;
	}
}

void FieldExpression::publish2 (FILE *f)
{
	if (enumeration)
		enumeration->publish (f);
	else if (object)
		object->publish (f);
}

bool FieldExpression::needPublish ()
{
	return object;
}

// FIXME: move to rm RESOLVE
Type *FieldExpression::lookup (ComponentType *component2, ClassType *set, BlockSpace *ns, ExpressionList *argList)
{
	Declaration *decl;
	Array<ComponentDeclaration> p;
	int i;
	Type *ret;

	// local lookup
	if (ns)
	{
#ifdef DEBUG
		fprintf (stderr, "looking up <%s> on given ns\n", identifier->value);
#endif
		decl = ns->lookup (identifier);
		if (decl)
			return rm.castPathx->follow ((PropertyDeclaration *) decl);
	}

	// class lookup
	if (set)
	{
#ifdef DEBUG
		fprintf (stderr, "looking up <%s> on set <%s>\n", identifier->value, set->pname);
#endif
		decl = set->rm.ns.lookup (identifier, argList, &p);
		if (decl)
		{
			for (i = 0; i < p.count; ++i)
				rm.castPathx->follow (p.array[i]);
			ret = rm.castPathx->follow ((PropertyDeclaration *) decl);

			if (!object && !set->mainSet)
				object = new ThisExpression (component, line);
			return ret;
	//		return castPathx->getTypex ();
		}
	}

	// component lookup
	if (component2)
	{
		decl = component2->rm.lookup (identifier, argList);
		if (decl)
		{
			switch (decl->type->which)
			{
				case Type::TYPE_ENUMERATION:
					enumeration = (EnumerationItem *) decl;
					return decl->type;
				default:
					return rm.castPathx->follow ((PropertyDeclaration *) decl);
			}
		}
	}
	identifier->erUndefined ();
	return NULL;
}

IdentifierExpression::IdentifierExpression (Identifier *_identifier)
	: FieldExpression (NULL, _identifier)
{
}

Type *IdentifierExpression::analysis2 (ClassType *set, BlockSpace *ns, ExpressionList *argList)
{
	return lookup (component, set, ns, argList);
}

