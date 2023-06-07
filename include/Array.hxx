
#ifndef __ARRAY__
#define __ARRAY__

#define assert(a,msg)	if (!a) fprintf (stderr, msg), fprintf (stderr, "\n"), exit (1)

template<class T>
class Array
{
	public:
		int count;
		T **array;

		Array ()
		{
			count = 0;
			array = NULL;
		}

		~Array ()
		{
			/*
			FIXME: this is strange.
			if (array)
				free (array);
				*/
		}

		void add (T *item)
		{
			T **newArray;

			newArray = (T **) realloc (array, (count + 1) * sizeof (T *));
			assert (newArray, "memory allocation failed");
			array = newArray;
			array[count++] = item;
		}

		void remove (T *item)
		{
			int i;

			for (i = 0; i < count; ++i)
				if (array[i] == item)
				{
					remove (i);
					return;
				}
		}

		void remove (int index)
		{
			if (index >= count)
				return;
			memmove (array + index, array + index + 1, (count - index - 1) * sizeof (T *));
			count--;
		}

		bool find (T *item)
		{
			int i;

			for (i = 0; i < count; ++i)
				if (array[i] == item)
					return true;
			return false;
		}

		void addFront (T *item)
		{
			T **newArray;
			int i;

			newArray = (T **) realloc (array, (count + 1) * sizeof (T *));
			assert (newArray, "memory allocation failed");
			array = newArray;
			for (i = count; i > 0; i--)
			{
				array[i] = array[i - 1];
			}
			array[0] = item;
			count++;
		}

		void removeFront ()
		{
			if (count > 0)
			{
				int i;

				count--;
				for (i = 0; i < count; ++i)
					array[i] = array[i + 1];
			}
		}

		void append (Array<T> *a)
		{
			int i;
			T **newArray;

			if (a == this)
			{
				fprintf (stderr, "Cannot an array to itself\n");
				return;
			}
			if (a->count <= 0)
				return;
			newArray = (T **) realloc (array, (count + a->count) * sizeof (T *));
			assert (newArray, "memory allocation failed");

			array = newArray;
			for (i = 0; i < a->count; )
				newArray[count++] = a->array[i++];
		}

		void reverse ()
		{
			int low, high;
			T *tmp;

			low = 0;
			high = count - 1;
			for ( ; low < high; low++, high--)
			{
				tmp = array[low];
				array[low] = array[high];
				array[high] = tmp;
			}
		}
		/*
		T *lookup (T *t)
		{
			int i;

			for (i = 0; i < count; ++i)
				if (array[i]->match (t))
					return array[i];
			return NULL;
		}
		*/
		T *lookup (Identifier *id)
		{
			int i;

			for (i = 0; i < count; ++i)
				if (array[i]->match (id))
					return array[i];
			return NULL;
		}
};

template<class T>
class UniqueArray
{
		bool (*matchim) (T *t1, T *t2);

	public:
		Array<T> array;

		UniqueArray (bool (*matchim) (T *t1, T *t2))
		{
			this->matchim = matchim;
		}

		UniqueArray ()
		{
			matchim = NULL;
		}

		T *match (T *item)
		{
			int i;

			if (matchim)
			{
			}
			else
			{
				for (i = 0; i < array.count; ++i)
					if (array.array[i]->match (item))
						return array.array[i];
			}
			return NULL;
		}

		T *lookup (Identifier *identifier)
		{
			int i;

			if (matchim)
			{
			}
			else
			{
				for (i = 0; i < array.count; ++i)
					if (array.array[i]->match (identifier))
						return array.array[i];
			}
			return NULL;
		}

		bool add (T *item)
		{
			if (matchim)
			{
			}
			else if (match (item))
				return false;
			array.add (item);
			return true;
		}
		void addNoCheck (T *item)
		{
			array.add (item);
		}
};

#endif

