/*
 *   surf - visualizing algebraic curves and algebraic surfaces
 *   Copyright (C) 1996-1997 Friedrich-Alexander-Universitaet
 *                           Erlangen-Nuernberg
 *                 1997-2001 Johannes Gutenberg-Universitaet Mainz
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





#ifndef INDEX_CL
#define INDEX_CL

#include <stdio.h>

#include "simple.h"

// class for a multinomial index with ++ for running up

class Index
{
private:
	Index(const Index &);
	void operator=(const Index &);

public:
	Index( int n, int s )
		{
			list = new int [ Number=n ];
			for( int i = 0; i < n-1; i++ )
				list[i] = 0;
			max = list[Number-1] = s;
		}

	~Index()
		{
			delete [] list;
		}
	// operator increment
	int operator++()
		{
			if ( list[Number-1] > 0 ) {
				list[Number-1]--;
				list[Number-2]++;
				return 1;
			}
			int j = Number-1;
			while( list[--j]==0 )
				;
			if( j>0 ) {
				list[j-1]++;
				list[Number-1]=list[j]-1;
				list[j]=0;
				return 1;
			}
			return 0;
		}

	// return multicoeff from simple.c
	double Coeff()
		{ return multinom_coeff( max, list, Number ); }

	int Element( int i )
		{ return list[i]; }

	// printout for debugging
	void Print()
		{
			for( int i = 0; i < Number; i++ )
				printf(" %d,",list[i]);
			printf("\n");
		}

private:
	int Number;
	int max;
	int *list;

};


#endif
