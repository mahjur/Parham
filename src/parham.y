
/*
   1. type state
   	- transition diagram
	- bitwise or
   2. exception handling
   3. extern support (virtual and delegate)
   4. better subtyping
   5. fields can have internal type
   6. enumeration type: static set
   7. typedef


New List:
	OK 1. extern support (virtual and delegate)
	OK 2. fields can have internal type
	OK 3. enumeration type: prevalued set, value set
	OK 4. typedef: how?
	OK 5. type state
		5.1. transition diagram
		5.2. Set -> Set declaraton of function variables and return values
		5.3. Set -> Set statements change the state of a variable
	   6. exception handling
	OK 7. attribute initialization
	   8. side effects of the type state. That is, when we change the state of an object which is not passed as argument and not returned

	   8. declarations are done using decl keyword
	   9. we do not specify the set of a variable
*/

/*
   List 3.
   	1. Fields can have enumeration or typedef type.
	2. I do not support implicit state changes (the change of the state of none-argument objects)
	3. simplifying set declaration using decl keyword
	4. exception handling
   */

%{

#include "parham.hxx"

extern int yylex (void);
extern void yyerror(char const *s);

extern Identifier *lexIdentifier;
extern char *lexString;
extern ClassType *lexSet;
extern Type *lexType;

InterfaceType *mainSet = NULL;
%}

%token	TOK_ADD_ASSIGN
%token	TOK_AND
%token	TOK_AND_ASSIGN
%token	TOK_BOOL
%token	TOK_BREAK
%token	TOK_CASE
%token	TOK_CHAR
%token	TOK_CLASS
%token  TOK_CONST
%token	TOK_CONTINUE
%token	TOK_DEC
%token	TOK_DEFAULT
%token  TOK_DELETE
%token	TOK_DIV_ASSIGN
%token	TOK_DO
%token	TOK_DOUBLE
%token	TOK_ELLIPSIS
%token	TOK_ELSE
%token	TOK_EQ
%token	TOK_EXTERN
%token	TOK_FALSE
%token	TOK_FLOAT
%token	TOK_FOR
%token	TOK_GE
%token	TOK_GOTO
%token	TOK_IDENTIFIER
%token	TOK_IF
%token	TOK_INC
%token	TOK_INT
%token	TOK_LE
%token	TOK_LEFT_ASSIGN
%token	TOK_LEFT_SHIFT
%token	TOK_LIT_CHARACTER
%token	TOK_LIT_FLOAT
%token	TOK_LIT_INTEGER
%token	TOK_LIT_STRING
%token	TOK_LOCAL
%token	TOK_LONG
%token	TOK_MOD_ASSIGN
%token	TOK_MUL_ASSIGN
%token	TOK_NE
%token	TOK_NEW
%token	TOK_NULL
%token	TOK_OR
%token	TOK_OR_ASSIGN
%token	TOK_PROVIDES
%token	TOK_REQUIRES
%token	TOK_RETURN
%token	TOK_RIGHT_ARROW
%token	TOK_RIGHT_ASSIGN
%token	TOK_RIGHT_SHIFT
%token	TOK_SET_NAME
%token	TOK_SHORT
%token	TOK_SIGNED
%token	TOK_SUB_ASSIGN
%token	TOK_SWITCH
%token	TOK_THIS
%token	TOK_TRUE
%token	TOK_TYPE
%token	TOK_UNSIGNED
%token	TOK_VOID
%token	TOK_WHILE
%token	TOK_XOR_ASSIGN

%union
{
	int int_t;

	Array<ConstructorCall>		*constructorCall_list;
	Array<Delegate>			*delegate_list;
	Array<Designator>		*designator_list;
	Array<InclusionBodyItem>	*inclusionBodyItem_list;
	Array<InitializedIdentifier>	*initializedIdentifier_list;
	Array<Initializer>		*initializer_list;
	Array<IntegerExpression>	*integerExpression_list;
	Array<MapDeclaration>		*mapDeclaration_list;
	Array<InstanceDeclaration>	*instanceDeclaration_list;
	Array<PropertyDeclaration>	*propertyDeclaration_list;
	Array<ClassType>			*setType_list;
	Array<Statement>		*statement_list;

	UniqueArray<EnumerationItem>	*enumeration_ulist;
	UniqueArray<Identifier>		*identifier_ulist;
	UniqueArray<ClassType>		*setType_ulist;
	UniqueArray<StateInitializer>	*stateInitializer_ulist;

	ArgumentDeclaration		*argumentDeclaration_t;
	ArgumentDeclarationList		*argumentDeclarationList_t;
	AssociationDeclaration		*associationDeclaration_t;
	BlockStatement			*blockStatement_t;
	ConstructorCall			*constructorCall_t;
	ConstructorDeclaration		*constructorDeclaration_t;
	Delegate			*delegate_t;
	Designator			*designator_t;
	Disambiguation			*disambiguation_t;
	EnumerationItem			*enumerationItem_t;
	EnumerationType			*enumerationType_t;
	ExplicitDeclaration		*explicitDeclaration_t;
	Expression			*expression_t;
	ExpressionList			*expressionList_t;
	Identifier			*identifier_t;
	InclusionBody			*inclusionBody_t;
	InclusionBodyItem		*inclusioBodyItem_t;
	InclusionDeclaration		*inclusionDeclaration_t;
	InitializedIdentifier		*initializedIdentifier_t;
	Initializer			*initializer_t;
	InstanceDeclaration		*instanceDeclaration_t;
	IntegerExpression		*integerExpression_t;
	InterfaceType			*interfaceType_t;
	InternalType			*internalType_t;
	LiteralExpression		*literalExpression_t;
	MapDeclaration			*mapDeclaration_t;
	MethodDeclaration		*methodDeclaration_t;
	Modifier			*modifier_t;
	QualifiedIdentifier		*qualifiedIdentifier_t;
	ClassType				*setType_t;
	StateInitializer		*stateInitializer_t;
	Statement			*statement_t;
	SwitchItem			*switchItem_t;
	SwitchItemList			*switchItemList_t;
	TransitionDeclaration		*transitionDeclaration_t;
	TransitionState			*transitionState_t;
	Type				*type_t;
	TypedefDeclaration		*typedefDeclaration_t;
};

