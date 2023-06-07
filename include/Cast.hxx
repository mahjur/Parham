
#ifndef __CAST__
#define __CAST__

class CastItem
{
	public:
		int direction;
		PropertyDeclaration *property;
		CastItem (int _direction, PropertyDeclaration *_property);
		Type *getType ();
};

class CastObject
{
		Type *castUp (PropertyDeclaration *);

		int publishPreUp (FILE *f, int begin);
		void publishPreDown (FILE *f, int index);
		int publishPostUp (FILE *f, int begin);
		void publishPostDown (FILE *f, int index, bool hasPrev);

	public:
		Array<CastItem> path;

		void cast (int direction, PropertyDeclaration *property);
		void getup ();
		PropertyDeclaration *getLast ();
		bool isEmpty (bool withLast);

		void publishPre (FILE *f, bool complete);
		void publishPost (FILE *f, bool complete, bool hasPrev);
		Type *getType ();
};

class Cast
{
		void cast (int direction, PropertyDeclaration *property);
		void castDown (PropertyDeclaration *property);
		void getup ();
	public:
		Array<CastObject> object;

		void castUp (PropertyDeclaration *property);
		Type *follow (PropertyDeclaration *);
		PropertyDeclaration *getLast ();
		void publish (FILE *f, bool complete, Expression *expression);
		void getdown (Mapper *);
		void getup (Mapper *);
};

#endif

