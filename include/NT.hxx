
#ifndef __NT__
#define __NT__

/*
   	NT
	NamedNT

	Declaration
	Statement
	Expression
	Type
   */

class NT
{
	public:
		ComponentType *component;
		int line;

		NT (ComponentType *_component, int _line);
		NT (NT *nt);
		static void putIndent (FILE *f, int indent);

		void error (const char *format, ...);
		void internalError (const char *format, ...);
		void unimplemented (const char *format, ...);
};

#define INIT(x)		x=_##x
#define INITHIS(x)	this->x=x
#define UNIMPLEMENTED()	unimplemented ("%s[%d]. unimplemented: %s", __FILE__, __LINE__, __FUNCTION__)

#endif

