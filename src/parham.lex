
%{
#include "parham.hxx"
#include "parham.tab.hxx"

void count (void);
void comment (void);
void linecomment (void);
int check_type (void);

char *lexString;
Identifier *lexIdentifier;
ClassType *lexSet;
Type *lexType;

%}

D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
E			[Ee][+-]?{D}+
FS			(f|F|l|L)
IS			(u|U|l|L)*

%%
"/*"			{ comment(); }
"//"			{ linecomment (); }



"+="			{ count (); return TOK_ADD_ASSIGN; }
"&&"			{ count (); return TOK_AND; }
"&="			{ count (); return TOK_AND_ASSIGN; }
"bool"			{ count (); return TOK_BOOL; }
"break"			{ count (); return TOK_BREAK; }
"case"			{ count (); return TOK_CASE; }
"char"			{ count (); return TOK_CHAR; }
"class"			{ count (); return TOK_CLASS; }
"const"			{ count (); return TOK_CONST; }
"continue"		{ count (); return TOK_CONTINUE; }
"--"			{ count (); return TOK_DEC; }
"default"		{ count (); return TOK_DEFAULT; }
"delete"		{ count (); return TOK_DELETE; }
"/="			{ count (); return TOK_DIV_ASSIGN; }
"do"			{ count (); return TOK_DO; }
"double"		{ count (); return TOK_DOUBLE; }
"..."			{ count (); return TOK_ELLIPSIS; }
"else"			{ count (); return TOK_ELSE; }
"=="			{ count (); return TOK_EQ; }
"requires"		{ count (); return TOK_REQUIRES; }
"false"			{ count (); return TOK_FALSE; }
"float"			{ count (); return TOK_FLOAT; }
"for"			{ count (); return TOK_FOR; }
"extern"		{ count (); return TOK_EXTERN; }
">="			{ count (); return TOK_GE; }
"goto"			{ count (); return TOK_GOTO; }
"if"			{ count (); return TOK_IF; }
"++"			{ count (); return TOK_INC; }
"int"			{ count (); return TOK_INT; }
"<="			{ count (); return TOK_LE; }
"<<="			{ count (); return TOK_LEFT_ASSIGN; }
"<<"			{ count (); return TOK_LEFT_SHIFT; }
"local"			{ count (); return TOK_LOCAL; }
"long"			{ count (); return TOK_LONG; }
"%="			{ count (); return TOK_MOD_ASSIGN; }
"*="			{ count (); return TOK_MUL_ASSIGN; }
"!="			{ count (); return TOK_NE; }
"new"			{ count (); return TOK_NEW; }
"null"			{ count (); return TOK_NULL; }
"||"			{ count (); return TOK_OR; }
"|="			{ count (); return TOK_OR_ASSIGN; }
"provides"		{ count (); return TOK_PROVIDES; }
"return"		{ count (); return TOK_RETURN; }
"->"			{ count (); return TOK_RIGHT_ARROW; }
">>="			{ count (); return TOK_RIGHT_ASSIGN; }
">>"			{ count (); return TOK_RIGHT_SHIFT; }
"short"			{ count (); return TOK_SHORT; }
"signed"		{ count (); return TOK_SIGNED; }
"-="			{ count (); return TOK_SUB_ASSIGN; }
"switch"		{ count (); return TOK_SWITCH; }
"this"			{ count (); return TOK_THIS; }
"true"			{ count (); return TOK_TRUE; }
"unsigned"		{ count (); return TOK_UNSIGNED; }
"void"			{ count (); return TOK_VOID; }
"while"			{ count (); return TOK_WHILE; }
"^="			{ count (); return TOK_XOR_ASSIGN; }

","			{ count (); return ','; }
"."			{ count (); return '.'; }
";"			{ count (); return ';'; }
"*"			{ count (); return '*'; }
"["			{ count (); return '['; }
"]"			{ count (); return ']'; }
":"			{ count (); return ':'; }
"{"			{ count (); return '{'; }
"}"			{ count (); return '}'; }
"("			{ count (); return '('; }
")"			{ count (); return ')'; }
"-"			{ count (); return '-'; }
"|"			{ count (); return '|'; }
"+"			{ count (); return '+'; }
"~"			{ count (); return '~'; }
"!"			{ count (); return '!'; }
"&"			{ count (); return '&'; }
"/"			{ count (); return '/'; }
"%"			{ count (); return '%'; }
"<"			{ count (); return '<'; }
">"			{ count (); return '>'; }
"^"			{ count (); return '^'; }
"?"			{ count (); return '?'; }
"="			{ count (); return '='; }

{L}({L}|{D})*		{ count(); return(check_type()); }
\'.\'			{ count(); lexString = (char *) malloc (yyleng+1); memcpy (lexString, yytext, yyleng); lexString[yyleng] = 0; return(TOK_LIT_CHARACTER); }

