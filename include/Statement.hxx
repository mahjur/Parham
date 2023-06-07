
#ifndef __STATEMENT__
#define __STATEMENT__

class SwitchItem: public NT
{
	public:
		SwitchItem (ComponentType *_component, int _line);
		virtual void publish (int newIndent, int indent, FILE *f) = 0;
		virtual void analysis (ClassType *set, BlockSpace *ns, Type *retType);
};

class SwitchItemList: public NT
{
	public:
		Array<SwitchItem> list;

		SwitchItemList (ComponentType *_component, int _line);
		void add (SwitchItem *);
		void publish (int indent, FILE *f);
		virtual void analysis (ClassType *set, BlockSpace *ns, Type *retType);
};

class SwitchIntegerLabel: public SwitchItem
{
	public:
		IntegerExpression *label;

		SwitchIntegerLabel (IntegerExpression *_label);
		virtual void publish (int newIndent, int indent, FILE *f);
		virtual void analysis (ClassType *set, BlockSpace *ns, Type *retType);
};

class SwitchIdentifierLabel: public SwitchItem
{
	public:
		Identifier *label;

		SwitchIdentifierLabel (Identifier *_label);
		virtual void publish (int newIndent, int indent, FILE *f);
		virtual void analysis (ClassType *set, BlockSpace *ns, Type *retType);
};

class SwitchDefault: public SwitchItem
{
	public:
		SwitchDefault (ComponentType *_component, int _line);
		virtual void publish (int newIndent, int indent, FILE *f);
};

class Statement: public SwitchItem
{
	public:
		Statement (ComponentType *_component, int _line);
};

class BlockStatement: public Statement
{
	public:
		Array<Statement> *list;

		BlockStatement (Array<Statement> *_list, ComponentType *_component, int _line);
		virtual void publish (int newIndent, int indent, FILE *f);
		virtual void analysis (ClassType *set, BlockSpace *ns, Type *retType);
};

class BreakStatement: public Statement
{
	public:
		BreakStatement (ComponentType *_component, int _line);
		virtual void publish (int newIndent, int indent, FILE *f);
};

class ContinueStatement: public Statement
{
	public:
		ContinueStatement (ComponentType *_component, int _line);
		virtual void publish (int newIndent, int indent, FILE *f);
};

class EmptyStatement: public Statement
{
	public:
		EmptyStatement (ComponentType *_component, int _line);
		virtual void publish (int newIndent, int indent, FILE *f);
};

class ExpressionStatement: public Statement
{
	public:
		Expression *expression;

		ExpressionStatement (Expression *_expression);
		virtual void publish (int newIndent, int indent, FILE *f);
		virtual void analysis (ClassType *set, BlockSpace *ns, Type *retType);
};

class ForStatement: public Statement
{
	public:
		Expression *expression0;
		Expression *expression1;
		Expression *expression2;
		Statement *statement;

		ForStatement (Expression *_expression0, Expression *_expression1, Expression *_expression2, Statement *_statement);
		virtual void publish (int newIndent, int indent, FILE *f);
		virtual void analysis (ClassType *set, BlockSpace *ns, Type *retType);
};

class GotoStatement: public Statement
{
	public:
		Identifier *label;

		GotoStatement (Identifier *_label);
		virtual void publish (int newIndent, int indent, FILE *f);
};

class IfStatement: public Statement
{
	public:
		Expression *expression;
		Statement *statement0, *statement1;

		IfStatement (Expression *_expression, Statement *_statement0, Statement *_statement1);
		virtual void publish (int newIndent, int indent, FILE *f);
		virtual void analysis (ClassType *set, BlockSpace *ns, Type *retType);
};

class LabeledStatement: public Statement
{
	public:
		Identifier *label;
		Statement *statement;

		LabeledStatement (Identifier *_label, Statement *_statement);
		virtual void publish (int newIndent, int indent, FILE *f);
		virtual void analysis (ClassType *set, BlockSpace *ns, Type *retType);
};

class ReturnStatement: public Statement
{
	public:
		Expression *expression;

		ReturnStatement (Expression *_expression, ComponentType *_component, int _line);
		virtual void publish (int newIndent, int indent, FILE *f);
		virtual void analysis (ClassType *set, BlockSpace *ns, Type *retType);
};

class SwitchStatement: public Statement
{
	public:
		Expression *expression;
		SwitchItemList *list;

		SwitchStatement (Expression *_expression, SwitchItemList *_list);
		virtual void publish (int newIndent, int indent, FILE *f);
		virtual void analysis (ClassType *set, BlockSpace *ns, Type *retType);
};

class WhileStatement: public Statement
{
	public:
		Expression *expression;
		Statement *statement;

		WhileStatement (Expression *_expression, Statement *_statement);
		virtual void publish (int newIndent, int indent, FILE *f);
		virtual void analysis (ClassType *set, BlockSpace *ns, Type *retType);
};

class DoStatement: public Statement
{
	public:
		Statement *statement;
		Expression *expression;

		DoStatement (Statement *_statement, Expression *_expression);
		virtual void publish (int newIndent, int indent, FILE *f);
		virtual void analysis (ClassType *set, BlockSpace *ns, Type *retType);
};

class DeclarationStatement: public Statement, public Declaration
{
	public:
		Initializer *initializer;
		DeclarationStatement (Modifier *_modifier, Type *_type, InitializedIdentifier *ii);
		virtual void publish (int newIndent, int indent, FILE *f);
		virtual void analysis (ClassType *set, BlockSpace *ns, Type *retType);
};

/*
class SwitchStatement: public Statement
{
	public:
		Expression *expression;
		SwitchItemList *list;

		SwitchStatement (Expression *_expression, SwitchItemList *_list);
};
*/
#endif

