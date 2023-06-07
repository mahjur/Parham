
#include "parham.hxx"

#define CAST_DOWN	1
#define CAST_UP		2

CastItem::CastItem (int _direction, PropertyDeclaration *_property)
{
	INIT (direction);
	INIT (property);

#ifdef DEBUG
	if (direction == CAST_DOWN)
		fprintf (stderr, "Cast Down (");
	else
		fprintf (stderr, "Cast Up   (");
	if (property)
	{
		if (property->container && property->container->pname)
			fprintf (stderr, "container=%s, ", property->container->pname);
		if (property->identifier && property->identifier->value)
			fprintf (stderr, "name=%s, ", property->identifier->value);
		if (property->type)
		{
			fprintf (stderr, "type->which=%d, ", property->type->which);
			if (property->type->which == TYPE_SET)
			{
				ClassType *st = (ClassType *)property->type;
				if (st->pname)
					fprintf (stderr, "type=%s, ", st->pname);
			}
		}
		fprintf (stderr, "\n");
	}
#endif
}

Type *CastItem::getType ()
{
	switch (direction)
	{
		case CAST_UP:
			return property->container;
		default:
			return property->type;
	}
}

void CastObject::cast (int direction, PropertyDeclaration *property)
{
	if ((path.count > 0) && (path.array[path.count - 1]->direction != direction) && (path.array[path.count - 1]->property == property))
		path.count--;
	else
		path.add (new CastItem (direction, property));
}

Type *CastObject::castUp (PropertyDeclaration *property)
{
	CastItem *ci;

	path.add (ci = new CastItem (CAST_UP, property));
	return ci->getType ();
}

void CastObject::getup ()
{
	int i;
	CastItem *ci;
	Array<MapDeclaration> *mapPath;

	if (path.count > 0)
	{
		ci = path.array[path.count - 1];
		if (ci->property->type->which == Type::TYPE_CLASS)
		{
			mapPath = &((ClassType *) ci->property->type)->rm.mapper.pathx;
	//		for (i = mapPath->count; i-- > 0; )
			for (i = 0; i < mapPath->count; i++)
				castUp (mapPath->array[i]);
		}
		else
		{
			// FIXME: In this case, it is an association to an array
		}
	}
}

PropertyDeclaration *CastObject::getLast ()
{
	if (path.count <= 0)
		return NULL;
	return path.array[path.count - 1]->property;
}

bool CastObject::isEmpty (bool withLast)
{
	if (path.count <= 0)
		return true;
	if (path.count == 1)
		return !withLast;
	return false;
}

void CastObject::publishPre (FILE *f, bool complete)
{
	int i;
	int count;

	count = path.count;
	if (!complete)
		count--;

	for (i = 0; i < count; ++i)
		if (path.array[i]->direction == CAST_UP)
			i = publishPreUp (f, i);
}

void CastObject::publishPost (FILE *f, bool complete, bool hasPrev)
{
	int i;
	int count;

	count = path.count;
	if (!complete)
		count--;

	for (i = 0; i < count; ++i)
		if (path.array[i]->direction == CAST_UP)
			i = publishPostUp (f, i);
		else
			publishPostDown (f, i, hasPrev || i);
}

int CastObject::publishPreUp (FILE *f, int begin)
{
	int last;
	int i;
	ClassType *sx;

	for (last = begin; last < path.count; ++last)
	{
		if (path.array[last]->direction != CAST_UP)
			break;
	}
	--last;

	sx = (ClassType *) path.array[last]->property->container;
#ifdef DEBUG
	fprintf (stderr, "publish castUP (%s::%s)\n", sx->pname, sx->identifier->value);
#endif

	if (sx->modifier->has (MOD_EXTERN))
		fprintf (f, "PARHAMUP (%s, ", sx->pname);
	else
		fprintf (f, "PARHAMUP (struct %s, ", sx->pname);

	for (i = last; i >= begin; --i)
	{
#ifdef DEBUG
	fprintf (stderr, "\t(%s::%s)\n", path.array[i]->property->container->pname, path.array[i]->property->pname);
#endif
		fprintf (f, "%s", path.array[i]->property->pname);
		if(i > begin)
			fprintf (f, "->");
	}

	/*
	if (path.array[begin]->property->container->modifier->has (MOD_FOREIGN))
		fprintf (f, "PARHAMUP (%s, ", path.array[begin]->property->container->pname);
	else
		fprintf (f, "PARHAMUP (struct %s, ", path.array[begin]->property->container->pname);

	for ( ; begin <= last; ++begin)
	{
		fprintf (f, "%s", path.array[begin]->property->pname);
		if (begin < last)
			fprintf (f, "->");
	}
	*/
	fprintf (f, ", ");
	return last;
}

