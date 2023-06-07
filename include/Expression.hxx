
#ifndef __EXPRESSION__
#define __EXPRESSION__

#define OP_ASSIGN	1
#define OP_MUL_ASSIGN	2
#define OP_DIV_ASSIGN	3
#define OP_MOD_ASSIGN	4
#define OP_ADD_ASSIGN	5
#define OP_SUB_ASSIGN	6
#define OP_LEFT_ASSIGN	7
#define OP_RIGHT_ASSIGN	8
#define OP_AND_ASSIGN	9
#define OP_XOR_ASSIGN	10
#define OP_OR_ASSIGN	11
#define OP_LOG_OR	12
#define OP_LOG_AND	13
#define OP_BIT_OR	14
#define OP_XOR		15
#define OP_BIT_AND	16
#define OP_EQ		17
#define OP_NE		18
#define OP_LESS		19
#define OP_GREATER	20
#define OP_LE		21
#define OP_GE		22
#define OP_SHIFT_LEFT	23
#define OP_SHIFT_RIGHT	24
#define OP_ADD		25
#define OP_SUB		26
#define OP_MUL		27
#define OP_DIV		28
#define OP_MOD		29
#define OP_PRE_INC	30
#define OP_PRE_DEC	31
#define OP_PLUS		32
#define OP_MINUS	33
#define OP_TILDA	34
#define OP_NOT		35
#define OP_POST_INC	36
#define OP_POST_DEC	37
#define OP_PAR		38
#define OP_DELETE	39

#define THIS_PTR	"thisthis"

class Expression: public NT
{
	public:
		Type *type;
//		Cast castPathx[1];
		ExpressionResolve rm;

		Expression (ComponentType *_component, int _line);
		Type *analysis (ClassType *set, BlockSpace *ns, ExpressionList *argList);
		virtual Type *analysis2 (ClassType *set, BlockSpace *ns, ExpressionList *argList) = 0;
		virtual void publish2 (FILE *f) = 0;
		virtual bool needPublish ();
		void publish (FILE *f);

		void publishExceptLast (FILE *f);
		bool publishLast (FILE *f);
};

class ExpressionList: public NT
{
	public:
		Array<Expression> list;

		ExpressionList (ComponentType *_component, int _line);
		void add (Expression *);
		void addFront (Expression *);
		void publishx (FILE *f, bool comma);
		bool analysis (ClassType *set, BlockSpace *ns);
};

class AssignExpression: public Expression
{
	public:
		Expression *left, *right;

		AssignExpression (Expression *, Expression *);
		virtual Type *analysis2 (ClassType *set, BlockSpace *ns, ExpressionList *argList);
		virtual void publish2 (FILE *f);
};

class ArrayExpression: public Expression
{
	public:
		Expression *base, *index;

		ArrayExpression (Expression *_base, Expression *_index);
		virtual void publish2 (FILE *f);
		virtual Type *analysis2 (ClassType *set, BlockSpace *ns, ExpressionList *argList);
};

class CallExpression: public Expression
{
	public:
		Expression *method;
		ExpressionList *argList;

		CallExpression (Expression *_method, ExpressionList *_argList);
		virtual void publish2 (FILE *f);
		virtual Type *analysis2 (ClassType *set, BlockSpace *ns, ExpressionList *argList);
};

class FieldExpression: public Expression
{
	protected:
		Type *lookup (ComponentType *component2, ClassType *set, BlockSpace *ns, ExpressionList *argList);
		EnumerationItem *enumeration;
	public:
		Expression *object;
		Identifier *identifier;
		FieldExpression (Expression *_object, Identifier *_identifier);
		virtual Type *analysis2 (ClassType *set, BlockSpace *ns, ExpressionList *argList);
		virtual void publish2 (FILE *f);
		virtual bool needPublish ();
};

class IdentifierExpression: public FieldExpression
{
	public:
		IdentifierExpression (Identifier *_identifier);
		virtual Type *analysis2 (ClassType *set, BlockSpace *ns, ExpressionList *argList);
};

class UnaryExpression: public Expression
{
	public:
		int op;
		Expression *expression;

		UnaryExpression (int _op, Expression *_expression);
		virtual void publish2 (FILE *f);
		virtual Type *analysis2 (ClassType *set, BlockSpace *ns, ExpressionList *argList);
};

class BinaryExpression: public Expression
{
	public:
		int op;
		Expression *left, *right;

		BinaryExpression (int _op, Expression *_left, Expression *_right);
		virtual void publish2 (FILE *f);
		virtual Type *analysis2 (ClassType *set, BlockSpace *ns, ExpressionList *argList);
};

class ConditionalExpression: public Expression
{
	public:
		Expression *condition, *expression0, *expression1;

