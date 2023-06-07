
#ifndef __CONSTRUCTOR_MANAGER__
#define __CONSTRUCTOR_MANAGER__

class PropertyConstructor
{
	public:
		int counter;

		PropertyConstructor ();
		int incCounter ();
};

class ClassConstructor
{
		ClassType *ct;

	public:
		Polymorphic constructorList;

		ClassConstructor (ClassType *ct);
		void initCounters ();
		void checkCounters (ConstructorDeclaration *caller);
		ConstructorDeclaration *findConstructor (ExpressionList *argList);
		int add (ConstructorDeclaration *constructor);

		void publishHeader (FILE *f);
		void publishSource (FILE *f);
};

#endif

