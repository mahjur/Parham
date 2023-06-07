
#ifndef __MODULE__
#define __MODULE__

/*
   We can assume that the output of the compiler is a set of classes.
   Some classes are real classes and some others are function pointer classes.
   	We define the above classes for every set regardless of the set type (abstract, actual, state).
   */
class ComponentType: public ContainerType
{
		int blockStatementCount;
	public:
		void enterBlock ();
		void leaveBlock ();
		bool isInBlock ();

	private:
		bool reorder ();
		Array<ClassType> ordered;
		virtual void addVirtual (MethodDeclaration *method);
		void publishMainFunction (FILE *f, const char *mainfunction);

	public:
		ComponentResolve rm; // resolve manager
//		ComponentSpace ns;
//		FieldList fieldList;
		const char *fname;
		const char *pname;
		QualifiedIdentifier *qname;

		QualifiedIdentifier *package;
		Array<QualifiedIdentifier> importList;
		bool error;

		Array<InterfaceType> interface;
		Array<InternalType> internal;
		Array<Declaration> item;
		Array<InstanceDeclaration> instanceList;

		void addInterface (InterfaceType *);
		void addInternal (InternalType *);
		void addItem (Declaration *mi);
		bool addAttribute (Declaration *attrib);
		void addInstance (InstanceDeclaration *instance);
		bool hasInclusion (ClassType *super, ClassType *sub);

		ComponentType (const char *_name);
		void formSet ();
		void analysis ();
		bool publish (const char *mainfunction);
		bool publishHeader ();
		bool publishSource (const char *mainfunction);

		void setPackage (QualifiedIdentifier *_package);
		void addImport (QualifiedIdentifier *import);
		void setName (QualifiedIdentifier *_qname);
		bool accept (InstanceDeclaration *mi);

		void addType (NamedType *type);
		Type *lookupType (Identifier *identifier);
		void mapSet ();
		bool same (ComponentType *component);
		InstanceDeclaration *lookupInstance (Identifier *);
};

extern ComponentType *curComponent;

#endif