		ConditionalExpression (Expression *, Expression *, Expression *);
		virtual void publish2 (FILE *f);
		virtual Type *analysis2 (ClassType *set, BlockSpace *ns, ExpressionList *argList);
};

class ConstructorCallExpression: public Expression
{
	public:
		ExpressionList *argList;
		CallExpression *constructorCall;

		ConstructorCallExpression (Type *_type, ExpressionList *_argList, ComponentType *_component, int _line);
		Type *analysis (BlockSpace *ns, PropertyDeclaration *pd);
//		Type *analysis (BlockSpace *ns, ComponentDeclaration *pd);
		void publish (FILE *f);
		void makeCall (PropertyDeclaration *pd, ConstructorDeclaration *constructor);
//		void makeCall (const char *name, ConstructorDeclaration *constructor);
};

class NewExpression: public ConstructorCallExpression
{
	public:
		NewExpression (Type *_type, ExpressionList *_argList, ComponentType *_component, int _line);
		virtual Type *analysis2 (ClassType *set, BlockSpace *ns, ExpressionList *argList);
		virtual void publish2 (FILE *f);
};

class LiteralExpression: public Expression
{
	public:
		const char *value;

		LiteralExpression (const char *_value, ComponentType *_component, int _line);
		virtual void publish2 (FILE *f);
};

class FloatExpression: public LiteralExpression
{
	public:
		FloatExpression (const char *_value, ComponentType *_component, int _line);
		virtual Type *analysis2 (ClassType *set, BlockSpace *ns, ExpressionList *argList);
};

class IntegerExpression: public LiteralExpression
{
	public:
		IntegerExpression (const char *_value, ComponentType *_component, int _line);
		virtual Type *analysis2 (ClassType *set, BlockSpace *ns, ExpressionList *argList);
};

class StringExpression: public LiteralExpression
{
	public:
		StringExpression (const char *_value, ComponentType *_component, int _line);
		virtual Type *analysis2 (ClassType *set, BlockSpace *ns, ExpressionList *argList);
};

class NullExpression: public LiteralExpression
{
	public:
		NullExpression (ComponentType *_component, int _line);
		virtual Type *analysis2 (ClassType *set, BlockSpace *ns, ExpressionList *argList);
};

class ThisExpression: public LiteralExpression
{
	public:
		ThisExpression (ComponentType *, int line);
		virtual Type *analysis2 (ClassType *set, BlockSpace *ns, ExpressionList *argList);
};

class CastExpression: public Expression
{
	public:
		Expression *expression;

		CastExpression (Type *_type, Expression *_expression);
		virtual void publish2 (FILE *f);
};

class PrimitiveCastExpression: public CastExpression
{
	public:
		PrimitiveCastExpression (Type *_type, Expression *_expression);
		virtual Type *analysis2 (ClassType *set, BlockSpace *ns, ExpressionList *argList);
};

class SetCastExpression: public CastExpression
{
	public:
		SetCastExpression (Type *_type, Expression *_expression);
		virtual Type *analysis2 (ClassType *set, BlockSpace *ns, ExpressionList *argList);
};

class ConstructorCall: public ConstructorCallExpression
{
		virtual PropertyDeclaration *findSet (ClassType *set, BlockSpace *ns) = 0;

	public:
		ConstructorCall (ExpressionList *_argList, ComponentType *_component, int _line);
		virtual Type *analysis2 (ClassType *set, BlockSpace *ns, ExpressionList *argList);
		virtual void publish2 (FILE *f);
		Declaration *analysis (ClassType *set, BlockSpace *ns);
};

class ConstructorCallImplicit: public ConstructorCall
{
		virtual PropertyDeclaration *findSet (ClassType *set, BlockSpace *ns);

	public:
		ConstructorCallImplicit (ComponentType *_component, int _line);
};

class ConstructorCallByID: public ConstructorCall
{
		virtual PropertyDeclaration *findSet (ClassType *set, BlockSpace *ns);

	public:
		Identifier *identifier;

		ConstructorCallByID (Identifier *_, ExpressionList *);
};

class ConstructorCallByQID: public ConstructorCall
{
		virtual PropertyDeclaration *findSet (ClassType *set, BlockSpace *ns);

	public:
		QualifiedIdentifier *qid;

		ConstructorCallByQID (QualifiedIdentifier *_qid, ExpressionList *_argList);
};

class ConstructorCallBySet: public ConstructorCall
{
		virtual PropertyDeclaration *findSet (ClassType *set, BlockSpace *ns);

	public:
		ClassType *set;

		ConstructorCallBySet (ClassType *_setType, ExpressionList *_argList, ComponentType *_component, int _line);
};

#endif

