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



#include <math.h>

#include <NewClipSphereCentral.h>
#include <ScriptVar.h>

void NewClipSphereCentral::init()
{
	NewSimpleClip::init();

	DISCR = d1sq + cysq - rsq;
	A0    = cy*sz*d1;
	A1    = sz*r;
	A2    = d1sq - rsq;
	
	if( DISCR >= 0.0 ) {
		DISCR1 = sqrt( DISCR );
		
		B0 = ( A0 - A1*DISCR1 )/A2;
		B1 = ( A0 + A1*DISCR1 )/A2;
	}

	a0 = cx*cy;
	a1 = cx*sz*d1;
	a2 = d1sq + cxsq + cysq - rsq;
	a3 = rsq - d1sq; 
	a4 = 2*cy*sz*d1;
	a5 = szsq*( rsq - cysq );
	a6 = d1sq + cysq - rsq;
	
	
	b0 = -( d1sq + cysq - rsq );
	b1 = 2*cx*cy;
	b2 = 2*sz*cx*d1;
	b3 = -( d1sq + cxsq - rsq );
	b4 = 2*sz*cy*d1;
	b5 = szsq*( rsq - cxsq - cysq );
}

bool NewClipSphereCentral::clip_user_y(double uy)
{
	if ( DISCR<0 || (uy>=B0 && uy<=B1)) {

		b6 = uy*uy;
		b7 = b1*uy + b2;
		b8 = b3*b6 + b4*uy + b5;

		discr1 = a2*( a3*b6 + a4*uy + a5 );
	
		if( discr1 >= 0.0 ) {
			discr2 = sqrt( discr1 );
			a7 = a0*uy + a1;
			a8 = ( a7 - discr2 )/a6;
			a9 = ( a7 + discr2 )/a6;
		}
	
		return true;

	}


	return false;
}

bool NewClipSphereCentral::clip_user_xy (double ux, double uy, double &zmin, double &zmax)
{
	if( discr1 >= 0.0 ) {
		if( ux < a8 || ux > a9 ) {
			return  false;
		}
	} else {
		return false;
	}

	double  uxsq = ux*ux;
	double  discr_b = b0*uxsq + ux*b7 + b8;
	discr_b = sqrt( discr_b );
	
	double  a = uxsq - ux*cx + b6 - uy*cy + sz*cz;
	double  b = sz/( uxsq + b6 + szsq );
	
	zmin = ( a - discr_b )*b;
	zmax = ( a + discr_b )*b;
	zmin = max( zmin,cb  );
	zmax = min( zmax,cf );
	
	return true;
}

bool NewClipSphereCentral::clip_user_xyz (double uz)
{
    return true;
}