{D}+{E}{FS}?		{ count(); lexString = (char *) malloc (yyleng+1); memcpy (lexString, yytext, yyleng); lexString[yyleng] = 0; return(TOK_LIT_FLOAT); }
{D}*"."{D}+({E})?{FS}?	{ count(); lexString = (char *) malloc (yyleng+1); memcpy (lexString, yytext, yyleng); lexString[yyleng] = 0; return(TOK_LIT_FLOAT); }
{D}+"."{D}*({E})?{FS}?	{ count(); lexString = (char *) malloc (yyleng+1); memcpy (lexString, yytext, yyleng); lexString[yyleng] = 0; return(TOK_LIT_FLOAT); }

0[xX]{H}+{IS}?		{ count(); lexString = (char *) malloc (yyleng+1); memcpy (lexString, yytext, yyleng); lexString[yyleng] = 0; return(TOK_LIT_INTEGER); }
0{D}+{IS}?		{ count(); lexString = (char *) malloc (yyleng+1); memcpy (lexString, yytext, yyleng); lexString[yyleng] = 0; return(TOK_LIT_INTEGER); }
{D}+{IS}?		{ count(); lexString = (char *) malloc (yyleng+1); memcpy (lexString, yytext, yyleng); lexString[yyleng] = 0; return(TOK_LIT_INTEGER); }
L?'(\\.|[^\\'])+'	{ count(); lexString = (char *) malloc (yyleng+1); memcpy (lexString, yytext, yyleng); lexString[yyleng] = 0; return(TOK_LIT_INTEGER); }

L?\"(\\.|[^\\"])*\"	{ count(); lexString = (char *) malloc (yyleng+1); memcpy (lexString, yytext, yyleng); lexString[yyleng] = 0; return(TOK_LIT_STRING); }

\n			{ curComponent->line++; }
[ \r\t\v\n\f]		{ count(); }
.			{ fprintf (stderr, "line[%d]. invalid character [%c]\n", curComponent->line, *yytext); exit (1); }

%%

int yywrap (void)
{
	return 1;
}

void linecomment (void)
{
	char c;

	for ( ; ; )
	{
		c = yyinput ();
		if (c == EOF)
		{
			fprintf (stderr, "line[%d]. unterminated comment\n", curComponent->line);
			exit (1);
		}
		else if (c == '\n')
		{
			curComponent->line++;
			return;
		}
	}
}

void comment(void)
{
	char c;

	for ( ; ; )
	{
		c = yyinput ();
		if (c == EOF)
		{
			fprintf (stderr, "line[%d]. unterminated comment\n", curComponent->line);
			exit (1);
		}
		if (c == '*')
		{
			c = yyinput ();
			if (c == EOF)
			{
				fprintf (stderr, "line[%d]. unterminated commenct\n", curComponent->line);
				exit (1);
			}
			if (c == '/')
				return;
			unput (c);
		}
		else if (c == '\n')
			curComponent->line++;
	}
}


int column = 0;
int tokenCount = 0;

void count()
{
	int i;

	tokenCount++;

	for (i = 0; yytext[i] != '\0'; i++)
		if (yytext[i] == '\n')
			column = 0;
		else if (yytext[i] == '\t')
			column += 8 - (column % 8);
		else
			column++;
}

int check_type (void)
{
	NamedType *type;
	static Identifier *componentName = NULL;

	lexString = (char *) malloc (yyleng+1);
	memcpy (lexString, yytext, yyleng);
	lexString[yyleng] = 0;
	lexIdentifier = new Identifier (lexString, curComponent, curComponent->line);

	switch (tokenCount)
	{
		case 1:
			componentName = lexIdentifier;
			break;
		case 2:
			if (componentName->match (lexIdentifier))
			{
				lexSet = NULL;
				return TOK_SET_NAME;
			}
			break;
	}

	type = (NamedType *) curComponent->lookupType (lexIdentifier);
	if (!type)
		return TOK_IDENTIFIER;
	switch (type->which)
	{
		case Type::TYPE_CLASS:
			lexSet = (ClassType *) type;
			if (!curComponent->isInBlock ())
				return TOK_SET_NAME;
			if (curComponent->rm.flIsType (lexIdentifier))
				return TOK_IDENTIFIER;
			return TOK_SET_NAME;
			/*
			if (!lexSet->isInternal)
				return TOK_SET_NAME;
			it = (InternalType *) lexSet;
			if (!it->base)
				return TOK_SET_NAME;
			return TOK_IDENTIFIER;
			*/
		case Type::TYPE_TYPEDEF:
		case Type::TYPE_ENUMERATION:
			lexType = type;
			return TOK_TYPE;
		default:
			return TOK_IDENTIFIER;
	}
}

void yyerror(char const *s)
{
	char *token;

	token = (char *) malloc (yyleng + 1);
	memcpy (token, yytext, yyleng);
	token[yyleng] = 0;

	fprintf (stderr, "[line: %d][token: %s]. syntax error\n", curComponent->line, token);
}

