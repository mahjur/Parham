
#ifndef __DECLARATION__
#define __DECLARATION__

#define MOD_PROVIDES	0x1
#define MOD_REQUIRES	0x2
#define MOD_LOCAL	0x4
#define MOD_EXTERN	0x8
#define MOD_CONST	0x10
#define MOD_VIRTUAL	0x20 //internal

class Modifier: public NT
{
	public:
		unsigned int value;

		Modifier (ComponentType *_component, int _line);
		void add (unsigned int);
		void add (Modifier *modifier);
		bool has (int v);
};

class Designator: public NT
{
	public:
		Designator (ComponentType *_component, int _line);
};

class ExpressionDesignator: public Designator
{
	public:
		Expression *expression;

		ExpressionDesignator (Expression *_expression);
};

class IdentifierDesignator: public Designator
{
	public:
		Identifier *identifier;

		IdentifierDesignator (Identifier *_identifier);
};

class Initializer: public NT
{
	public:
		Array<Designator> *designator;

		Initializer (ComponentType *_component, int _line);
		void setDesignation (Array<Designator> *designator);
		void analysis ();
};

class ExpressionInitializer: public Initializer
{
	public:
		Expression *expression;

		ExpressionInitializer (Expression *_expression);
};

class ArrayInitializer: public Initializer
{
	public:
		Array<Initializer> *list;

		ArrayInitializer (Array<Initializer> *_list, ComponentType *_component, int _line);
};

class InitializedIdentifier
{
	public:
		Identifier *identifier;
		Initializer *initializer;

		InitializedIdentifier (Identifier *_identifier, Initializer *_initializer);
};

class SingleInitializedIdentifier: public InitializedIdentifier
{
	public:
		ClassType *container;

		SingleInitializedIdentifier (ClassType *_container, Identifier *_identifier, Initializer *_initializer);
};

class Declaration: public NT
{
	public:
		Identifier *identifier;
		Modifier *modifier;
		Type *type;
		bool isRef;
		const char *pname;
		bool isProperty;
		enum DeclarationType {DECL_VALUE, DECL_ARGUMENT, DECL_ATTRIBUTE, DECL_CONSTRUCTOR, DECL_METHOD, DECL_INSTANCE, DECL_EXPLICIT, DECL_MAP, DECL_ASSOCIATION, DECL_INCLUSION, DECL_SUPER, DECL_LOCAL, DECL_ENUMERATION} which;

		Declaration (DeclarationType _which, bool _isProperty, Modifier *_modifier, Type *_type, bool _isRef, Identifier *_identifier, ComponentType *_component, int _line);
		void setModifier (Modifier *_modifier);
		void setIdentifier (Identifier *_identifier);
		void setType (Type *_type);
		bool matchIdentifier (Identifier *id);
		bool satisfied (ComponentType *component, Array<ClassType> *list);

		virtual void formSet ();

		virtual void publishHeader (FILE *f);
		virtual void publishSource (FILE *f);
		virtual void publish (FILE *f);
		void publishAttribute (FILE *f, bool header);
		void publishReference (FILE *f);
		virtual void analysis ();
		virtual void mapSet ();

#ifdef DEBUG
		virtual void dbg_print ();
#endif
};
/*
   Declaration
   	PropertyDeclaration
		AttributeDeclaration
		AssociationDeclaration
		ExecutionDeclaration
			ConstructorDeclaration
			MethodDeclaration
		ComponentDeclaration
			SuperDeclaration
			MapDeclaration
	ArgumentDeclaration
	InstanceDeclaration
	ExplicitDeclaration
	InclusionDeclaration

   Declaration					publish	-		-
	ArgumentDeclaration			-	-		-
	InstanceDeclaration			-	publishHeader	-
	InclusionDeclaration			-	-		-
	DeclarationStatement			publish	-		-
	ExplicitDeclaration			-	-		-
	PropertyDeclaration			-	-		-
		AssociationDeclaration		-	-		-
		ComponentDeclaration
			MapDeclaration		-	-		-
			SuperDeclaration
		AttributeDeclaration		-	-		-
		ExecutionDeclaration			-	publishHeader	publishSource
			ConstructorDeclaration	-	-		-
			MethodDeclaration	-	-		-
   */

/*
class ValueDeclaration: public Declaration 
{
	public:
		Identifier *identifier;
		LiteralExpression *literal;

		ValueDeclaration (Identifier *_identifier, LiteralExpression *_literal);
		virtual void formSet ();
		virtual void publishSource (FILE *f);
		virtual void publish (FILE *f);
};
*/

class PropertyDeclaration: public Declaration
{
	public:
		ClassType *container;
		PropertyConstructor cm;
//		int constructorCount;

