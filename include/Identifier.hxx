
#ifndef __IDENTIFIER__
#define __IDENTIFIER__

class Identifier: public NT
{
	public:
		const char *value;

		Identifier (const char *_value, ComponentType *_component, int _line);
		Identifier (Identifier *identifier);
		bool match (Identifier *identifier);
		void publish (FILE *f);

		void erUndefined ();
		void erDuplicate ();
		void erAmbigious ();
};

class QualifiedIdentifier: public NT
{
	public:
		Array<Identifier> list;

		QualifiedIdentifier (Identifier *);
		void add (Identifier *identifier);
		void flat (char *buffer, char sep);
		void flat (char *buffer, int count, char sep);
		bool match (QualifiedIdentifier *qid, int count);
		bool match (Identifier *identifier);
		bool same (QualifiedIdentifier *qid);
};

#endif

