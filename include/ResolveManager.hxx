
#ifndef __RESOLVE_MANAGER__
#define __RESOLVE_MANAGER__

class ExpressionResolve
{
	public:
		Cast castPathx[1];
};

class ClassResolve
{
		ClassType *ct;
	public:
		ClassSpace ns;
		Mapper mapper;

		ClassResolve (ClassType *ct);
};

class ComponentResolve: public FieldList, public ComponentSpace
{
	public:
		bool addAttribute (Declaration *attrib);
};

#endif

