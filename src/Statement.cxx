
#include "parham.hxx"

SwitchItem::SwitchItem (ComponentType *_component, int _line)
	: NT (_component, _line)
{
}

SwitchItemList::SwitchItemList (ComponentType *_component, int _line)
	: NT (_component, _line)
{
}

void SwitchItemList::add (SwitchItem *item)
{
	list.add (item);
}

SwitchIntegerLabel::SwitchIntegerLabel (IntegerExpression *_label)
	: SwitchItem (_label->component, _label->line)
{
	INIT (label);
}

SwitchIdentifierLabel::SwitchIdentifierLabel (Identifier *label)
	: SwitchItem (label->component, label->line)
{
	this->label = label;
}

SwitchDefault::SwitchDefault (ComponentType *_component, int _line)
	: SwitchItem (_component, _line)
{
}

Statement::Statement (ComponentType *_component, int _line)
	: SwitchItem (_component, _line)
{
}

BlockStatement::BlockStatement (Array<Statement> *_list, ComponentType *_component, int _line)
	: Statement (_component, _line)
{
	INIT (list);
}

BreakStatement::BreakStatement (ComponentType *_component, int _line)
	: Statement (_component, _line)
{
}

ContinueStatement::ContinueStatement (ComponentType *_component, int _line)
	: Statement (_component, _line)
{
}

EmptyStatement::EmptyStatement (ComponentType *_component, int _line)
	: Statement (_component, _line)
{
}

ExpressionStatement::ExpressionStatement (Expression *_expression)
	: Statement (_expression->component, _expression->line)
{
	INIT (expression);
}

ForStatement::ForStatement (Expression *_expression0, Expression *_expression1, Expression *_expression2, Statement *_statement)
	: Statement (_statement->component, _statement->line)
{
	INIT (expression0);
	INIT (expression1);
	INIT (expression2);
	INIT (statement);
}

GotoStatement::GotoStatement (Identifier *_label)
	: Statement (_label->component, _label->line)
{
	INIT (label);
}

IfStatement::IfStatement (Expression *_expression, Statement *_statement0, Statement *_statement1)
	: Statement (_expression->component, _expression->line)
{
	INIT (expression);
	INIT (statement0);
	INIT (statement1);
}

LabeledStatement::LabeledStatement (Identifier *_label, Statement *_statement)
	: Statement (_label->component, _label->line)
{
	INIT (label);
	INIT (statement);
}

ReturnStatement::ReturnStatement (Expression *_expression, ComponentType *_component, int _line)
	: Statement (_component, _line)
{
	INIT (expression);
}

SwitchStatement::SwitchStatement (Expression *_expression, SwitchItemList *_list)
	: Statement (_expression->component, _expression->line)
{
	INIT (expression);
	INIT (list);
}

WhileStatement::WhileStatement (Expression *_expression, Statement *_statement)
	: Statement (_expression->component, _expression->line)
{
	INIT (expression);
	INIT (statement);
}

DoStatement::DoStatement (Statement *_statement, Expression *_expression)
	: Statement (_statement->component, _statement->line)
{
	INIT (statement);
	INIT (expression);
}

DeclarationStatement::DeclarationStatement (Modifier *_modifier, Type *_type, InitializedIdentifier *ii)
	: Statement (_type->component, _type->line), Declaration (DECL_LOCAL, false, _modifier, _type, true, ii->identifier, _type->component, _type->line)
{
	initializer = ii->initializer;
}

void BlockStatement::publish (int newIndent, int indent, FILE *f)
{
	int i;

	putIndent (f, indent);
	fprintf (f, "{\n");
	for (i = 0; i < list->count; ++i)
		list->array[i]->publish (0, indent + 1, f);
	putIndent (f, indent);
	fprintf (f, "}\n");
}

void BreakStatement::publish (int newIndent, int indent, FILE *f)
{
	putIndent (f, newIndent + indent);
	fprintf (f, "break;\n");
}

void ContinueStatement::publish (int newIndent, int indent, FILE *f)
{
	putIndent (f, newIndent + indent);
	fprintf (f, "continue;\n");
}

void EmptyStatement::publish (int newIndent, int indent, FILE *f)
{
	putIndent (f, newIndent + indent);
	fprintf (f, ";\n");
}

void ForStatement::publish (int newIndent, int indent, FILE *f)
{
	indent += newIndent;

	putIndent (f, indent);
	fprintf (f, "for (");

	if (expression0)
		expression0->publish (f);
	else
		fprintf (f, " ");
	fprintf (f, "; ");

	if (expression1)
		expression1->publish (f);
	else
		fprintf (f, " ");
	fprintf (f, "; ");

	if (expression2)
		expression2->publish (f);
	else
		fprintf (f, " ");
	fprintf (f, ")\n");

	statement->publish (1, indent, f);

}

void GotoStatement::publish (int newIndent, int indent, FILE *f)
{
	putIndent (f, newIndent + indent);
	fprintf (f, "goto %s\n", label->value);
}

void IfStatement::publish (int newIndent, int indent, FILE *f)
{
	indent += newIndent;
	putIndent (f, indent);
	fprintf (f, "if (");
	expression->publish (f);
	fprintf (f, ")\n");
	statement0->publish (1, indent, f);
	if (statement1)
	{
		putIndent (f, indent);
		fprintf (f, "else\n");
		statement1->publish (1, indent, f);
	}
}

