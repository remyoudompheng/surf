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




#include <math.h>
#include <stdio.h>


#include "def.h"
#include "float_buffer.h"
#include "Thread.h"
#include "stop.h"

// ----------------------------------------------------------------------------
// --------- constructors for old float_buffer --------------------------------
// ----------------------------------------------------------------------------

float_buffer::float_buffer()
	: b( 0 ),
	  n(0),
	  width(0),
	  height(0)
{
}

float_buffer::float_buffer( int w, int h )
	: b(new float [w*h] ),
	  n(w*h),
	  width(w),
	  height(h)
{
}

// ----------------------------------------------------------------------------
// ----------- release memory -------------------------------------------------
// ----------------------------------------------------------------------------

float_buffer::~float_buffer()
{
	delete [] b;
}

// ----------------------------------------------------------------------------
// ------------ change size of buffer -----------------------------------------
// ----------------------------------------------------------------------------

void float_buffer::Realloc( int w, int h )
{
	if (width==w && height == h)
		return;

	delete [] b;
	b = new float [ w * h ];
	n = w * h;
	width = w;
	height = h;
	*this=-10.0; //FIXME
}

// ----------------------------------------------------------------------------
// ------------ set the whole buffer equal f ----------------------------------
// ----------------------------------------------------------------------------

float_buffer& float_buffer::operator=( float f )
{
	for( int i = 0; i < n; i++ )
		b[i] = f;
	return *this;
}

// ----------------------------------------------------------------------------
// ------------ set the whole buffer equal zero -------------------------------
// ----------------------------------------------------------------------------

void float_buffer::NullInit()
{
	for( int i = 0; i < n; i++ )
		b[i] = 0.0;
}


// ----------------------------------------------------------------------------
// ------------ copy from a float buffer with maybe different size ------------
// ----------------------------------------------------------------------------

void float_buffer::Copy( const float_buffer& alt )
{
	int xm = min( width, alt.width );
	int ym = min( height, alt.height );
	for( int y = 0; y < ym; y++ )
		for( int x = 0; x < xm; x++ )
			b[y*width+x] = alt.Get( x, y );
}

// ----------------------------------------------------------------------------
// ------------ copy one line of a float buffer with maybe different size -----
// ----------------------------------------------------------------------------

void float_buffer::CopyLine( const float_buffer& alt, int src, int dest )
{
	if( dest >= 0 && dest < height ) {
		int xm = min( width, alt.width );
		for( int x = 0; x < xm; x++ )
			b[dest*width+x]=alt.Get(x,src);
	}
}


// ----------------------------------------------------------------------------
// -------- get 3 by 3 field in a small buffer --------------------------------
// -------- used in enhance edges ---------------------------------------------
// ----------------------------------------------------------------------------

float float_buffer::GetAbsFieldModuloHeight( int x, int y )
{
	if( x < 0 || x >= width - 2 || y < 0 )
		return 0.0;
	int l1 = (y%height) * width + x;
	int l2 = ((y+1)%height) * width + x;
	int l3 = ((y+2)%height) * width + x;
	return ( fabs(b[l1]) + fabs(b[l1+1]) + fabs(b[l1+2]) +
		 fabs(b[l2]) + fabs(b[l2+1]) + fabs(b[l2+2]) +
		 fabs(b[l3]) + fabs(b[l3+1]) + fabs(b[l3+2]) );
}

// ----------------------------------------------------------------------------
// ------- perform gamma correction on buffer ---------------------------------
// ----------------------------------------------------------------------------

void float_buffer::CorrectGamma (double gamma)
{
	for( int pos = 0; pos < n; pos++ )
		b[pos] = pow( b[pos], gamma );
}

// ----------------------------------------------------------------------------
// ------- perform edge enhancement on buffer ---------------------------------
// ------- routine fixed ----- uses only a width by 3 temp buffer -------------
// ----------------------------------------------------------------------------

void float_buffer::EnhanceEdges (double alpha)
{
	float_buffer  buffer( width, 3 );
  
	buffer.Copy( *this );

	for( int y = 0; y < height - 2 && !stop; y++ ) {
		int pos = y*width;
		for( int x = 0; x < width - 2; x++, pos++ ) {
			// calculate new value from data in temp buffer
			float err = buffer.GetAbsFieldModuloHeight( x, y ) / 9.0;
			float result = ( buffer.Get( x+1, (y+1)%3 ) - alpha * err )
				/ ( 1.0 - alpha );
			// do min and max range clipping
			if( result < 0.0 )
				b[pos] = 0.0;
			else if ( result < 1.0 )
				b[pos] = result;
			else
				b[pos] = 1.0;
		}
		// insert next line at obsolete line in temp buffer
		if( y < height - 3 )
			buffer.CopyLine( *this, y+3, y%3 );
	}
}
  
// ----------------------------------------------------------------------------
// ------ do tone scale adjustment --------------------------------------------
// ----------------------------------------------------------------------------

void float_buffer::AdjustToneScale( )
{
	float   value;

	for( int pos = 0; pos < n; pos++ ) {
		value = b[pos];

		if( value < 0.1 ) {
			b[pos] = 0.0;
		} else if( value < 0.85 ) {
			b[pos] = ( value - 0.1 )/0.8;
		} else {
			b[pos] = 1.0;
		}
	}
}
