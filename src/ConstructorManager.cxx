
#include "parham.hxx"

// PropertyConstructor
PropertyConstructor::PropertyConstructor ()
{
	counter = 0;
}

int PropertyConstructor::incCounter ()
{
	return ++counter;
}

// ClassConstructor
ClassConstructor::ClassConstructor (ClassType *ct)
{
	this->ct = ct;
}

void ClassConstructor::initCounters ()
{
	int i;
	Array<PropertyDeclaration> *pd;

	pd = &ct->attributeList;
	for (i = 0; i < pd->count; ++i)
		pd->array[i]->cm.counter = 0;
}

void ClassConstructor::checkCounters (ConstructorDeclaration *caller)
{
	// now, I check that for every AttributeDeclaration whose type is internal class, SuperDeclaration and MapDeclaration a constructor is called.
	// If there is no one, I will try to add a default one
	// otherwise, an error is raised.

	int i;
	ExpressionList argList (ct->component, ct->line);
	ClassType *set;
	ConstructorDeclaration *cd;
	ConstructorCallImplicit *cci;
	PropertyDeclaration *prop;

	for (i = 0; i < ct->attributeList.count; ++i)
	{
		prop = ct->attributeList.array[i];
		if (prop->cm.counter != 0)
			continue;
		switch (prop->which)
		{
			case Declaration::DECL_ATTRIBUTE:
				if (prop->type->which != Type::TYPE_CLASS)
					continue;
//				if (prop->isRef)
//					continue;
			case Declaration::DECL_MAP:
			case Declaration::DECL_SUPER:
				if (prop->modifier->has (MOD_REQUIRES))
					continue;
				set = (ClassType *) prop->type;
				cd = set->cm.findConstructor (&argList);
				if (cd)
				{
					cci = new ConstructorCallImplicit (ct->component, ct->line);
					cci->makeCall (prop, cd);
					caller->addConstructorCall (cci);
				}
				else
					ct->error ("missing constructor for %s on set %s", ct->identifier->value, set->pname);
				break;
			default:
				break;
		}
	}
}

ConstructorDeclaration *ClassConstructor::findConstructor (ExpressionList *argList)
{
	return (ConstructorDeclaration *) constructorList.lookup (argList);
}

void ClassConstructor::publishHeader (FILE *f)
{
	int i;

	for (i = 0; i < constructorList.list.count; ++i)
		constructorList.list.array[i]->publishHeader (f);
}

void ClassConstructor::publishSource (FILE *f)
{
	int i;

	for (i = 0; i < constructorList.list.count; ++i)
		constructorList.list.array[i]->publishSource (f);
}

int ClassConstructor::add (ConstructorDeclaration *constructor)
{
	return constructorList.add (constructor);
}