%type <expression_t>			AdditiveExpression
%type <expression_t>			AndExpression
%type <argumentDeclaration_t>		ArgumentDeclaration
%type <argumentDeclarationList_t>	ArgumentDeclarationList
%type <argumentDeclarationList_t>	ArgumentList
%type <type_t>				ArgumentType
%type <expression_t>			AssignmentExpression
%type <associationDeclaration_t>	AssociationDeclaration
%type <propertyDeclaration_list>	AttributeDeclaration
%type <inclusionBody_t>			InclusionBody
%type <int_t>				AssignmentOperator
%type <blockStatement_t>		BlockStatement
%type <literalExpression_t>		BooleanLiteral
%type <statement_t>			BreakStatement
%type <expression_t>			CastExpression
%type <expression_t>			ConditionalAndExpression
%type <expression_t>			ConditionalExpression
%type <expression_t>			ConditionalOrExpression
%type <constructorCall_t>		ConstructorCall
%type <constructorCall_list>		ConstructorCallList
%type <constructorDeclaration_t>	ConstructorDeclaration
%type <constructorDeclaration_t>	ConstructorPrototype
%type <statement_t>			ContinueStatement
%type <statement_list>			DeclarationStatement
%type <delegate_t>			Delegate
%type <delegate_list>			DelegateList
%type <designator_t>			Designator
%type <designator_list>			Designation
%type <designator_list>			DesignatorList
%type <integerExpression_list>		Dimension
%type <disambiguation_t>		Disambiguation
%type <statement_t>			DoStatement
%type <statement_t>			EmptyStatement
%type <enumerationItem_t>		EnumerationItem
%type <enumeration_ulist>		EnumerationList
%type <enumerationType_t>		EnumerationType
%type <expression_t>			EqualityExpression
%type <expression_t>			ExclusiveOrExpression
%type <expression_t>			Expression
%type <expression_t>			ExpressionOrNull
%type <statement_t>			ExpressionStatement
%type <expressionList_t>		ExpressionList
%type <statement_t>			ForStatement
%type <statement_t>			GotoStatement
%type <identifier_t>			Identifier
%type <identifier_ulist>		IdentifierList
%type <statement_t>			IfStatement
%type <inclusioBodyItem_t>		InclusionBodyItem
%type <inclusionBodyItem_list>		InclusionBodyList
%type <inclusionDeclaration_t>		InclusionDeclaration
%type <setType_ulist>			InclusionList
%type <setType_ulist>			InclusionList2
%type <expression_t>			InclusiveOrExpression
%type <initializedIdentifier_t>		InitializedIdentifier
%type <initializedIdentifier_list>	InitializedIdentifierList
%type <initializer_t>			Initializer
%type <initializer_list>		InitializerList
%type <instanceDeclaration_t>		InstanceDeclaration
%type <integerExpression_t>		IntegerLiteral
%type <interfaceType_t>			InterfaceType
%type <statement_t>			LabeledStatement
%type <literalExpression_t>		Literal
%type <mapDeclaration_t>		MapDeclaration
%type <mapDeclaration_list>		MapDeclarationList
%type <mapDeclaration_t>		MapDetail
%type <methodDeclaration_t>		MethodDeclaration
%type <methodDeclaration_t>		MethodPrototype
%type <int_t>				Modifier
%type <modifier_t>			ModifierList
%type <identifier_t>			ComponentName
%type <expression_t>			MultiplicativeExpression
%type <type_t>				NonVoidType
%type <expressionList_t>		ParameterList
%type <expression_t>			PostfixExpression
%type <expression_t>			PrimaryExpression
%type <type_t>				PrimitiveType
%type <propertyDeclaration_list>	PropertyDeclaration
%type <propertyDeclaration_list>	PropertyDeclarationList
%type <identifier_t>			QIPart
%type <qualifiedIdentifier_t>		QualifiedIdentifier
%type <expression_t>			RelationalExpression
%type <statement_t>			ReturnStatement
%type <explicitDeclaration_t>		RoleDeclaration
%type <setType_t>			SetIdentifier
%type <setType_list>			SetIdentifierList
%type <instanceDeclaration_t>		SimpleInstanceDeclaration

//%type <propertyDeclaration_list>	SingleAttributeDeclaration
%type <methodDeclaration_t>		SingleMethodDeclaration
%type <methodDeclaration_t>		SingleMethodPrototype
%type <explicitDeclaration_t>		SinglePropertyDeclaration

%type <inclusioBodyItem_t>		StateAttributeDeclaration
%type <expression_t>			ShiftExpression
%type <stateInitializer_t>		StateInitializer
%type <stateInitializer_ulist>		StateInitializerList
%type <statement_t>			Statement
%type <statement_list>			StatementList
%type <switchItemList_t>		SwitchItemList
%type <switchItem_t>			SwitchLabel
%type <statement_t>			SwitchStatement
%type <transitionDeclaration_t>		Transition
%type <transitionState_t>		TransitionState
%type <type_t>				TransitionType
%type <typedefDeclaration_t>		TypedefDeclaration
%type <type_t>				TypeName
%type <expression_t>			UnaryExpression
%type <setType_ulist>			UnificationList
%type <statement_t>			WhileStatement

%start ComponentDeclaration

%%

/*
   Overall structure
*/

ComponentDeclaration
	: ComponentPrototype ComponentBody
	;

ComponentPrototype
	: ComponentName
	{
//		fprintf (stderr, "%s[%d]. ComponentPrototype -> ComponentName\n", __FILE__, __LINE__);
		curComponent->addInterface (mainSet = new InterfaceType ($1, new UniqueArray<Identifier>));
		mainSet->mainSet = true;
	}
	| ComponentName '[' InterfaceList ']'
	;

ComponentName
	: Identifier
	{
//		fprintf (stderr, "%s[%d]. ComponentName -> Identifier\n", __FILE__, __LINE__);
		curComponent->setName (new QualifiedIdentifier ($1));
		$$ = $1;
	}
	| QualifiedIdentifier
	{
		curComponent->setName ($1);
		$$ = NULL;
	}
	;

InterfaceList
	: InterfaceType
	{
		curComponent->addInterface ($1);
	}
	| InterfaceList ',' InterfaceType
	{
		curComponent->addInterface ($3);
	}
	;

InterfaceType
	: Identifier
	{
		$$ = new InterfaceType ($1, new UniqueArray<Identifier>);
	}
	| Identifier '<' IdentifierList '>'		/* state export */
	{
		/*
		   - DONE. the identifier list should not contain repeative names
		   TODO
		   - they should appear in an inclusion relation later
		     I can assume that it is an inclusion declaration itself
		     --MHJ
		   */
		$$ = new InterfaceType ($1, $3);
	}
	| ModifierList Identifier
	{
		$$ = new InterfaceType ($1, $2, new UniqueArray<Identifier>);
	}
	| ModifierList Identifier '<' IdentifierList '>'		/* state export */
	{
		/*
		   - DONE. the identifier list should not contain repeative names
		   TODO
		   - they should appear in an inclusion relation later
		     I can assume that it is an inclusion declaration itself
		     --MHJ
		   */
		$$ = new InterfaceType ($1, $2, $4);
	}
	;

ComponentBody
	: 
	| ComponentItemList
	;

ComponentItemList
	: ComponentItem
	| ComponentItemList ComponentItem
	;

ComponentItem
	: EnumerationType
	{
		curComponent->addType ($1);
	}
	| TypedefDeclaration ';'
	{
		curComponent->addType ($1);
	}
	| ConstDeclaration
	| AssociationDeclaration ';'
	{
		curComponent->addItem ($1);
	}
	| AssociationDeclaration IdentifierList ';'
	{
		AssociationDeclaration *ad;
		int i;

		$1->setIdentifier ($2->array.array[0]);
		curComponent->addItem ($1);
		for (i = 1; i < $2->array.count; ++i)
		{
			ad = $1->clone ();
			ad->setIdentifier ($2->array.array[i]);
			curComponent->addItem (ad);
		}
	}
	| InclusionDeclaration
	{
		curComponent->addItem ($1);
	}
	| InstanceDeclaration
	{
		curComponent->addItem ($1);
	}
	| StateUnification ';'
	| RoleDeclaration
	{
		if ($1)
			curComponent->addItem ($1);
	}
	| MethodDeclaration
	{
		curComponent->addItem ($1);
	}
	;

EnumerationType
	: Identifier '=' '{' EnumerationList '}'
	{
		int i;

		$$ = new EnumerationType ($1, $4, curComponent, curComponent->line);
		for (i = 0; i < $4->array.count; ++i)
			$4->array.array[i]->setType ($$);
	}
	;

EnumerationList
	: EnumerationItem
	{
		$$ = new UniqueArray<EnumerationItem>;
		$$->add ($1);
		if (!curComponent->addAttribute ($1))
			$1->identifier->erDuplicate ();
	}
	| EnumerationList ',' EnumerationItem
	{
		$$ = $1;
		if (!$$->add ($3))
			$3->error ("duplicate enumeration item (%s)\n", $3->identifier->value);
		else if (!curComponent->addAttribute ($3))
			$3->identifier->erDuplicate ();
	}
	;

EnumerationItem
	: Identifier '=' IntegerLiteral
	{
		/*
		   TODO
		   	1. Having value for an enumeration item is optional
			2. If it does not have a value, the list is integer. In this case, it has the next value of the list.
		   */
		$$ = new EnumerationItem ($1, $3);
		curComponent->addItem ($$);
	}
	| Identifier
	{
		$$ = new EnumerationItem ($1, NULL);
		curComponent->addItem ($$);
	}
	;

TypedefDeclaration
	: Identifier '=' PrimitiveType
	{
		$$ = new TypedefDeclaration ($1, $3);
	}
	;

