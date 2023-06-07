
#ifndef __TYPE__
#define __TYPE__

/*
   Type
     |
     +--- NullType
     |
     +--- ArrayType
     |
     +--- TransitionType
     |
     +--- ExecutionType
     |
     +--- NamedType
            |
	    +--- VoidType
	    |
	    +--- IntegerType
	    |
	    +--- FloatType
	    |
	    +--- TypedefDeclaration
	    |
	    +--- EnumerationType
	    |
	    +--- ContainerType
	           |
		   +--- ClassType
		          |
			  +--- InternalType
			  |
			  +--- InterfaceType
   */

class Type: public NT
{
	public:
		enum WhichType {TYPE_ARRAY, TYPE_COMPONENT, TYPE_VOID, TYPE_INTEGER, TYPE_FLOAT, TYPE_CLASS, TYPE_TYPEDEF, TYPE_ENUMERATION, TYPE_TRANSITION, TYPE_EXECUTION, TYPE_NULL} which;

		Type (WhichType _which, ComponentType *_component, int _line);
		virtual void publishPre (bool withPost, bool asRef, FILE *f);
		virtual void publishPost (FILE *f);
		virtual void analysis ();

		virtual bool accept (Type *type, Cast *castPath);
		bool accept (Expression *exp);
		virtual bool match (Type *type);
		virtual bool equal (Type *) = 0;
		virtual bool attrSatisfied (Array<ClassType> *list);
};

class NullType: public Type
{
	public:
		NullType (ComponentType *_component, int _line);
		virtual bool equal (Type *type);
};

class ArrayType: public Type
{
	public:
		Type *base;
		IntegerExpression *index;

		ArrayType (Type *_base, IntegerExpression *_index);
		virtual void publishPre (bool withPost, bool asRef, FILE *f);
		virtual void publishPost (FILE *f);
		virtual bool accept (Type *type, Cast *castPath);
		virtual bool match (Type *type);
		virtual bool equal (Type *);
		static Type *make (Type *base, Array<IntegerExpression> *dimension);
		virtual bool attrSatisfied (Array<ClassType> *list);
};

/* Named Type */

class NamedType: public Type
{
	public:
		Identifier *identifier;

		NamedType (WhichType _which, Identifier *_identifier);
		virtual void publishPre (bool withPost, bool asRef, FILE *f);
		virtual bool equal (Type *type);
};

class VoidType: public NamedType
{
	public:
		VoidType (ComponentType *_component, int _line);
};

class IntegerType: public NamedType
{
	public:
		IntegerType (const char *_name, ComponentType *_component, int _line);
		virtual bool accept (Type *type, Cast *castPath);
};

class FloatType: public NamedType
{
	public:
		FloatType (const char *_name, ComponentType *_component, int _line);
		virtual bool accept (Type *type, Cast *castPath);
};

class UnificationList: public NT
{
	public:
		UniqueArray<ClassType> *list;

		UnificationList (UniqueArray<ClassType> *_list, ComponentType *_component, int _line);
};

class ContainerType: public NamedType
{
		virtual void addVirtual (MethodDeclaration *method) = 0;

	public:
		ContainerSpace *nsp;
		Array<Declaration> propertyList;

		ContainerType (WhichType _which, Identifier *_identifier, ContainerSpace *_nsp);
		int addMethod (MethodDeclaration *method);
};

/*
class Mapper
{
	public:
		ClassType *target;
		Array<MapDeclaration> pathx;

		Mapper (ClassType *_target);
		void init (MapDeclaration *map);
		void publish (FILE *f, int index);
		int match (Array<Declaration> *dest);
};
*/

class VTValue
{
	public:
		const char *entry;

		VTValue (const char*_entry);
		virtual void publish (FILE *f, int indent) = 0;
};

class VTDeclaration: public VTValue
{
		virtual void publish (FILE *f, int indent);
	public:
		Declaration *value;

		VTDeclaration (const char *_entry, Declaration *_value);
};

class VTable: public VTValue
{
		virtual void publish (FILE *f, int indent);
	public:
		Array<VTValue> list;

		VTable (const char *_entry);
		void publish (FILE *f);
};


class VTableType: public NT
{
	public:
		ClassType *set;

		Array<MethodDeclaration> methodList;

		VTableType (ClassType *_set, ComponentType *_component, int _line);
		void add (MethodDeclaration *method);

		void publishEntry (FILE *f, const char *name);
		void publishEntry (FILE *f);
		bool hasEntry ();
		Statement *generateAssignment ();
		void publishClass (FILE *f);
		void publishPrototype (FILE *f);
		void publishArgument (FILE *f);
		Expression *makeCallArgument (PropertyDeclaration *pd);
//		Expression *makeCallArgument (const char *name);
		VTable *fill (const char *name, Array<ComponentDeclaration> *path, ClassType *owner);
};

