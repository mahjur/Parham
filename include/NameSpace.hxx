
#ifndef __NAMESPACE__
#define __NAMESPACE__

// Symbols

class Polymorphic
{
	public:
		Array<ExecutionDeclaration> list;

		Polymorphic (ExecutionDeclaration *decl);
		Polymorphic ();

		int add (ExecutionDeclaration *decl);
		Declaration *lookup (ExpressionList *argList);
		ExecutionDeclaration *lookup (ExecutionType *methodType);
};

class Symbol
{
	public:
		enum SymbolType {SYM_SIMPLE, SYM_POLYMORPHIC, SYM_TYPE} which;
		Identifier *identifier;

		Symbol (SymbolType _which, Identifier *_identifier);
		bool match (Symbol *symbol);
		bool match (Identifier *id);
		virtual Declaration *lookup (ExpressionList *argList);
		virtual ~Symbol () {}
};

class SimpleSymbol: public Symbol
{
	public:
		Declaration *declaration;

		SimpleSymbol (Declaration *_declaration);
		virtual Declaration *lookup (ExpressionList *argList);
};

class PolymorphicSymbol: public Symbol
{
	public:
		Polymorphic poly;

		PolymorphicSymbol (ExecutionDeclaration *decl);
		virtual Declaration *lookup (ExpressionList *argList);
		int add (ExecutionDeclaration *decl);
};

class TypeSymbol: public Symbol
{
	public:
		NamedType *type;

		TypeSymbol (NamedType *_type);
		virtual Declaration *lookup (ExpressionList *argList);
};

// NameSpaces

class NameSpace
{
	protected:
		UniqueArray<Symbol> list;

	public:
		SimpleSymbol *addSimple (Declaration *decl);

};

class BlockSpace: public NameSpace
{
	public:
		BlockSpace *parent;

		BlockSpace (BlockSpace *_parent);
		Declaration *lookup (Identifier *identifier);
};

class ContainerSpace: public NameSpace
{
	public:
		int addPolymorphic (MethodDeclaration *decl); // index of the added item
};

class ClassSpace: public ContainerSpace
{
		ClassSpace *base;

	public:
		Array<SuperDeclaration> super;
		Array<MapDeclaration> extension;

		ClassSpace ();
		void setBase (ClassSpace *base);
		void addSuper (SuperDeclaration *decl);
		void addExtension (MapDeclaration *decl);

		PropertyDeclaration *lookup (Identifier *identifier, ExpressionList *argList, Array<ComponentDeclaration> *path);

		MethodDeclaration *lookupMethod (MethodDeclaration *);
		Polymorphic *lookupMethod (Identifier *, Array<ComponentDeclaration> *path);
		MethodDeclaration *lookupNonExternMethod (MethodDeclaration *method, Array<ComponentDeclaration> *downPath);

		Polymorphic *lookupMethod (Identifier *identifier);
};

class ComponentSpace: public ContainerSpace
{
	public:
		Array<InstanceDeclaration> extension;

		TypeSymbol *addType (NamedType *type);
		void addExtension (InstanceDeclaration *decl);

		Type *lookupType (Identifier *);
		Declaration *lookup (Identifier *identifier, ExpressionList *argList);
};

/*
   name space extension
   	- module
		- instance declaration		DONE
	- set
		- inclusion declaration		DONE
	   	- anonumous associations	DONE
		- map declaration		DONE
   name space items
   	- module
		- instance declaration		DONE
		- enumeration declaration	DONE
		- enumeration item		DONE
		- value declaration		DONE
		- typedef declaration		DONE
		- method declaration		DONE
		- attribute delcaration		DONE
	- set
	   	- named associaitons		DONE
		- map declaration		DONE
		- state attribute declaration
		- attribute declaration		DONE
		- method declaration		DONE
		- constructor declaration	DONE
	- block
		- argument declaration
		- declaration statement
   */

#endif

