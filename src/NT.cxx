
#include "parham.hxx"

NT::NT (ComponentType *_component, int _line)
{
	INIT (component);
	INIT (line);
}

void NT::unimplemented (const char *format, ...)
{
	va_list vl;

	component->error = true;
	program.error = true;
	fprintf (stderr, "Unimplemented: %s[%d]. ", component->fname, line);

	va_start (vl, format);
	vfprintf (stderr, format, vl);
	va_end (vl);
	fprintf (stderr, "\n");
}

void NT::error (const char *format, ...)
{
	va_list vl;

	component->error = true;
	program.error = true;
	fprintf (stderr, "Error: %s[%d]. ", component->fname, line);

	va_start (vl, format);
	vfprintf (stderr, format, vl);
	va_end (vl);
	fprintf (stderr, "\n");
}

void NT::internalError (const char *format, ...)
{
	va_list vl;
	char buffer[1024];

	va_start (vl, format);
	vsprintf (buffer, format, vl);
	va_end (vl);
	error ("internal error: %s", buffer);
}

void NT::putIndent (FILE *f, int indent)
{
	int i;

	for (i = 0; i < indent; ++i)
		fprintf (f, "\t");
}

NT::NT (NT *nt)
{
	component = nt->component;
	line = nt->line;
}

