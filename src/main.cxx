
#include "parham.hxx"

int main (int argc, char **argv)
{
	char *fname;
	int len;
	const char *mainfunction;

	fprintf (stderr, "Parham Compiler (release: 2023.04.10)\n");
	mainfunction = "main";
	switch (argc)
	{
		case 3:
			mainfunction = argv[2];
		case 2:
			fname = argv[1];
			len = strlen (fname);
			if (!strcmp (fname + len - 4, ".par"))
				fname[len - 4] = 0;
			break;
		default:
			fprintf (stderr, "Usage is: %s <input file> <the name of the entry function>?\n", argv[0]);
			return -1;
	}
	if (program.compile (fname, mainfunction))
	{
		fprintf (stderr, "%s.par was compiled successfully\n", argv[1]);
		return 0;
	}
	fprintf (stderr, "%s.par has errors\n", argv[1]);
	return -1;
}