ConstDeclaration
	: Identifier '=' Literal
	;

AssociationDeclaration
	:              SetIdentifier TOK_RIGHT_ARROW SetIdentifier
	{
		$$ = new AssociationDeclaration ($1, new Modifier (curComponent, curComponent->line), $3);
	}
	|                 Identifier TOK_RIGHT_ARROW SetIdentifier
	{
		InternalType *intern;

		intern = curComponent->rm.flMakeType ($1);
		$$ = new AssociationDeclaration (intern, new Modifier (curComponent, curComponent->line), $3);
	}
	|              SetIdentifier TOK_RIGHT_ARROW SetIdentifier Dimension
	{
		$$ = new AssociationDeclaration ($1, new Modifier (curComponent, curComponent->line), ArrayType::make ($3, $4));
	}
	|                 Identifier TOK_RIGHT_ARROW SetIdentifier Dimension
	{
		InternalType *intern;

		intern = curComponent->rm.flMakeType ($1);
		$$ = new AssociationDeclaration (intern, new Modifier (curComponent, curComponent->line), ArrayType::make ($3, $4));
	}
	| ModifierList SetIdentifier TOK_RIGHT_ARROW SetIdentifier
	{
		$$ = new AssociationDeclaration ($2, $1, $4);
	}
	| ModifierList    Identifier TOK_RIGHT_ARROW SetIdentifier
	{
		InternalType *intern;

		intern = curComponent->rm.flMakeType ($2);
		$$ = new AssociationDeclaration (intern, $1, $4);
	}
	| ModifierList SetIdentifier TOK_RIGHT_ARROW SetIdentifier Dimension
	{
		$$ = new AssociationDeclaration ($2, $1, ArrayType::make ($4, $5));
	}
	| ModifierList    Identifier TOK_RIGHT_ARROW SetIdentifier Dimension
	{
		InternalType *intern;

		intern = curComponent->rm.flMakeType ($2);
		$$ = new AssociationDeclaration (intern, $1, ArrayType::make ($4, $5));
	}
	;

InclusionDeclaration
	:           SetIdentifier '=' InclusionList ';'
	{
		$$ = new InclusionDeclaration ($1, $3, NULL, NULL);
	}
	|           SetIdentifier '=' InclusionList '{' InclusionBody '}'
	{
		$$ = new InclusionDeclaration ($1, $3, $5->body, $5->disambiguation);
	}
	;

InclusionList
	: InclusionList2
	{
		$$ = $1;
	}
	| InclusionList2 '|' TOK_ELLIPSIS
	{
		$$ = $1;
		$$->addNoCheck (NULL);
	}
	;

InclusionList2
	: SetIdentifier
	{
		$$ = new UniqueArray<ClassType>;
		$$->add ($1);
	}
	| InclusionList2 '|' SetIdentifier
	{
		$$ = $1;
		$$->add ($3);
	}
	|    Identifier
	{
		InternalType *intern;

		intern = curComponent->rm.flMakeType ($1);
		$$ = new UniqueArray<ClassType>;
		$$->add (intern);
	}
	| InclusionList2 '|'    Identifier
	{
		InternalType *intern;

		intern = curComponent->rm.flMakeType ($3);
		$$ = $1;
		$$->add (intern);
	}
	;

/*
   Currently, an inclusion body has two parts
   	transition declarations
	disambiguation statements
   	specific field declaration (state clarification)
   We should add the following parts too
	specific constructors (initializer)
   */
InclusionBody
	: InclusionBodyList
	{
		$$ = new InclusionBody ($1, NULL, curComponent, curComponent->line);
	}
	|                   Disambiguation
	{
		$$ = new InclusionBody (NULL, $1, curComponent, curComponent->line);
	}
	| InclusionBodyList Disambiguation
	{
		$$ = new InclusionBody ($1, $2, curComponent, curComponent->line);
	}
	;

InclusionBodyList
	: InclusionBodyItem
	{
		$$ = new Array<InclusionBodyItem>;
		$$->add ($1);
	}
	| InclusionBodyList InclusionBodyItem
	{
		$$ = $1;
		$$->add ($2);
	}
	;

InclusionBodyItem
	: Transition
	{
		$$ = $1;
	}
	| StateAttributeDeclaration
	{
		$$ = $1;
	}
	;

StateAttributeDeclaration
	: PrimitiveType Identifier '=' '{' StateInitializerList '}'
	{
		$$ = new StateAttributeDeclaration ($1, $2, $5);
	}
	;

StateInitializerList
	: StateInitializer
	{
		$$ = new UniqueArray<StateInitializer>;
		$$->add ($1);
	}
	| StateInitializerList StateInitializer
	{
		$$ = $1;
		if (!$$->add ($2))
			$2->error ("duplicate state initializer (%s)\n", $2->set->identifier->value);
	}
	;

StateInitializer
	: SetIdentifier ':' UnaryExpression
	{
		$$ = new StateInitializer ($1, $3);
	}
	;

Transition
	: TransitionState TOK_RIGHT_ARROW TransitionState
	{
		/*
		   TODO
		   	1. transiting a state to itself is meaningless.
		   */
		$$ = new TransitionDeclaration ($1);
		$$->add ($3);
	}
	| Transition TOK_RIGHT_ARROW TransitionState
	{
		$$ = $1;
		$$->add ($3);
	}
	;

TransitionState
	: '.'
	{
		$$ = new TransitionState (NULL, curComponent, curComponent->line);
	}
	| SetIdentifier
	{
		$$ = new TransitionState ($1, curComponent, curComponent->line);
	}
	;

Disambiguation
	: BlockStatement
	{
		$$ = new Disambiguation ($1, curComponent, curComponent->line);
	}
	;

InstanceDeclaration
	: SimpleInstanceDeclaration ';'
	{
		$$ = $1;
	}
	| SimpleInstanceDeclaration Identifier ';'
	{
		$$ = $1;
		$1->setIdentifier ($2);
	}
	| SimpleInstanceDeclaration '{' DelegateList '}'
	{
		$1->setDelegate ($3);
		$$ = $1;
	}
	| SimpleInstanceDeclaration Identifier '{' DelegateList '}'
	{
		$1->setDelegate ($4);
		$1->setIdentifier ($2);
		$$ = $1;
	}
	;

	/*
	   It seems we should remove delegate model
	   */
DelegateList
	: Delegate
	{
		$$ = new Array<Delegate>;
		$$->add ($1);
	}
	| DelegateList Delegate
	{
		$$ = $1;
		$$->add ($2);
	}
	;

Delegate
	: Identifier '.' Identifier '=' Identifier ';'
	{
		$$ = new Delegate ($1, $3, $5);
	}
	| Identifier '.' Identifier '=' Identifier '.' Identifier '.' Identifier ';'
	{
		$$ = new Delegate ($1, $3, $5, $7, $9);
	}
	;

SimpleInstanceDeclaration
	:                       Identifier '[' MapDeclarationList ']'
	{
		$$ = new InstanceDeclaration (new Modifier (curComponent, curComponent->line), new QualifiedIdentifier ($1), $3, NULL, curComponent, curComponent->line);
	}
	|                       Identifier '[' MapDeclarationList ']' Dimension
	{
		$$ = new InstanceDeclaration (new Modifier (curComponent, curComponent->line), new QualifiedIdentifier ($1), $3, $5, curComponent, curComponent->line);
	}
	| ModifierList          Identifier '[' MapDeclarationList ']'
	{
		$$ = new InstanceDeclaration ($1, new QualifiedIdentifier ($2), $4, NULL, curComponent, curComponent->line);
	}
	| ModifierList          Identifier '[' MapDeclarationList ']' Dimension
	{
		$$ = new InstanceDeclaration ($1, new QualifiedIdentifier ($2), $4, $6, curComponent, curComponent->line);
	}
	|              QualifiedIdentifier '[' MapDeclarationList ']'
	{
		$$ = new InstanceDeclaration (new Modifier (curComponent, curComponent->line), $1, $3, NULL, curComponent, curComponent->line);
	}
	|              QualifiedIdentifier '[' MapDeclarationList ']' Dimension
	{
		$$ = new InstanceDeclaration (new Modifier (curComponent, curComponent->line), $1, $3, $5, curComponent, curComponent->line);
	}
	| ModifierList QualifiedIdentifier '[' MapDeclarationList ']'
	{
		$$ = new InstanceDeclaration ($1, $2, $4, NULL, curComponent, curComponent->line);
	}
	| ModifierList QualifiedIdentifier '[' MapDeclarationList ']' Dimension
	{
		$$ = new InstanceDeclaration ($1, $2, $4, $6, curComponent, curComponent->line);
	}
	;

