
#include "parham.hxx"

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */

Program program;

bool Program::compile (const char *fname, const char *mainfunction)
{
	ComponentType *component;

	component = front (fname);
	if (!component)
		return false;
	component->mapSet ();
	if (component->error)
		return false;
	component->analysis ();
	if (error) //we check program error
		return false;
	if (component->publish (mainfunction))
		return !error;
	return error;
}

ComponentType *Program::parse (const char *fname)
{
	char buffer[1024];
	FILE *f;
	extern FILE *yyin;

	sprintf (buffer, "%s.par", fname);
	f = fopen (buffer, "r");
	if (!f)
	{
		fprintf (stderr, "unable to open input file: %s\n", buffer);
		return NULL;
	}

	curComponent = new ComponentType (strdup (fname));
	yyin = f;
	if (yyparse () != 0)
	{
		fprintf (stderr, "%s contains syntax errors\n", buffer);
		fclose (f);
		return NULL;
	}
	fclose (f);
	if (curComponent->error)
		return NULL;
	return curComponent;
}

ComponentType *Program::front (const char *fname)
{
	ComponentType *component;

	component = parse (fname);
	if (!component)
		return NULL;
	component->formSet ();
	if (component->error)
		return NULL;
	return component;
}

Program::Program ()
{
	error = false;
}