		PropertyDeclaration (DeclarationType _which, ClassType *_container, Modifier *_modifier, Type *_type, bool _isRef, Identifier *_identifier, ComponentType *_component, int _line);
		virtual void setContainer (ClassType *_container);
		virtual void removeDuplicate ();
		virtual bool containInclusion (SuperDeclaration *);
};

class ComponentDeclaration: public PropertyDeclaration
{
	public:

		ComponentDeclaration (DeclarationType _which, ClassType *_container, Modifier *_modifier, Type *_type, bool _isRef, Identifier *_identifier, ComponentType *_component, int _line);
		virtual ComponentDeclaration *findUp (ClassType *s) = 0;
		virtual ComponentDeclaration *findDown (ClassType *s) = 0;
		virtual MethodDeclaration *resolveVT (MethodDeclaration *method, Array<ComponentDeclaration> *upPath, Array<ComponentDeclaration> *downPath) = 0;
};

class ArgumentDeclaration: public Declaration
{
	public:
		ArgumentDeclaration (Type *, Identifier *, ComponentType *, int line);
		void analysis (BlockSpace *ns);
};

class ArgumentDeclarationList: public NT
{
	public:
		Array<ArgumentDeclaration> list;

		ArgumentDeclarationList (ComponentType *_component, int _line);
		void add (ArgumentDeclaration *);
		void addFront (ArgumentDeclaration *);
		void analysis (BlockSpace *ns);
		void publish (FILE *f, ClassType *self, bool needVT);
		bool match (ArgumentDeclarationList *argList);
		bool accept (ExpressionList *argList);
};

class AttributeDeclaration: public PropertyDeclaration
{
	public:
		Initializer *initializer;
		AttributeDeclaration (Modifier *_modifier, Type *_type, InitializedIdentifier *ii, ComponentType *_component, int _line);
		virtual void formSet ();
		virtual void analysis ();
};

class SuperDeclaration: public ComponentDeclaration
{
	public:
		InclusionDeclaration *inclusion;

		SuperDeclaration (ClassType *_container, InclusionDeclaration *_inclusion);
		bool hasLeft (Array<ClassType> *list);
		virtual ComponentDeclaration *findUp (ClassType *s);
		virtual ComponentDeclaration *findDown (ClassType *s);
//		virtual MethodDeclaration *resolveVT (MethodDeclaration *method, Array<ComponentDeclaration> *point);
		virtual MethodDeclaration *resolveVT (MethodDeclaration *method, Array<ComponentDeclaration> *upPath, Array<ComponentDeclaration> *downPath);
		virtual void removeDuplicate ();
};

/*
   A constructor may be for 
   	* super set
	* map declaration
	* internal class

   The first step is to identify that the constructor is for which one.
   	* super set
	* map declaration

   If it is identified by an identifier or qualified identifier
   	* the identifier should refer to a map declaration

   If it is identified by a set name
   	* it may refer to a super set
	* it may refer to a map declaration? No
   */

class ExecutionDeclaration: public PropertyDeclaration
{
	private:
//		virtual int addToContainer (ClassType *set) = 0;
		virtual int addToContainer () = 0;
	public:
		Type *retType;
		ArgumentDeclarationList *argList;
		BlockStatement *body;
		ClassType *thisType;

		ExecutionDeclaration (DeclarationType _which, Modifier *_modifier, Type *_retType, Identifier *_identifier, ArgumentDeclarationList *_argList);
		void setBody (BlockStatement *_body);
		void publishSignature (FILE *f);
		virtual void formSet ();
		virtual void publishSource (FILE *f);
		virtual void publishHeader (FILE *f);
		virtual void setContainer (ClassType *_container);
};

class ConstructorDeclaration: public ExecutionDeclaration
{
	private:
//		virtual int addToContainer (ClassType *set);
		virtual int addToContainer ();

	public:
		Array<ConstructorCall> *callList;

		ConstructorDeclaration (ClassType *, ArgumentDeclarationList *);
		void setCallList (Array<ConstructorCall> *_callList);
		virtual void analysis ();
		void addConstructorCall (ConstructorCall *cc);
};

class MethodDeclaration: public ExecutionDeclaration
{
	private:
//		virtual int addToContainer (ClassType *set);
		virtual int addToContainer ();
	public:
		MethodDeclaration (Modifier *_modifier, Type *_retType, Identifier *_identifier, ArgumentDeclarationList *_argList);
		void publishVT (FILE *f);
		MethodDeclaration *resolveVT (Array<ComponentDeclaration> *path, Array<ComponentDeclaration> *upPath, Array<ComponentDeclaration> *downPath);
		virtual void analysis ();
//		MethodDeclaration *castTo (MethodDeclaration *dst, Array<ComponentDeclaration> *path, int point, ClassType *owner);
		MethodDeclaration *castFrom (MethodDeclaration *sig, Array<ComponentDeclaration> *upPath, Array<ComponentDeclaration> *downPath, ClassType *owner);
};