StateUnification
	: SetIdentifier '=' UnificationList
	{
		$1->addUnification (new UnificationList ($3, curComponent, curComponent->line));
	}
	|    Identifier '=' UnificationList
	{
		InternalType *intern;

		intern = curComponent->rm.flMakeType ($1);
		intern->addUnification (new UnificationList ($3, curComponent, curComponent->line));
	}
	;

UnificationList
	: SetIdentifier '&' SetIdentifier
	{
		$$ = new UniqueArray<ClassType>;
		$$->add ($1);
		$$->add ($3);
	}
	| UnificationList '&' SetIdentifier
	{
		$$ = $1;
		$$->add ($3);
	}
	;

/*
   Instance Declaration
   */

MapDeclarationList
	: MapDeclaration
	{
		$$ = new Array<MapDeclaration>;
		$$->add ($1);
	}
	| MapDeclarationList ',' MapDeclaration
	{
		$$ = $1;
		$$->add ($3);
	}
	;

MapDeclaration
	:              MapDetail
	{
		$$ = $1;
		$$->setModifier (new Modifier (curComponent, curComponent->line));
	}
	|              MapDetail Identifier
	{
		$$ = $1;
		$$->setIdentifier ($2);
		$$->setModifier (new Modifier (curComponent, curComponent->line));
	}
	| ModifierList MapDetail
	{
		$$ = $2;
		$$->setModifier ($1);
	}
	| ModifierList MapDetail Identifier
	{
		$$ = $2;
		$$->setIdentifier ($3);
		$$->setModifier ($1);
	}
	| Identifier
	{
		InternalType *intern;

		intern = curComponent->rm.flMakeType ($1);
		$$ = new MapDeclaration (intern, new Array<ClassType>, curComponent, curComponent->line);
		$$->setModifier (new Modifier (curComponent, curComponent->line));
	}
	| ModifierList Identifier
	{
		InternalType *intern;

		intern = curComponent->rm.flMakeType ($2);
		$$ = new MapDeclaration (intern, new Array<ClassType>, curComponent, curComponent->line);
		$$->setModifier ($1);
	}
	;

MapDetail
	: SetIdentifier
	{
		$$ = new MapDeclaration ($1, new Array<ClassType>, curComponent, curComponent->line);
	}
	| SetIdentifier '<' SetIdentifierList '>'
	{
		$$ = new MapDeclaration ($1, $3, curComponent, curComponent->line);
	}
	;

SetIdentifierList
	: SetIdentifier
	{
		$$ = new Array<ClassType>;
		$$->add ($1);
	}
	| SetIdentifierList ',' SetIdentifier
	{
		$$ = $1;
		$$->add ($3);
	}
	;

/*
   Role Declaration
   */

RoleDeclaration
	: SetIdentifier '{' '}'
	{
		$$ = NULL;
	}
	| SetIdentifier '{' PropertyDeclarationList '}'
	{
		$$ = new ExplicitDeclaration ($1, $3, curComponent, curComponent->line);
	}
	|    Identifier '{' PropertyDeclarationList '}'
	{
		InternalType *intern;

		intern = curComponent->rm.flMakeType ($1);
		$$ = new ExplicitDeclaration (intern, $3, curComponent, curComponent->line);
	}
	| SinglePropertyDeclaration
	{
//		TODO
//		$$ = new ExplicitDeclaration ($1, $1, curComponent, curComponent->line);
	}
	;

SinglePropertyDeclaration
/*
	: SingleAttributeDeclaration
	{
		$$ = $1;
	}
	*/
	: SingleMethodDeclaration
	{
		Array<PropertyDeclaration> *pdList;

		pdList = new Array<PropertyDeclaration>;
		pdList->add ($1);
		$$ = new ExplicitDeclaration ($1->container, pdList, curComponent, curComponent->line);
	}
	| ConstructorDeclaration
	{
		Array<PropertyDeclaration> *pdList;

		pdList = new Array<PropertyDeclaration>;
		pdList->add ($1);
		$$ = new ExplicitDeclaration ($1->container, pdList, curComponent, curComponent->line);
	}
	;

	/*
SingleAttributeDeclaration
	:              NonVoidType InitializedIdentifierList ';'
	{
		int i;

		$$ = new Array<PropertyDeclaration>;
		for (i = 0; i < $2->count; ++i)
			$$->add (new AttributeDeclaration (new Modifier (curComponent, curComponent->line), $1, $2->array[i], curComponent, curComponent->line));
	}
	| ModifierList NonVoidType InitializedIdentifierList ';'
	{
		int i;

		$$ = new Array<PropertyDeclaration>;
		for (i = 0; i < $3->count; ++i)
			$$->add (new AttributeDeclaration ($1, $2, $3->array[i], curComponent, curComponent->line));
	}
	;
	*/




SingleMethodDeclaration
	: SingleMethodPrototype ';'
	{
		$$ = $1;
	}
	| SingleMethodPrototype BlockStatement
	{
		$$ = $1;
		$$->setBody ($2);
	}
	;

SingleMethodPrototype
	:              NonVoidType SetIdentifier '.' Identifier ArgumentList
	{
		$$ = new MethodDeclaration (new Modifier (curComponent, curComponent->line), $1, $4, $5);
		$$->setContainer ($2);
	}
	|              TOK_VOID    SetIdentifier '.' Identifier ArgumentList
	{
		$$ = new MethodDeclaration (new Modifier (curComponent, curComponent->line), new VoidType (curComponent, curComponent->line), $4, $5);
		$$->setContainer ($2);
	}
	| ModifierList NonVoidType SetIdentifier '.' Identifier ArgumentList
	{
		$$ = new MethodDeclaration ($1, $2, $5, $6);
		$$->setContainer ($3);
	}
	| ModifierList TOK_VOID    SetIdentifier '.' Identifier ArgumentList
	{
		$$ = new MethodDeclaration ($1, new VoidType (curComponent, curComponent->line), $5, $6);
		$$->setContainer ($3);
	}
	|              NonVoidType    Identifier '.' Identifier ArgumentList
	{
		$$ = new MethodDeclaration (new Modifier (curComponent, curComponent->line), $1, $4, $5);
		$$->setContainer (curComponent->rm.flMakeType ($2));
	}
	|              TOK_VOID       Identifier '.' Identifier ArgumentList
	{
		$$ = new MethodDeclaration (new Modifier (curComponent, curComponent->line), new VoidType (curComponent, curComponent->line), $4, $5);
		$$->setContainer (curComponent->rm.flMakeType ($2));
	}
	| ModifierList NonVoidType    Identifier '.' Identifier ArgumentList
	{
		$$ = new MethodDeclaration ($1, $2, $5, $6);
		$$->setContainer (curComponent->rm.flMakeType ($3));
	}
	| ModifierList TOK_VOID       Identifier '.' Identifier ArgumentList
	{
		$$ = new MethodDeclaration ($1, new VoidType (curComponent, curComponent->line), $5, $6);
		$$->setContainer (curComponent->rm.flMakeType ($3));
	}
	;








PropertyDeclarationList
	: PropertyDeclaration
	{
		$$ = $1;
	}
	| PropertyDeclarationList PropertyDeclaration
	{
		$$ = $1;
		$$->append ($2);
	}
	;

