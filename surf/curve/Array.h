/*
 *   surf - visualizing algebraic curves and algebraic surfaces
 *   Copyright (C) 1996-1997 Friedrich-Alexander-Universitaet
 *                           Erlangen-Nuernberg
 *                 1997-2000 Johannes Gutenberg-Universitaet Mainz
 *   Authors: Stephan Endrass, Hans Huelf, Ruediger Oertel,
 *            Kai Schneider, Ralf Schmitt, Johannes Beigel
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */


#ifndef ARRAY_H
#define ARRAY_H

// #include <assert.h>


#include "defs.h"

template<class Type> class Array
{
public:
	Array();
	Array(const Array &array);

	~Array();
	Array & operator = (const Array &);

	void setSize (int newSize);

	Type&       getElement (int n);
	const Type& getElement (int n) const;

	Type&       operator [] (int n)       {return getElement(n);};
	const Type& operator [] (int n) const {return getElement(n);};
	
	int getSize() const {return size;};

	void swapContentsWith (Array<Type> &arr)  {swap (size, arr.size); swap(elems, arr.elems);};
	
	// friend void swap (Array<Type> &arr1, Array<Type> &arr2);

protected:
	int size;
	Type *elems;
};

template<class Type> void swap (Array<Type> &arr1, Array<Type> &arr2)
{
	arr1->swapContentsWith (arr2);
}


template<class Type> Array<Type>::Array()
{
	size = 0;
	elems = 0;
}

template<class Type> Array<Type>::~Array()
{
	if (elems)
		delete [] elems;
}

template<class Type> Array<Type>::Array(const Array &array)
{
	elems = 0;
	setSize(array.size);

	int i;
	for (i=0; i<size; i++)
		elems[i] = array.elems[i];
}

template<class Type> Array<Type>& Array<Type>::operator = (const Array &array)
{
	setSize(array.size);
	int i;
	for (i=0; i<size; i++)
	        elems[i] = array.elems[i];
	return *this;
}

template<class Type> void Array<Type>::setSize (int newSize)
{
	delete[]elems;
	size = newSize;
	elems = new Type[size];
}

template<class Type> Type & Array<Type>::getElement(int n)
{
	assert(n>=0);
	assert(n<size);
	return elems[n];
}

template<class Type> const Type & Array<Type>::getElement(int n) const
{
	assert(n>=0);
	assert(n<size);
	return elems[n];
}
#endif
