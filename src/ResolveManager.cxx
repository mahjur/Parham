
#include "parham.hxx"

bool ComponentResolve::addAttribute (Declaration *attrib)
{
	if (!attrib->identifier)
		addExtension ((InstanceDeclaration *) attrib);
	else if (!addSimple (attrib))
	{
		attrib->identifier->erDuplicate ();
		return false;
	}
	return true;
}

ClassResolve::ClassResolve (ClassType *ct)
	: mapper (ct)
{
	this->ct = ct;
}