PropertyDeclaration
	: AttributeDeclaration
	{
		$$ = $1;
	}
	| MethodDeclaration
	{
		$$ = new Array<PropertyDeclaration>;
		$$->add ($1);
	}
	| ConstructorDeclaration
	{
		$$ = new Array<PropertyDeclaration>;
		$$->add ($1);
	}
	;

AttributeDeclaration
	:              NonVoidType InitializedIdentifierList ';'
	{
		int i;
		AttributeDeclaration *attrib;

		$$ = new Array<PropertyDeclaration>;
		for (i = 0; i < $2->count; ++i)
		{
			$$->add (attrib = new AttributeDeclaration (new Modifier (curComponent, curComponent->line), $1, $2->array[i], curComponent, curComponent->line));
			curComponent->rm.flAdd (attrib);
		}
	}
	| ModifierList NonVoidType InitializedIdentifierList ';'
	{
		int i;
		AttributeDeclaration *attrib;

		$$ = new Array<PropertyDeclaration>;
		for (i = 0; i < $3->count; ++i)
		{
			$$->add (attrib = new AttributeDeclaration ($1, $2, $3->array[i], curComponent, curComponent->line));
			curComponent->rm.flAdd (attrib);
		}
	}
	| Identifier ';'
	{
		AttributeDeclaration *attrib;

		$$ = new Array<PropertyDeclaration>;
		$$->add (attrib = new AttributeDeclaration (new Modifier (curComponent, curComponent->line), /*Type*/ NULL, new InitializedIdentifier ($1, NULL), curComponent, curComponent->line));
		curComponent->rm.flAdd (attrib);
	}
	;

InitializedIdentifierList
	: InitializedIdentifier
	{
		$$ = new Array<InitializedIdentifier>;
		$$->add ($1);
	}
	| InitializedIdentifierList ',' InitializedIdentifier
	{
		$$ = $1;
		$$->add ($3);
	}
	;

InitializedIdentifier
	: Identifier
	{
		$$ = new InitializedIdentifier ($1, NULL);
	}
	| Identifier '=' Initializer
	{
		$$ = new InitializedIdentifier ($1, $3);
	}
	;

Initializer
	: UnaryExpression
	{
		$$ = new ExpressionInitializer ($1);
	}
	| '{' InitializerList '}'
	{
		$$ = new ArrayInitializer ($2, curComponent, curComponent->line);
	}
	| '{' InitializerList ',' '}'
	{
		$$ = new ArrayInitializer ($2, curComponent, curComponent->line);
		$2->add (NULL);
	}
	;

InitializerList
	:             Initializer
	{
		$$ = new Array<Initializer>;
		$$->add ($1);
	}
	| Designation Initializer
	{
		$$ = new Array<Initializer>;
		$$->add ($2);
		$2->setDesignation ($1);
	}
	| InitializerList ','             Initializer
	{
		$$ = $1;
		$$->add ($3);
	}
	| InitializerList ',' Designation Initializer
	{
		$$ = $1;
		$$->add ($4);
		$4->setDesignation ($3);
	}
	;

Designation
	: DesignatorList '=' 
	{
		$$ = $1;
	}
	;

DesignatorList
	: Designator
	{
		$$ = new Array<Designator>;
		$$->add ($1);
	}
	| DesignatorList Designator
	{
		$$ = $1;
		$$->add ($2);
	}
	;

Designator
	: '[' ConditionalExpression ']'
	{
		$$ = new ExpressionDesignator ($2);
	}
	| '.' Identifier
	{
		$$ = new IdentifierDesignator ($2);
	}
	;

MethodDeclaration
	: MethodPrototype ';'
	{
		$$ = $1;
	}
	| MethodPrototype BlockStatement
	{
		$$ = $1;
		$$->setBody ($2);
	}
	;

MethodPrototype
	:              NonVoidType Identifier ArgumentList
	{
		$$ = new MethodDeclaration (new Modifier (curComponent, curComponent->line), $1, $2, $3);
	}
	|              TOK_VOID    Identifier ArgumentList
	{
		$$ = new MethodDeclaration (new Modifier (curComponent, curComponent->line), new VoidType (curComponent, curComponent->line), $2, $3);
	}
	| ModifierList NonVoidType Identifier ArgumentList
	{
		$$ = new MethodDeclaration ($1, $2, $3, $4);
	}
	| ModifierList TOK_VOID    Identifier ArgumentList
	{
		$$ = new MethodDeclaration ($1, new VoidType (curComponent, curComponent->line), $3, $4);
	}
	;

ArgumentList
	: '(' ')'
	{
		$$ = new ArgumentDeclarationList (curComponent, curComponent->line);
	}
	| '(' TOK_ELLIPSIS ')'
	{
		$$ = new ArgumentDeclarationList (curComponent, curComponent->line);
		$$->add (new ArgumentDeclaration (NULL, NULL, curComponent, curComponent->line));
	}
	| '(' TOK_ELLIPSIS Identifier ')'
	{
		$$ = new ArgumentDeclarationList (curComponent, curComponent->line);
		$$->add (new ArgumentDeclaration (NULL, $3, curComponent, curComponent->line));
	}
	| '(' ArgumentDeclarationList ')'
	{
		$$ = $2;
	}
	| '(' ArgumentDeclarationList ',' TOK_ELLIPSIS ')'
	{
		$$ = $2;
		$$->add (new ArgumentDeclaration (NULL, NULL, curComponent, curComponent->line));
	}
	| '(' ArgumentDeclarationList ',' TOK_ELLIPSIS Identifier ')'
	{
		$$ = $2;
		$$->add (new ArgumentDeclaration (NULL, $5, curComponent, curComponent->line));
	}
	;

ArgumentDeclarationList
	: ArgumentDeclaration
	{
		$$ = new ArgumentDeclarationList (curComponent, curComponent->line);
		$$->add ($1);
	}
	| ArgumentDeclarationList ',' ArgumentDeclaration
	{
		$$ = $1;
		$$->add ($3);
	}
	;

ArgumentDeclaration
	: ArgumentType
	{
		$$ = NULL;
		if ($1->which == Type::TYPE_CLASS)
		{
			ClassType *ct;

			ct = (ClassType *)$1;
			if (curComponent->rm.flLookup (ct->identifier))
				$$ = new ArgumentDeclaration ($1, ct->identifier, curComponent, curComponent->line);
		}
		if (!$$)
			$$ = new ArgumentDeclaration ($1, NULL, curComponent, curComponent->line);
	}
	| ArgumentType Identifier
	{
		$$ = new ArgumentDeclaration ($1, $2, curComponent, curComponent->line);
	}
	;

ConstructorDeclaration
	: ConstructorPrototype ';'
	{
		$$ = $1;
	}
	| ConstructorPrototype BlockStatement
	{
		$$ = $1;
		$$->setBody ($2);
	}
	| ConstructorPrototype ':' ENTER_BlockStatement ConstructorCallList EXIT_BlockStatement BlockStatement
	{
		$$ = $1;
		$$->setBody ($6);
		$$->setCallList ($4);
	}
	;

ConstructorPrototype
	: SetIdentifier ArgumentList
	{
		$$ = new ConstructorDeclaration ($1, $2);
//		$$->setContainer ($1);
	}
	|    Identifier ArgumentList
	{
		InternalType *intern;

		intern = curComponent->rm.flMakeType ($1);
		$$ = new ConstructorDeclaration (intern, $2);
	}
	;

ConstructorCallList
	: ConstructorCall
	{
		$$ = new Array<ConstructorCall>;
		$$->add ($1);
	}
	| ConstructorCallList ',' ConstructorCall
	{
		$$ = $1;
		$$->add ($3);
	}
	;

/*
   We have three different ways to call a contructor.
   	1. ID. In this case, the set whose constructor is called is identified by an ID. The ID is the name of a map declaration.
	2. QID. In this case, the set whose constructor is called is identified by a QID. The QID is the name of a module followed by its set interface name.
		In this case, it is a 2-part identifier. The first is the module name (if we have just one instance of the module) or the instance name.
		and the second part is the set interface name.
	3. SuperSet. In this case, the name of the superset is given. This is used for calling superset constructor.
   */