class SetDelegate
{
	public:
		InstanceDeclaration *instance;
		Identifier *left, *right;

		SetDelegate (InstanceDeclaration *_instance, Identifier *_left, Identifier *_right);
};

class ClassType: public ContainerType
{
		void markVirtual (MethodDeclaration *m);
		void markVirtual2 (MethodDeclaration *m);
		virtual void addVirtual (MethodDeclaration *method);
		bool accept2 (ClassType *type, Array<PropertyDeclaration> *path);
		void setPName ();

	public:
		ClassResolve rm;
		ClassConstructor cm;

		Modifier *modifier;
//		ClassSpace ns;
		const char *pname;
		bool isState;
		bool isAbstract;
		UnificationList *unificationList;
		int indexName;
//		Mapper mapper;
		VTableType vtable;
		bool mainSet;
		bool isInternal;

		Array<PropertyDeclaration> attributeList;
		Array<MethodDeclaration> methodList;
		Array<MapDeclaration> mapAttributes;
//		Polymorphic constructorList;

		Array<SuperDeclaration> superList;
		Array<InclusionDeclaration> subList;

		ClassType (bool _isInternal, Identifier *_identifier);
		ClassType (bool _isInternal, Modifier *_modifier, Identifier *_identifier);

		bool same (ClassType *set);

		void addUnification (UnificationList *_unificationList);
		void addAttribute (PropertyDeclaration *attrib);
		void addMap (MapDeclaration *mapDecl);
		int addMethod (MethodDeclaration *method);
		int addConstructor (ConstructorDeclaration *constructor);
		void addSuper (InclusionDeclaration *incDecl);
		void addSub (InclusionDeclaration *incDecl);
		void removeProperty (PropertyDeclaration *prop);

		void publishHeader (FILE *f);
		void publishSource (FILE *f);
		void publishName (FILE *f);
		virtual void publishPre (bool withPost, bool asRef, FILE *f);
		void formSet ();
		Identifier *makeName (ComponentType *owner, int line);
		void map (MapDeclaration *map);
		virtual bool accept (Type *type, Cast *castPath);
		virtual bool match (Type *type);
		void markVirtual ();
		bool satisfied (Array<ClassType> *list);
		virtual void analysis ();
		virtual bool canInstantiate ();
		virtual void publishVT (FILE *f);
		void up (Cast *castPath);
		void down (Cast *castPath);
};

class InternalType: public ClassType
{
		VTable *vt;

	public:
		InternalType *base;

		InternalType (Identifier *);
		InternalType (Modifier *_modifier, Identifier *);
		void setBase (InternalType *base);

		virtual bool canInstantiate ();
		virtual void publishVT (FILE *f);
		virtual void analysis ();
		virtual bool attrSatisfied (Array<ClassType> *list);
};

class InterfaceType: public ClassType
{
	public:
		UniqueArray<Identifier> *stateList;

		InterfaceType (Identifier *_identifier, UniqueArray<Identifier> *_stateList);
		InterfaceType (Modifier *_modifier, Identifier *_identifier, UniqueArray<Identifier> *_stateList);
};

class TransitionType: public Type
{
	public:
		Type *from;
		Type *to;

		TransitionType (ClassType *_from, ClassType *_to);
		virtual bool accept (Type *type, Cast *castPath);
		virtual bool match (Type *type);
		virtual bool equal (Type *type);
};

class TypedefDeclaration: public NamedType
{
	public:
		Type *type;

		TypedefDeclaration (Identifier *_identifier, Type *_type);
		virtual bool accept (Type *type, Cast *castPath);
		virtual bool match (Type *type);
};

class EnumerationItem: public Declaration
{
	public:
		IntegerExpression *literal;

		EnumerationItem (Identifier *_identifier, IntegerExpression *_literal);
		bool match (EnumerationItem *item);
//		virtual void analysis ();
		void publish (FILE *f);
};

class EnumerationType: public NamedType
{
	public:
		UniqueArray<EnumerationItem> *list;

		EnumerationType (Identifier *_identifier, UniqueArray<EnumerationItem> *_list, ComponentType *_component, int _line);
		virtual void publishPre (bool withPost, bool asRef, FILE *f);
		virtual bool accept (Type *type, Cast *castPath);
		virtual bool match (Type *type);
};

class ExecutionType: public Type
{
	public:
		Type *retType;
		ArgumentDeclarationList *argList;

		ExecutionType (Type *_retType, ArgumentDeclarationList *_argList);
		virtual bool match (Type *type);
		virtual bool accept (Type *type, Cast *castPath);
		bool accept (ExpressionList *argList);
		virtual bool equal (Type *type);
		void addFront (ArgumentDeclaration *);
};

#endif

