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



#ifndef POLYX_H
#define POLYX_H

#include<iostream>

/*****************************************************************************/
/*  POLYNIMIALS IN X                                                         */
/*****************************************************************************/

/* ------------------------------------------------------------------------- */
/*                                                                           */
/* Define some struct for:                                                   */
/*                 n-1                                                       */
/*                 ---                                                       */
/*                 \          k                                              */
/*                  >   a[k]*x                                               */
/*                 /                                                         */
/*                 ---                                                       */
/*                 k=0                                                       */
/*                                                                           */
/* ------------------------------------------------------------------------- */


class polyx 
{
private:
	void operator= (const polyx &); 
	polyx (const polyx &p);

public:
	polyx()		{n=0; a=0;};
	~polyx()	{delete a;}

	void allocCoeff (int size);

	double horner (double x) const;		// evaluate at x with horners method
	double dx_horner (double x) const;	// evaluate derivative at x with horners method

	
	void fastDerive (polyx &derivative) const;

public:
	int     n;
	double  *a;
};

std::ostream& operator << (std::ostream& os, const polyx& p);

#endif