ConstructorCall
	: Identifier ParameterList
	{
		$$ = new ConstructorCallByID ($1, $2);
	}
	| QualifiedIdentifier ParameterList
	{
		$$ = new ConstructorCallByQID ($1, $2);
	}
	| SetIdentifier ParameterList
	{
		$$ = new ConstructorCallBySet ($1, $2, curComponent, curComponent->line);
	}
	;

ParameterList
	: '(' ')'
	{
		$$ = new ExpressionList (curComponent, curComponent->line);
	}
	| '(' ExpressionList ')'
	{
		$$ = $2;
	}
	;

ExpressionList
	: Expression
	{
		$$ = new ExpressionList (curComponent, curComponent->line);
		$$->add ($1);
	}
	| ExpressionList ',' Expression
	{
		$$ = $1;
		$$->add ($3);
	}
	;

/*
   Declaration
   */

ModifierList
	: Modifier
	{
		$$ = new Modifier (curComponent, curComponent->line);
		$$->add ($1);
	}
	| ModifierList Modifier
	{
		$$ = $1;
		$$->add ($2);
	}
	;

Modifier
	: TOK_PROVIDES
	{
		$$ = MOD_PROVIDES;
	}
	| TOK_REQUIRES
	{
		$$ = MOD_REQUIRES;
	}
	| TOK_LOCAL
	{
		$$ = MOD_LOCAL;
	}
	| TOK_EXTERN
	{
		$$ = MOD_EXTERN;
	}
	| TOK_CONST
	{
		$$ = MOD_CONST;
	}
	;

Dimension
	: '[' ']'
	{
//		$$ = new Dimension (curComponent, curComponent->line);
		$$ = new Array<IntegerExpression>;
		$$->add (NULL);
	}
	| '[' IntegerLiteral ']'
	{
//		$$ = new Dimension (curComponent, curComponent->line);
		$$ = new Array<IntegerExpression>;
		$$->add ($2);
	}
	| Dimension '[' ']'
	{
		$$ = $1;
		$$->add (NULL);
	}
	| Dimension '[' IntegerLiteral ']'
	{
		$$ = $1;
		$$->add ($3);
	}
	;

/*
   Type
   */

PrimitiveType
	: TOK_BOOL
	{
		$$ = new IntegerType ("unsigned char /*bool*/", curComponent, curComponent->line);
	}
	| TOK_CHAR
	{
		$$ = new IntegerType ("char", curComponent, curComponent->line);
	}
	| TOK_SIGNED TOK_CHAR
	{
		$$ = new IntegerType ("signed char", curComponent, curComponent->line);
	}
	| TOK_UNSIGNED TOK_CHAR
	{
		$$ = new IntegerType ("unsigned char", curComponent, curComponent->line);
	}
	| TOK_SHORT
	{
		$$ = new IntegerType ("short", curComponent, curComponent->line);
	}
	| TOK_SIGNED TOK_SHORT
	{
		$$ = new IntegerType ("signed short", curComponent, curComponent->line);
	}
	| TOK_SHORT TOK_INT
	{
		$$ = new IntegerType ("short int", curComponent, curComponent->line);
	}
	| TOK_SIGNED TOK_SHORT TOK_INT
	{
		$$ = new IntegerType ("signed short int", curComponent, curComponent->line);
	}
	| TOK_UNSIGNED TOK_SHORT
	{
		$$ = new IntegerType ("unsigned short", curComponent, curComponent->line);
	}
	| TOK_UNSIGNED TOK_SHORT TOK_INT
	{
		$$ = new IntegerType ("unsigned short int", curComponent, curComponent->line);
	}
	| TOK_INT
	{
		$$ = new IntegerType ("int", curComponent, curComponent->line);
	}
	| TOK_SIGNED
	{
		$$ = new IntegerType ("signed", curComponent, curComponent->line);
	}
	| TOK_SIGNED TOK_INT
	{
		$$ = new IntegerType ("signed int", curComponent, curComponent->line);
	}
	| TOK_UNSIGNED
	{
		$$ = new IntegerType ("unsigned", curComponent, curComponent->line);
	}
	| TOK_UNSIGNED TOK_INT
	{
		$$ = new IntegerType ("unsigned int", curComponent, curComponent->line);
	}
	| TOK_LONG
	{
		$$ = new IntegerType ("long", curComponent, curComponent->line);
	}
	| TOK_SIGNED TOK_LONG
	{
		$$ = new IntegerType ("signed long", curComponent, curComponent->line);
	}
	| TOK_LONG TOK_INT
	{
		$$ = new IntegerType ("long int", curComponent, curComponent->line);
	}
	| TOK_SIGNED TOK_LONG TOK_INT
	{
		$$ = new IntegerType ("signed long int", curComponent, curComponent->line);
	}
	| TOK_UNSIGNED TOK_LONG
	{
		$$ = new IntegerType ("unsigned long", curComponent, curComponent->line);
	}
	| TOK_UNSIGNED TOK_LONG TOK_INT
	{
		$$ = new IntegerType ("unsigned long int", curComponent, curComponent->line);
	}
	| TOK_LONG TOK_LONG
	{
		$$ = new IntegerType ("long long", curComponent, curComponent->line);
	}
	| TOK_SIGNED TOK_LONG TOK_LONG
	{
		$$ = new IntegerType ("signed long long", curComponent, curComponent->line);
	}
	| TOK_LONG TOK_LONG TOK_INT
	{
		$$ = new IntegerType ("long long int", curComponent, curComponent->line);
	}
	| TOK_SIGNED TOK_LONG TOK_LONG TOK_INT
	{
		$$ = new IntegerType ("signed long long int", curComponent, curComponent->line);
	}
	| TOK_UNSIGNED TOK_LONG TOK_LONG
	{
		$$ = new IntegerType ("unsigned long long", curComponent, curComponent->line);
	}
	| TOK_UNSIGNED TOK_LONG TOK_LONG TOK_INT
	{
		$$ = new IntegerType ("unsigned long long int", curComponent, curComponent->line);
	}
	| TOK_FLOAT
	{
		$$ = new FloatType ("float", curComponent, curComponent->line);
	}
	| TOK_DOUBLE
	{
		$$ = new FloatType ("double", curComponent, curComponent->line);
	}
	| TOK_LONG TOK_DOUBLE
	{
		$$ = new FloatType ("long double", curComponent, curComponent->line);
	}
	| TypeName
	{
		$$ = $1;
	}
	;


NonVoidType
	: PrimitiveType
	{
		$$ = $1;
	}
	| SetIdentifier
	{
		$$ = $1;
	}
	| PrimitiveType Dimension
	{
		$$ = ArrayType::make ($1, $2);
	}
	| SetIdentifier Dimension
	{
		$$ = ArrayType::make ($1, $2);
	}
	;

ArgumentType
	: NonVoidType
	{
		$$ = $1;
	}
	| TransitionType
	{
		$$ = $1;
	}
	;

TransitionType
	: SetIdentifier TOK_RIGHT_ARROW SetIdentifier
	{
		$$ = new TransitionType ($1, $3);
	}

/*
   Statement
   */

Statement
	: BlockStatement
	{
		$$ = $1;
	}
	| BreakStatement
	{
		$$ = $1;
	}
	| ContinueStatement
	{
		$$ = $1;
	}
	| DoStatement
	{
		$$ = $1;
	}
	| EmptyStatement
	{
		$$ = $1;
	}
	| ExpressionStatement
	{
		$$ = $1;
	}
	| ForStatement
	{
		$$ = $1;
	}
	| GotoStatement
	{
		$$ = $1;
	}
	| IfStatement
	{
		$$ = $1;
	}
	| LabeledStatement
	{
		$$ = $1;
	}
	| ReturnStatement
	{
		$$ = $1;
	}
	| SwitchStatement
	{
		$$ = $1;
	}
	| WhileStatement
	{
		$$ = $1;
	}
	;

