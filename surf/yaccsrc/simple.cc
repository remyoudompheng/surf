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





/* ------------------------------------------------------------------------- */
/* simple.c                                                                  */
/* Author:   Stephan Endrass                                                 */
/* Address:  endrass@mi.uni-erlangen.de                                      */
/* Date:     14.8.94                                                         */
/* ------------------------------------------------------------------------- */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "simple.h"
#include "mymemory.h"


static int     **new_int_p( int n );
static int     **renew_int_p( int **i,int n );

double  epsilon = 1e-5;


/* ------------------------------------------------------------------------- */
/*  Here we store binomeal coefficients                                      */
/* ------------------------------------------------------------------------- */

typedef struct
{
	int     created;
	int     **data;
} binom_struct;


/* ------------------------------------------------------------------------- */
/* compare two double's                                                      */
/* ------------------------------------------------------------------------- */

int     double_equal( double s,double t )
{
	return  fabs( s - t ) < epsilon;
}


/* ------------------------------------------------------------------------- */
/* binomeal coefficient:                                                     */
/*                                                                           */
/*     / n \   / n-1 \   / n-1 \                                             */
/*     |   | = |     | + |     |                                             */
/*     \ k /   \ k-1 /   \  k  /                                             */
/*                                                                           */
/* ------------------------------------------------------------------------- */

static  binom_struct    b_c =
{
	-1,
	(int**)NULL
};

static  void    binom_struct_create_row( int n )
{
	if( n == 0 ) {
		b_c.data[0][0] = 1;
	} else {
		int     i;
		int     j;
		int     k = n - 1;
		
		b_c.data[n][0] = 1;
		
		for( i = 1, j = 0; i < n; i++, j++ ) {
			b_c.data[n][i] =
				b_c.data[k][i] + b_c.data[k][j];
		}

		b_c.data[n][n] = 1;
	}
}

static  void    binom_struct_add( int n )
{
	int     i;

	b_c.data = renew_int_p( b_c.data,n + 1 );

	for( i = b_c.created +1; i <= n; i++ ) {
		b_c.data[i] = new int[i + 1];
		binom_struct_create_row( i );
	}

	b_c.created = n;
}
    
int     binom_coeff( int n,int k )
{
	if( n >= k && k >= 0 ) {
		if( n > b_c.created ) {
			binom_struct_add( n );
		}
		return  b_c.data[n][k];
	} else if( k > n && n >= 0 ) {
		return  0;
	} else {
		fprintf( stderr,"can't compute binomeal coefficient of %d and %d\n",n,k );
		exit( 1 );
	}
}


/* ------------------------------------------------------------------------- */
/* multinomial coefficient: see                                              */
/*     Donald E. Knuth: The Art of Computer Programming, Volume 1 /          */
/*     Fundamental Algorithms, Addison-Wesley 1973, 2nd edition, p. 64       */
/* for the recursion formula                                                 */
/* ------------------------------------------------------------------------- */

int     multinom_coeff( int n,int *index,int k )
{
	int     i;
	int     j;
	int     result = 1;
	int     up     = index[0];
	int     down   = 0;

	for( i = 0, j = 1; j < k; i++, j++ ) {
		up   += index[j];
		down += index[i];
    
		result *= binom_coeff( up,down );
	}
	return  result;
}



// static  char    ascii[26]="abcdefghijklmnopqrstuvwxyz";

int    itotexascii( int i,char *s,int l )
{
	if( i < 0 || l < 2 || s==(char*)NULL ) {
		return  0;
	} else {
		/*  find length of 26-adic number i  */
		
		int     foo    = i;
		int     length = ( foo==0 ? 1 : 0 );
		
		while( foo > 0 ) {
			foo /= 26;
			length++;
		}

		/*  test if s is long enough  */

		if( length >= l ) {
			return  0;
		}

		/*  terminate with \0  */

		s[length]='\0';

		while( length > 0 ) {
			length--;
// 			s[length]= ascii[i%26];
			s[length]= 'a'+(i%26);
			i = i/26;
		}

		return  1;
	}
}





static int     **new_int_p( int n )
{
	if( n > 0 ) {
		int     **i;
		i = (int**)malloc( n*sizeof(int*) );
		
		if( i == (int**)NULL ) {
			(void)fprintf( stderr,"can't allocate %d pointers to int\n",n );
			exit( 1 );
		}
		
		return  i;
	} else {
		(void)fprintf( stderr,"can't allocate %d pointers to int\n",n );
		exit( 1 );
	}
}


static int     **renew_int_p( int **i,int n )
{
	if( i == (int**)NULL ) {
		return  new_int_p( n );
	} else if( n > 0 ) {
		i = (int**)realloc( (void**)i,n*sizeof(int*) );
		
		if( i == (int**)NULL ) {
			(void)fprintf( stderr,"can't realloc %d pointers to int\n",n );
			exit( 1 );
		}
		
		return  i;
	} else {
		(void)fprintf( stderr,"can't realloc %d pointers to int\n",n );
		exit( 1 );
	}
}

/* ------------------------------------------------------------------------- */
/* end of file: simple.c                                                     */
/* ------------------------------------------------------------------------- */
