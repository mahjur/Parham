
#ifndef __PROGRAM__
#define __PROGRAM__

class Program
{
		ComponentType *parse (const char *fname);

	public:
		bool error;

		Program ();
		ComponentType *front (const char *fname);
		bool compile (const char *fname, const char *mainfunction);
};

extern Program program;

#endif