BlockStatement
	: ENTER_BlockStatement '{' '}' EXIT_BlockStatement
	{
		$$ = new BlockStatement (new Array<Statement>, curComponent, curComponent->line);
	}
	| ENTER_BlockStatement '{' StatementList '}' EXIT_BlockStatement
	{
		$$ = new BlockStatement ($3, curComponent, curComponent->line);
	}
	;

ENTER_BlockStatement
	:
	{
		curComponent->enterBlock ();
	}
	;

EXIT_BlockStatement
	:
	{
		curComponent->leaveBlock ();
	}
	;

StatementList
	: Statement
	{
		$$ = new Array<Statement>;
		$$->add ($1);
	}
	| DeclarationStatement
	{
		$$ = $1;
	}
	| StatementList Statement
	{
		$$ = $1;
		$$->add ($2);
	}
	| StatementList DeclarationStatement
	{
		$$ = $1;
		$$->append ($2);
	}
	;

DeclarationStatement
	: ArgumentType InitializedIdentifierList ';'
	{
		int i;

		$$ = new Array<Statement>;
		for (i = 0; i < $2->count; ++i)
			$$->add (new DeclarationStatement (new Modifier (curComponent, curComponent->line), $1, $2->array[i]));
	}
	| ModifierList ArgumentType InitializedIdentifierList ';'
	{
		int i;

		$$ = new Array<Statement>;
		for (i = 0; i < $3->count; ++i)
			$$->add (new DeclarationStatement ($1, $2, $3->array[i]));
	}
	| Identifier InitializedIdentifierList ';'
	{
		Type *type;
		int i;

		$$ = new Array<Statement>;
		type = curComponent->rm.flLookup ($1);
		if (type)
		{
			for (i = 0; i < $2->count; ++i)
				$$->add (new DeclarationStatement (new Modifier (curComponent, curComponent->line), type, $2->array[i]));
		}
		else
			$1->error ("%s does not refer to a type");
	}
	| ModifierList Identifier InitializedIdentifierList ';'
	{
		Type *type;
		int i;

		$$ = new Array<Statement>;
		type = curComponent->rm.flLookup ($2);
		if (type)
		{
			for (i = 0; i < $3->count; ++i)
				$$->add (new DeclarationStatement ($1, type, $3->array[i]));
		}
		else
			$2->error ("%s does not refer to a type");
	}
	;

BreakStatement
	: TOK_BREAK ';'
	{
		$$ = new BreakStatement (curComponent, curComponent->line);
	}
	;

ContinueStatement
	: TOK_CONTINUE ';'
	{
		$$ = new ContinueStatement (curComponent, curComponent->line);
	}
	;

DoStatement
	: TOK_DO Statement TOK_WHILE '(' ExpressionOrNull ')' ';'
	{
		$$ = new DoStatement ($2, $5);
	}
	;

EmptyStatement
	: ';'
	{
		$$ = new EmptyStatement (curComponent, curComponent->line);
	}
	;

ExpressionStatement
	: Expression ';'
	{
		$$ = new ExpressionStatement ($1);
	}
	;

ForStatement
	: TOK_FOR '(' ExpressionOrNull ';' ExpressionOrNull ';' ExpressionOrNull ')' Statement
	{
		$$ = new ForStatement ($3, $5, $7, $9);
	}
	;

GotoStatement
	: TOK_GOTO Identifier ';'
	{
		$$ = new GotoStatement ($2);
	}
	;

IfStatement
	: TOK_IF '(' Expression ')' Statement
	{
		$$ = new IfStatement ($3, $5, NULL);
	}
	| TOK_IF '(' Expression ')' Statement TOK_ELSE Statement
	{
		$$ = new IfStatement ($3, $5, $7);
	}
	;

LabeledStatement
	: Identifier ':' Statement
	{
		$$ = new LabeledStatement ($1, $3);
	}
	;

ReturnStatement
	: TOK_RETURN ';'
	{
		$$ = new ReturnStatement (NULL, curComponent, curComponent->line);
	}
	| TOK_RETURN Expression ';'
	{
		$$ = new ReturnStatement ($2, curComponent, curComponent->line);
	}
	;

SwitchStatement
	: TOK_SWITCH '(' Expression ')' '{' SwitchItemList '}'
	{
		$$ = new SwitchStatement ($3, $6);
	}
	;

SwitchItemList
	: SwitchLabel
	{
		$$ = new SwitchItemList (curComponent, curComponent->line);
		$$->add ($1);
	}
	| SwitchItemList SwitchLabel
	{
		$$ = $1;
		$$->add ($2);
	}
	| SwitchItemList Statement
	{
		$$ = $1;
		$$->add ($2);
	}
	;

SwitchLabel
	: TOK_CASE IntegerLiteral ':'
	{
		$$ = new SwitchIntegerLabel ($2);
	}
	| TOK_CASE Identifier ':'
	{
		$2->unimplemented ("switch cases with identifier has not implemented");
		$$ = NULL;
	}
	| TOK_DEFAULT ':'
	{
		$$ = new SwitchDefault (curComponent, curComponent->line);
	}
	;

WhileStatement
	: TOK_WHILE '(' Expression ')' Statement
	{
		$$ = new WhileStatement ($3, $5);
	}
	;

/*
   Expression
   */

ExpressionOrNull
	:
	{
		$$ = NULL;
	}
	| Expression
	{
		$$ = $1;
	}
	;

Expression
	: AssignmentExpression
	{
		$$ = $1;
	}
	;

AssignmentExpression
	: ConditionalExpression
	{
		$$ = $1;
	}
	| UnaryExpression AssignmentOperator AssignmentExpression
	{
		if ($2 == OP_ASSIGN)
			$$ = new AssignExpression ($1, $3);
		else
			$$ = new BinaryExpression ($2, $1, $3);
	}
	;

AssignmentOperator
	: '='
	{
		$$ = OP_ASSIGN;
	}
	| TOK_MUL_ASSIGN
	{
		$$ = OP_MUL_ASSIGN;
	}
	| TOK_DIV_ASSIGN
	{
		$$ = OP_DIV_ASSIGN;
	}
	| TOK_MOD_ASSIGN
	{
		$$ = OP_MOD_ASSIGN;
	}
	| TOK_ADD_ASSIGN
	{
		$$ = OP_ADD_ASSIGN;
	}
	| TOK_SUB_ASSIGN
	{
		$$ = OP_SUB_ASSIGN;
	}
	| TOK_LEFT_ASSIGN
	{
		$$ = OP_LEFT_ASSIGN;
	}
	| TOK_RIGHT_ASSIGN
	{
		$$ = OP_RIGHT_ASSIGN;
	}
	| TOK_AND_ASSIGN
	{
		$$ = OP_AND_ASSIGN;
	}
	| TOK_XOR_ASSIGN
	{
		$$ = OP_XOR_ASSIGN;
	}
	| TOK_OR_ASSIGN
	{
		$$ = OP_OR_ASSIGN;
	}
	;

ConditionalExpression
	: ConditionalOrExpression
	{
		$$ = $1;
	}
	| ConditionalOrExpression '?' Expression ':' ConditionalOrExpression
	{
		$$ = new ConditionalExpression ($1, $3, $5);
	}
	;

ConditionalOrExpression
	: ConditionalAndExpression
	{
		$$ = $1;
	}
	| ConditionalOrExpression TOK_OR ConditionalAndExpression
	{
		$$ = new BinaryExpression (OP_LOG_OR, $1, $3);
	}
	;

ConditionalAndExpression
	: InclusiveOrExpression
	{
		$$ = $1;
	}
	| ConditionalAndExpression TOK_AND InclusiveOrExpression
	{
		$$ = new BinaryExpression (OP_LOG_AND, $1, $3);
	}
	;

InclusiveOrExpression
	: ExclusiveOrExpression
	{
		$$ = $1;
	}
	| InclusiveOrExpression '|' ExclusiveOrExpression
	{
		$$ = new BinaryExpression (OP_BIT_OR, $1, $3);
	}
	;

