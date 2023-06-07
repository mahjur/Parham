
/*
   1. name lookup should be revised to support base field
   	1.1. base was added.
	1.2. lookup should consider base
   2. in the grammar, calls to FieldList should be added.
   	2.1. add was added
	2.2. makeType was added.
   */

#ifndef __FIELD_LIST__
#define __FIELD_LIST__

class FieldListItem
{
		enum FieldStatus {FS_FIELD, FS_TYPE, FS_DUPLICATE} status;
		bool match (FieldListItem *fli);

	public:
		AttributeDeclaration *attribute;

		FieldListItem (AttributeDeclaration *attribute);
		InternalType *makeType ();
		void markDuplicate ();
		bool isType ();
};

class FieldList
{
		Array<FieldListItem> list;

		FieldListItem *find (Identifier *identifier);

	public:
		// This function is called whenever a new field declration is encountered
		void flAdd (AttributeDeclaration *attribute);

		// This function is called whenever a field is used instead of a class
		InternalType *flMakeType (Identifier *identifier);

		Type *flLookup (Identifier *identifier);
		bool flIsType (Identifier *identifier);
};

#endif