class AssociationDeclaration: public PropertyDeclaration
{
	public:
		AssociationDeclaration (ClassType *_container, Modifier *_modifier, Type *_type);

		virtual void formSet ();
		AssociationDeclaration *clone ();
};

class MapDelegate
{
	public:
		MethodDeclaration *left, *right;
		Array<ComponentDeclaration> *rightPath;

		MapDelegate (MethodDeclaration *_left, MethodDeclaration *_right, Array<ComponentDeclaration> *_rightPath);
		MethodDeclaration *resolve (MethodDeclaration *md, Array<ComponentDeclaration> *point);
};

class MapDeclaration: public ComponentDeclaration
{
	public:
		Array<ClassType> *stateMap;
		InstanceDeclaration *instance;
		Array<MapDelegate> delegateList;

		MapDeclaration (ClassType *, Array<ClassType> *_stateMap, ComponentType *_component, int _line);
		void setInstance (InstanceDeclaration *_instance);
		void formSet (ClassType *_type);
		void map ();
		virtual ComponentDeclaration *findUp (ClassType *s);
		virtual ComponentDeclaration *findDown (ClassType *s);
		virtual MethodDeclaration *resolveVT (MethodDeclaration *method, Array<ComponentDeclaration> *upPath, Array<ComponentDeclaration> *downPath);
		void addDelegate (Delegate *delegate);
		virtual bool containInclusion (SuperDeclaration *);
};

class Delegate: public NT
{
	public:
		Identifier *instance, *instanceSet;
		Identifier *set;
		Identifier *left, *right;

		Delegate (Identifier *_set, Identifier *_left, Identifier *_right);
		Delegate (Identifier *_set, Identifier *_left, Identifier *_instance, Identifier *_instanceSet, Identifier *_right);
		bool match (Delegate *delegate);
};

class InstanceDeclaration: public Declaration
{
	public:
		QualifiedIdentifier *qid;
		Array<MapDeclaration> *mapList;
		Array<IntegerExpression> *dimension;
		Array<Delegate> *delegateList;

		InstanceDeclaration (Modifier *, QualifiedIdentifier *, Array<MapDeclaration> *, Array<IntegerExpression> *_dimension, ComponentType *, int);
		bool matchComponent (Identifier *identifier);
		void setDelegate (Array<Delegate> *);
		virtual void formSet ();
		virtual void publishHeader (FILE *f);
		virtual void mapSet ();
		MapDeclaration *findMapByType (ClassType *s);
		MapDeclaration *findMapByContainer (ClassType *s);
		MapDeclaration *findMapByType (Identifier *id);
		void applyDelegate (Delegate *delegate);
};

class ExplicitDeclaration: public Declaration
{
	public:
		Array<PropertyDeclaration> *propertyList;

		ExplicitDeclaration (ClassType *_container, Array<PropertyDeclaration> *_propertyList, ComponentType *_component, int _line);
		virtual void formSet ();
};

class Disambiguation: public NT
{
	public:
		Statement *statement;

		Disambiguation (Statement *, ComponentType *, int);
};

class StateInitializer: public NT
{
	public:
		ClassType *set;
		Expression *expression;

		StateInitializer (ClassType *_set, Expression *_expression);
		bool match (StateInitializer *st);
};

class InclusionBodyItem: public NT
{
	public:
		InclusionBodyItem (ComponentType *_component, int _line);
};

class StateAttributeDeclaration: public InclusionBodyItem
{
	public:
		Type *type;
		Identifier *identifier;
		UniqueArray<StateInitializer> *initializer;

		StateAttributeDeclaration (Type *_type, Identifier *_identifier, UniqueArray<StateInitializer> *_initializer);
};

class TransitionState: public NT
{
	public:
		ClassType *set;

		TransitionState (ClassType *_set, ComponentType *_component, int _line);
};

class TransitionDeclaration: public InclusionBodyItem
{
	public:
		Array<TransitionState> list;

		TransitionDeclaration (TransitionState *_state);
		void add (TransitionState *_state);
};

class InclusionBody: public NT
{
	public:
		Array<InclusionBodyItem> *body;
		Disambiguation *disambiguation;

		InclusionBody (Array<InclusionBodyItem> *_body, Disambiguation *_disambiguation, ComponentType *_component, int _line);
};

class InclusionDeclaration: public Declaration
{
	public:
		ClassType *left;
		UniqueArray<ClassType> *inclusion;
		Array<InclusionBodyItem> *body;
		Disambiguation *disambiguation;

		InclusionDeclaration (ClassType *_left, UniqueArray<ClassType> *_inclusion, Array<InclusionBodyItem> *_body, Disambiguation *_disambiguation);
		virtual void formSet ();
		bool hasInclusion (ClassType *super, ClassType *sub);
};

#endif