ExclusiveOrExpression
	: AndExpression
	{
		$$ = $1;
	}
	| ExclusiveOrExpression '^' AndExpression
	{
		$$ = new BinaryExpression (OP_XOR, $1, $3);
	}
	;

AndExpression
	: EqualityExpression
	{
		$$ = $1;
	}
	| AndExpression '&' EqualityExpression
	{
		$$ = new BinaryExpression (OP_BIT_AND, $1, $3);
	}
	;

EqualityExpression
	: RelationalExpression
	{
		$$ = $1;
	}
	| EqualityExpression TOK_EQ RelationalExpression
	{
		$$ = new BinaryExpression (OP_EQ, $1, $3);
	}
	| EqualityExpression TOK_NE RelationalExpression
	{
		$$ = new BinaryExpression (OP_NE, $1, $3);
	}
	;

RelationalExpression
	: ShiftExpression
	{
		$$ = $1;
	}
	| RelationalExpression '<' ShiftExpression
	{
		$$ = new BinaryExpression (OP_LESS, $1, $3);
	}
	| RelationalExpression '>' ShiftExpression
	{
		$$ = new BinaryExpression (OP_GREATER, $1, $3);
	}
	| RelationalExpression TOK_LE ShiftExpression
	{
		$$ = new BinaryExpression (OP_LE, $1, $3);
	}
	| RelationalExpression TOK_GE ShiftExpression
	{
		$$ = new BinaryExpression (OP_GE, $1, $3);
	}
	;

ShiftExpression
	: AdditiveExpression
	{
		$$ = $1;
	}
	| ShiftExpression TOK_LEFT_SHIFT AdditiveExpression
	{
		$$ = new BinaryExpression (OP_SHIFT_LEFT, $1, $3);
	}
	| ShiftExpression TOK_RIGHT_SHIFT AdditiveExpression
	{
		$$ = new BinaryExpression (OP_SHIFT_RIGHT, $1, $3);
	}
	;

AdditiveExpression
	: MultiplicativeExpression
	{
		$$ = $1;
	}
	| AdditiveExpression '+' MultiplicativeExpression
	{
		$$ = new BinaryExpression (OP_ADD, $1, $3);
	}
	| AdditiveExpression '-' MultiplicativeExpression
	{
		$$ = new BinaryExpression (OP_SUB, $1, $3);
	}
	;

MultiplicativeExpression
	: UnaryExpression
	{
		$$ = $1;
	}
	| MultiplicativeExpression '*' UnaryExpression
	{
		$$ = new BinaryExpression (OP_MUL, $1, $3);
	}
	| MultiplicativeExpression '/' UnaryExpression
	{
		$$ = new BinaryExpression (OP_DIV, $1, $3);
	}
	| MultiplicativeExpression '%' UnaryExpression
	{
		$$ = new BinaryExpression (OP_MOD, $1, $3);
	}
	;

UnaryExpression
	: PostfixExpression
	{
		$$ = $1;
	}
	| TOK_INC UnaryExpression
	{
		$$ = new UnaryExpression (OP_PRE_INC, $2);
	}
	| TOK_DEC UnaryExpression
	{
		$$ = new UnaryExpression (OP_PRE_DEC, $2);
	}
	| '+' UnaryExpression
	{
		$$ = new UnaryExpression (OP_PLUS, $2);
	}
	| '-' UnaryExpression
	{
		$$ = new UnaryExpression (OP_MINUS, $2);
	}
	| '~' UnaryExpression
	{
		$$ = new UnaryExpression (OP_TILDA, $2);
	}
	| '!' UnaryExpression
	{
		$$ = new UnaryExpression (OP_NOT, $2);
	}
	| TOK_DELETE UnaryExpression
	{
		$$ = new UnaryExpression (OP_DELETE, $2);
	}
	| CastExpression
	{
		$$ = $1;
	}
	;

CastExpression
	: '(' PrimitiveType ')' UnaryExpression
	{
		$$ = new PrimitiveCastExpression ($2, $4);
	}
	| '(' SetIdentifier ')' UnaryExpression
	{
		$$ = new SetCastExpression ($2, $4);
	}
	;

PostfixExpression
	: PrimaryExpression
	{
		$$ = $1;
	}
	| PostfixExpression '[' Expression ']'
	{
		$$ = new ArrayExpression ($1, $3);
	}
	| PostfixExpression ParameterList
	{
		$$ = new CallExpression ($1, $2);
	}
	| PostfixExpression '.' Identifier
	{
		$$ = new FieldExpression ($1, $3);
	}
	| PostfixExpression TOK_INC
	{
		$$ = new UnaryExpression (OP_POST_INC, $1);
	}
	| PostfixExpression TOK_DEC
	{
		$$ = new UnaryExpression (OP_POST_DEC, $1);
	}
	;

PrimaryExpression
	: Identifier
	{
		$$ = new IdentifierExpression ($1);
	}
	| Literal
	{
		$$ = $1;
	}
	| TOK_THIS
	{
		$$ = new ThisExpression (curComponent, curComponent->line);
	}
	| '(' Expression ')'
	{
		$$ = new UnaryExpression (OP_PAR, $2);
	}
	| TOK_NEW NonVoidType ParameterList
	{
		$$ = new NewExpression ($2, $3, curComponent, curComponent->line);
	}
	;

Literal
	: IntegerLiteral
	{
		$$ = $1;
	}
	| TOK_LIT_FLOAT
	{
		$$ = new FloatExpression (lexString, curComponent, curComponent->line);
	}
	| TOK_LIT_STRING
	{
		$$ = new StringExpression (lexString, curComponent, curComponent->line);
	}
	| TOK_NULL
	{
		$$ = new NullExpression (curComponent, curComponent->line);
	}
	| BooleanLiteral
	{
		$$ = $1;
	}
	;

IntegerLiteral
	: TOK_LIT_INTEGER
	{
		$$ = new IntegerExpression (lexString, curComponent, curComponent->line);
	}
	| TOK_LIT_CHARACTER
	{
		$$ = new IntegerExpression (lexString, curComponent, curComponent->line);
	}
	;

BooleanLiteral
	: TOK_TRUE
	{
		$$ = new IntegerExpression ("1", curComponent, curComponent->line);
	}
	| TOK_FALSE
	{
		$$ = new IntegerExpression ("0", curComponent, curComponent->line);
	}
	;

/*
   Identifier
   */

IdentifierList
	: Identifier
	{
		$$ = new UniqueArray<Identifier>;
		$$->add ($1);
	}
	| IdentifierList ',' Identifier
	{
		$$ = $1;
		if (!$$->add ($3))
			$3->error ("the identifier %s is declared previously\n", $3->value);
	}
	;

QualifiedIdentifier
	: QIPart '.' QIPart
	{
		$$ = new QualifiedIdentifier ($1);
		$$->add ($3);
	}
	| QualifiedIdentifier '.' QIPart
	{
		$$ = $1;
		$$->add ($3);
	}
	;

QIPart
	: Identifier
	{
		$$ = $1;
	}
	| SetIdentifier
	{
		$$ = $1->identifier;
	}
	;

Identifier
	: TOK_IDENTIFIER
	{
//		fprintf (stderr, "%s[%d]. Identifier -> TOK_IDENTIFIER %s\n", __FILE__, __LINE__, firstToken? "true": "false");
		$$ = lexIdentifier;
	}
	;

SetIdentifier
	: TOK_SET_NAME
	{
		// mainSet is used when we have a component with no interface class and the token after the component name is the component name again.
		// In this case, the second component name should be identified as class name not identifier.
		if (lexSet)
			$$ = lexSet;
		else
			$$ = mainSet;
	}
	| TOK_CLASS Identifier
	{
		InternalType *it;
		it = new InternalType ($2);
		curComponent->addInternal (it);
		$$ = it;
	}
	;

TypeName
	: TOK_TYPE
	{
		$$ = lexType;
	}
	;

