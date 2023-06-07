
#ifndef __MAPPER__
#define __MAPPER__

class Mapper
{
	public:
		ClassType *target;
		Array<MapDeclaration> pathx;

		Mapper (ClassType *_target);
		void init (MapDeclaration *map);
		void publish (FILE *f, int index);
		int match (Array<Declaration> *dest);
};

#endif

