
#include "parham.hxx"

Mapper::Mapper (ClassType *_target)
{
	INIT (target);
}

void Mapper::init (MapDeclaration *map)
{
	target = map->container->rm.mapper.target;
	pathx.add (map);
	pathx.append (&map->container->rm.mapper.pathx);
}

void Mapper::publish (FILE *f, int index)
{
	int i;
	Declaration *prev;

	prev = NULL;
	for (i = index; i < pathx.count; ++i)
	{
		if (prev)
			fprintf (f, "->");
		prev = pathx.array[i];
		fprintf (f, "%s", prev->identifier->value);
	}
}

int Mapper::match (Array<Declaration> *dest)
{
	int i;

	if (!dest)
		return 0;

	for (i = 0; ; ++i)
	{
		if (i >= dest->count)
			return i;
		if (i >= pathx.count)
			return i;
		if (pathx.array[i] != dest->array[i])
			return i;
	}
}