void LabeledStatement::publish (int newIndent, int indent, FILE *f)
{
	fprintf (f, "%s:\n", label->value);
	statement->publish (newIndent, indent, f);
}

void ReturnStatement::publish (int newIndent, int indent, FILE *f)
{
	putIndent (f, newIndent + indent);
	fprintf (f, "return");
	if (expression)
	{
		fprintf (f, " ");
		expression->publish (f);
	}
	fprintf (f, ";\n");
	{
	}
}

void SwitchStatement::publish (int newIndent, int indent, FILE *f)
{
	indent += newIndent;

	putIndent (f, indent);
	fprintf (f, "switch (");
	expression->publish (f);
	fprintf (f, ")\n");
	list->publish (indent, f);
}

void WhileStatement::publish (int newIndent, int indent, FILE *f)
{
	indent += newIndent;

	putIndent (f, indent);
	fprintf (f, "while (");
	expression->publish (f);
	fprintf (f, ")\n");
	statement->publish (1, indent, f);
}

void DoStatement::publish (int newIndent, int indent, FILE *f)
{
	indent += newIndent;

	putIndent (f, indent);
	fprintf (f, "do\n");
	statement->publish (1, indent, f);
	putIndent (f, indent);
	fprintf (f, "while (");
	expression->publish (f);
	fprintf (f, ");\n");
}

void ExpressionStatement::publish (int newIndent, int indent, FILE *f)
{
	putIndent (f, newIndent + indent);
	expression->publish (f);
	fprintf (f, ";\n");
}

void SwitchDefault::publish (int newIndent, int indent, FILE *f)
{
	putIndent (f, indent);
	fprintf (f, "default:\n");
}

void SwitchIntegerLabel::publish (int newIndent, int indent, FILE *f)
{
	putIndent (f, indent);
	fprintf (f, "case ");
	label->publish (f);
	fprintf (f, ":\n");
}

void SwitchIdentifierLabel::publish (int newIndent, int indent, FILE *f)
{
#warning TODO
}

void SwitchItemList::publish (int indent, FILE *f)
{
	int i;

	putIndent (f, indent);
	fprintf (f, "{\n");
	for (i = 0; i < list.count; ++i)
		list.array[i]->publish (1, indent + 1, f);
	putIndent (f, indent);
	fprintf (f, "}\n");
}

void DeclarationStatement::publish (int newIndent, int indent, FILE *f)
{
	Statement::putIndent (f, newIndent + indent);
	Declaration::publish (f);
	fprintf (f, ";\n");
}

void BlockStatement::analysis (ClassType *set, BlockSpace *pns, Type *retType)
{
	int i;
	BlockSpace ns (pns);

	for (i = 0; i < list->count; ++i)
		list->array[i]->analysis (set, &ns, retType);
}

void ExpressionStatement::analysis (ClassType *set, BlockSpace *ns, Type *retType)
{
	expression->analysis (set, ns, NULL);
}

void ForStatement::analysis (ClassType *set, BlockSpace *ns, Type *retType)
{
	if (expression0)
		expression0->analysis (set, ns, NULL);
	if (expression1)
		expression1->analysis (set, ns, NULL);
	if (expression2)
		expression2->analysis (set, ns, NULL);
	statement->analysis (set, ns, retType);
}

void IfStatement::analysis (ClassType *set, BlockSpace *ns, Type *retType)
{
	expression->analysis (set, ns, NULL);
	statement0->analysis (set, ns, retType);
	if (statement1)
		statement1->analysis (set, ns, retType);
}

void LabeledStatement::analysis (ClassType *set, BlockSpace *ns, Type *retType)
{
	statement->analysis (set, ns, retType);
}

void ReturnStatement::analysis (ClassType *set, BlockSpace *ns, Type *retType)
{
	if (expression)
	{
		expression->analysis (set, ns, NULL);
		if (expression->type)
			if (!retType->accept (expression))
				error ("Retrun value does not match the return type of the method");
	}
	else if (retType->which != Type::TYPE_VOID)
		error ("Void method/constructor cannot return a value");
}

void SwitchStatement::analysis (ClassType *set, BlockSpace *ns, Type *retType)
{
	expression->analysis (set, ns, NULL);
	list->analysis (set, ns, retType);
}

void WhileStatement::analysis (ClassType *set, BlockSpace *ns, Type *retType)
{
	expression->analysis (set, ns, NULL);
	statement->analysis (set, ns, retType);
}

void DoStatement::analysis (ClassType *set, BlockSpace *ns, Type *retType)
{
	statement->analysis (set, ns, retType);
	expression->analysis (set, ns, NULL);
}

void DeclarationStatement::analysis (ClassType *set, BlockSpace *ns, Type *retType)
{
	ns->addSimple (this);
	if (initializer)
		initializer->analysis ();
}

void SwitchItemList::analysis (ClassType *set, BlockSpace *ns, Type *retType)
{
	int i;

	for (i = 0; i < list.count; ++i)
		list.array[i]->analysis (set, ns, retType);
}

void SwitchIntegerLabel::analysis (ClassType *set, BlockSpace *ns, Type *retType)
{
	label->analysis (set, ns, NULL);
}

void SwitchIdentifierLabel::analysis (ClassType *set, BlockSpace *ns, Type *retType)
{
#warning TODO
}

void SwitchItem::analysis (ClassType *set, BlockSpace *ns, Type *retType)
{
}