int CastObject::publishPostUp (FILE *f, int begin)
{
	int last;

	for (last = begin; last < path.count; ++last)
	{
		if (path.array[last]->direction != CAST_UP)
			break;
	}
	--last;

	fprintf (f, ")");
	return last;
}

void CastObject::publishPostDown (FILE *f, int index, bool hasPrev)
{
	if (hasPrev > 0)
		fprintf (f, "->%s", path.array[index]->property->pname);
	else
		fprintf (f, "%s", path.array[index]->property->pname);
}

Type *CastObject::getType ()
{
	if (path.count <= 0)
		return NULL;
	return path.array[path.count - 1]->getType ();
}

void Cast::cast (int direction, PropertyDeclaration *property)
{
	if (object.count == 0)
		object.add (new CastObject ());
	object.array[object.count - 1]->cast (direction, property);
}

void Cast::castUp (PropertyDeclaration *property)
{
#ifdef DEBUG
//	fprintf (stderr, "castUp (%s: %s => %s)\n", property->pname, ((ClassType *)(property->type))->pname, property->container? property->container->pname: "null");
	fprintf (stderr, "castUp (%s)\n", property->pname);
#endif
	cast (CAST_UP, property);
}

void Cast::castDown (PropertyDeclaration *property)
{
#ifdef DEBUG
//	fprintf (stderr, "castDown (%s: %s => %s)\n", property->pname, property->container? property->container->pname: "null", ((ClassType *)(property->type))->pname);
	fprintf (stderr, "castDown (%s)\n", property->pname);
#endif
	cast (CAST_DOWN, property);
}

Type *Cast::follow (PropertyDeclaration *property)
{
	switch (property->which)
	{
		case Declaration::DECL_ASSOCIATION:
			object.add (new CastObject ());
			castDown (property);
			getup ();
			break;
		default:
			castDown (property);
	}
	return object.array[object.count - 1]->getType ();
}

void Cast::getup ()
{
	if (object.count > 0)
		object.array[object.count - 1]->getup ();

	/*
	if (object.count <= 0)
		return;
	co = object.array[object.count - 1];
	if (co->path.count == 0)
		return;
	ci = co->path.array[co->path.count - 1];
	mapPath = &((ClassType *) ci->property->type)->mapper.path;
	for (i = mapPath->count; i-- > 0; )
		castUp (mapPath->array[i]);
	*/
}

PropertyDeclaration *Cast::getLast ()
{
	if (object.count <= 0)
		return NULL;
	return object.array[object.count - 1]->getLast ();
}

void Cast::publish (FILE *f, bool complete, Expression *expression)
{
	int i;
	bool hasPrev;

	if (object.count <= 0)
	{
		expression->publish2 (f);
		return;
	}

	for (i = 0; i < object.count - 1; ++i)
		object.array[i]->publishPre (f, true);
	object.array[object.count - 1]->publishPre (f, complete);

	hasPrev = false;
	if (expression->needPublish ())
	{
		expression->publish2 (f);
		hasPrev = true;
//		fprintf (f, "->");
	}

	for (i = 0; i < object.count - 1; ++i)
		object.array[i]->publishPost (f, true, hasPrev || i);
	object.array[object.count - 1]->publishPost (f, complete, hasPrev || (object.count - 1));
}

/*
Type *Cast::getTypex ()
{
	if (object.count <= 0)
		return NULL;
	return object.array[object.count - 1]->getType ();
}
*/

void Cast::getdown (Mapper *mapper)
{
	int i;

//	for (i = 0; i < mapper->path.count; ++i)
	for (i = mapper->pathx.count; i-- > 0; )
		castDown (mapper->pathx.array[i]);
}

void Cast::getup (Mapper *mapper)
{
	int i;

	for (i = 0; i < mapper->pathx.count; ++i)
		castUp (mapper->pathx.array[i]);
}

