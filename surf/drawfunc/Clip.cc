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




#include <stdio.h>
#include <math.h>
#include <stdlib.h>


#include <def.h>
#include <simple.h>
#include <monomarith.h>
#include <polyarith.h>
#include <hornerarith.h>
#include <Vector.h>
#include <roots.h>
#include <polylexyacc.h>
#include <color.h>

#include <Vector.h>
#include <Clip.h>
#include <MultiVariatePolynom.h>
#include <MappingMatrix.h>
#include <Position.h>

// FIXME!!! These values are different from the clip constants
//  defined in ScriptVar!
#define SPHERE       0
#define CYLH         1
#define CYLV         2
#define CYLZ         3
#define CUBE         4
#define NONE         8 // changed from 5 to 8 my jojo

// ----------------------------------------------------------------------------
// ---------------- constructor for general base class ------------------------
// ----------------------------------------------------------------------------

Clip::Clip( const int cm,
	    const clip_numeric_t &ClipData,
	    const double mi, const double mx,
	    const double y1, const double y2 )
	: ClipModus(cm),
	  Front(ClipData.clip_front),                       // maximum z value
	  Back(ClipData.clip_back),                        // minimum z value
	  Radius(ClipData.radius),
	  RadQuad(ClipData.radius*ClipData.radius)
{
	Center[0] = ClipData.center_x;     // centerx, ..y, ..z
	Center[1] = ClipData.center_y;
	Center[2] = ClipData.center_z;
	WinMin[0] = mi;              // uxmin, uxmax, uymin, uymax
	WinMax[0] = mx;
	WinMin[1] = min( y1,y2 );
	WinMax[1] = max( y1,y2 );
}

// ----------------------------------------------------------------------------
// ---------------- constructor for subclass central --------------------------
// ---------------- base class for all central clippings ----------------------
// ----------------------------------------------------------------------------

ClipCentral::ClipCentral( const int cm,
			  const clip_numeric_t &ClipData,
			  const double mi, const double mx,
			  const double y1, const double y2,
			  const double specz ) 
	: Clip( cm , ClipData, mi, mx, y1, y2 ),
	  SpectatorZ(specz),
	  ZDistance(specz-ClipData.center_z),            // distance spec to centerz
	  ZDistSquare(ZDistance*ZDistance),
	  SpecZSquare(specz*specz)
{
	CenterSquare[0] = ClipData.center_x * ClipData.center_x;
	CenterSquare[1] = ClipData.center_y * ClipData.center_y;
	CenterSquare[2] = ClipData.center_z * ClipData.center_z;
}

// ----------------------------------------------------------------------------
// ------------ clipping for sphere -- projection parallel --------------------
// ----------------------------------------------------------------------------

ClipSphere::ClipSphere( const clip_numeric_t &ClipData,
			const double mi, const double mx,
			const double y1, const double y2 ) 
	: Clip( 0, ClipData, mi, mx, y1, y2 )
{;}

int ClipSphere::ClipXY( int dir, double uv, double &min, double &max )
{
	if( dir == 0 || dir == 1 ) {
		if( fabs( uv - Center[dir] ) > Radius ) {
			return false;
		}
		double h = sqrt( RadQuad - (uv - Center[dir])*(uv - Center[dir]) );
		min = Center[1-dir] - h;
		max = Center[1-dir] + h;
	}
	return true;
}

int ClipSphere::ClipXYZ( double ux, double uy, double &min, double &max )
{
	double h = ( ux - Center[0] ) * ( ux - Center[0] )
		+ ( uy - Center[1] ) * ( uy - Center[1] );
	if ( h > RadQuad ) {
		return false;
	}
	h = sqrt( RadQuad - h );
	min = max( Center[2] - h, Back );
	max = min( Center[2] + h, Front );
	return true;
}

// ----------------------------------------------------------------------------
// ----- clipping for horiz cylinder -- projection parallel -------------------
// ----------------------------------------------------------------------------

ClipCylH::ClipCylH( const clip_numeric_t& ClipData,
		    const double mi, const double mx,
		    const double y1, const double y2 ) 
	: Clip( 1, ClipData, mi, mx, y1, y2 )
{
}

int ClipCylH::ClipXY( int dir, double uv, double &min, double &max )
{
	if( dir == 0 ) {
		min = Center[1] - Radius;
		max = Center[1] + Radius;
	} else if( dir == 1 ) {
		if( fabs( uv - Center[1] ) > Radius ) {
			return false;
		}
		min = WinMin[0];
		max = WinMax[0];
	}
	return true;
}

int ClipCylH::ClipXYZ( double, double uy, double &min, double &max )
{
	double h = ( uy - Center[1] ) * ( uy - Center[1] );
	if ( h > RadQuad ) {
		return false;
	}
	h = sqrt( RadQuad - h );
	min = max( Center[2] - h, Back );
	max = min( Center[2] + h, Front );
	return true;
}

// ----------------------------------------------------------------------------
// ------ clipping for vert cylinder -- projection parallel -------------------
// ----------------------------------------------------------------------------

ClipCylV::ClipCylV( const clip_numeric_t &ClipData,
		    const double mi, const double mx,
		    const double y1, const double y2 ) 
	: Clip( 2, ClipData, mi, mx, y1, y2 )
{
}

int ClipCylV::ClipXY( int dir, double uv, double &min, double &max )
{
	if( dir == 0 ) {
		if( fabs( uv - Center[0] ) > Radius ) {
			return false;
		}
		min = WinMin[1];
		max = WinMax[1];
	} else if( dir == 1 ) {
		min = Center[0] - Radius;
		max = Center[0] + Radius;
	}
	return true;
}

int ClipCylV::ClipXYZ( double ux, double, double &min, double &max )
{
	double h = ( ux - Center[0] ) * ( ux - Center[0] );
	if ( h > RadQuad ) {
		return false;
	}
	h = sqrt( RadQuad - h );
	min = max( Center[2] - h, Back );
	max = min( Center[2] + h, Front );
	return true;
}

// ----------------------------------------------------------------------------
// --- clipping for cylinder in z direction -- projection parallel ------------
// ----------------------------------------------------------------------------

ClipCylZ::ClipCylZ( const clip_numeric_t &ClipData,
		    const double mi, const double mx,
		    const double y1, const double y2 ) 
	: Clip( 3, ClipData, mi, mx, y1, y2 )
{
}

int ClipCylZ::ClipXY( int dir, double uv, double &min, double &max )
{
	if( dir == 0 || dir == 1 ) {
		if( fabs( uv - Center[dir] ) > Radius ) {
			return false;
		}
		double h = sqrt( RadQuad - (uv - Center[dir])*(uv - Center[dir]) );
		min = Center[1-dir] - h;
		max = Center[1-dir] + h;
	}
	return true;
}

int ClipCylZ::ClipXYZ( double ux, double uy, double &min, double &max )
{
	double h = ( ux - Center[0] ) * ( ux - Center[0] )
		+ ( uy - Center[1] ) * ( uy - Center[1] );
	if ( h > RadQuad ) {
		return false;
	}
	min = Back;
	max = Front;
	return true;
}

// ----------------------------------------------------------------------------
// ------------- clipping for cube -- projection parallel ---------------------
// ----------------------------------------------------------------------------

ClipCube::ClipCube( const clip_numeric_t &ClipData,
		    const double mi, const double mx,
		    const double y1, const double y2,
		    Position* position ) 
  : Clip( 4, ClipData, mi, mx, y1, y2 ), CubeVec(new Vector[3])
{
	CubeVec[0].Set(1.0,0.0,0.0);
	CubeVec[1].Set(0.0,1.0,0.0);
	CubeVec[2].Set(0.0,0.0,1.0);
	Length[0] = - ( Center[0] + Radius );
	Length[1] = - ( Center[0] - Radius );
	Length[2] = - ( Center[1] + Radius );
	Length[3] = - ( Center[1] - Radius );
	Length[4] = - ( Center[2] + Radius );
	Length[5] = - ( Center[2] - Radius );
	for( int i = 0; i < 3; i++ )
		CubeVec[i] = position->Rotate( CubeVec[i] );
}

ClipCube::~ClipCube()
{
	delete [] CubeVec;
}

int ClipCube::ClipXY( int dir, double, double &min, double &max )
{
	if( dir == 0 || dir == 1 ) {
		min = WinMin[1-dir];
		max = WinMax[1-dir];
	}
	return true;
}

int ClipCube::ClipXYZ( double ux, double uy, double &min, double &max )
{
	min = Back;
	max = Front;
	for( int i = 0; i < 3; i ++ ) {
		double h1 = CubeVec[i].x * ux + CubeVec[i].y * uy;
		if( CubeVec[i].z != 0.0 ) {
			double h1 = CubeVec[i].x * ux + CubeVec[i].y * uy;
			double h2 = ( h1 + Length[2*i] ) / CubeVec[i].z;
			double h3 = ( h1 + Length[2*i+1] ) / CubeVec[i].z;
			min = max( min, min( h2, h3 ) );
			max = min( max, max( h2, h3 ) );
		} else {
			if( ( h1 + Length[2*i] ) * ( h1 + Length[2*i+1] ) > 0.0 ) {
				min = 1.0;
				max = -1.0;
			}
		}
	}
	return (min < max);
}

// ----------------------------------------------------------------------------
// ----------- clip only at window -- projection parallel ---------------------
// ----------------------------------------------------------------------------

ClipNone::ClipNone( const clip_numeric_t &ClipData,
		    const double mi, const double mx,
		    const double y1, const double y2 ) 
	: Clip( 5, ClipData, mi, mx, y1, y2 )
{
}

int ClipNone::ClipXY( int dir, double, double &min, double &max )
{
	if( dir == 0 || dir == 1 ) {
		min = WinMin[1-dir];
		max = WinMax[1-dir];
	}
	return true;
}

int ClipNone::ClipXYZ( double, double, double &min, double &max )
{
	min = Back;
	max = Front;
	return true;
}

// ----------------------------------------------------------------------------
// ------------ clipping for sphere -- projection central ---------------------
// ----------------------------------------------------------------------------

ClipSphereCentral::ClipSphereCentral( const clip_numeric_t &ClipData,
				      const double mi, const double mx,
				      const double y1, const double y2,
				      const double specz ) 
	: ClipCentral( 0, ClipData, mi, mx, y1, y2, specz ),
	  A(new double[4]),
	  B(new double[4]),
	  a(new double[6]),
	  Disc(new double[2])
{
	Disc[0] = ZDistSquare + CenterSquare[1] - RadQuad;
	Disc[1] = ZDistSquare + CenterSquare[0] - RadQuad;
	A[0] = Center[1] * SpectatorZ * ZDistance;
	A[1] = SpectatorZ * Radius;
	A[2] = ZDistSquare - RadQuad;
	A[3] = Center[0] * SpectatorZ * ZDistance;
	if( Disc[0] >= 0.0 ) {
		double h = sqrt(Disc[0]);
		B[2] = (A[0] - A[1] * h )/ A[2];
		B[3] = (A[0] + A[1] * h )/ A[2];
	}

	if( Disc[1] >= 0.0 ) {
		double h = sqrt(Disc[1]);
		B[0] = (A[3] - A[1] * h )/ A[2];
		B[1] = (A[3] + A[1] * h )/ A[2];
	}

	a[0] = Center[0] * Center[1];
	a[1] = A[3] + A[3];
	a[2] = CenterSquare[0] + CenterSquare[1] + A[2];
	a[3] = A[0] + A[0];
	a[4] = a[0] + a[0];
	a[5] = SpecZSquare * ( RadQuad - CenterSquare[0] - CenterSquare[1] );
}

ClipSphereCentral::~ClipSphereCentral()
{
	delete [] a;
	delete [] A;
	delete [] B;
	delete [] Disc;
}

int ClipSphereCentral::ClipXY( int dir, double uv, double &min, double &max )
{
	if( dir == 0 || dir == 1) {

		if( Disc[1-dir] < 0.0 ) {
			return false;
		}

		if( uv < B[dir+dir] || uv > B[dir+dir+1] ) {
			return false;
		}

		double uvsq = uv * uv;
		double discr = a[2] * ( -A[2] * uvsq + a[dir+dir+1] * uv + a[5] );

		if( discr < 0.0 ) {
			return false;
		}
      
		double hh = sqrt(discr);
		double h = a[0] * uv + A[dir+dir+dir];
		min = max( ( h - hh ) / Disc[1-dir], WinMin[1-dir] );
		max = min( ( h + hh ) / Disc[1-dir], WinMax[1-dir] );
	}
	return true;
}

int ClipSphereCentral::ClipXYZ( double ux, double uy,
				double &min, double &max )
{
	if( uy <= B[2] || uy >= B[3] ) {
		return false;
	}

	if( ux <= B[0] || ux >= B[1] ) {
		return false;
	}

	double uxsq = ux * ux;
	double uysq = uy * uy;
	double sqsum = uxsq + uysq;
	double h = -Disc[0] * uxsq - Disc[1] * uysq + ux * uy * a[4] 
		+ a[1] * ux + a[3] * uy + a[5];
	h = ( h < 0.0 ? 0.0 : sqrt(h) );
	
	double a = sqsum - ux * Center[0] - uy * Center[1] + SpectatorZ * Center[2];
	double aa = SpectatorZ / ( sqsum + SpecZSquare );
	
	min = max( (a-h)*aa, Back );
	max = min( (a+h)*aa, Front );
	return true;
}

// ----------------------------------------------------------------------------
// ------- clipping for horiz cylinder -- projection central ------------------
// ----------------------------------------------------------------------------

ClipCylHCentral::ClipCylHCentral( const clip_numeric_t &ClipData,
				  const double mi, const double mx,
				  const double y1, const double y2,
				  const double specz ) 
	: ClipCentral( 1, ClipData, mi, mx, y1, y2, specz ),
	  A(new double[4]),
	  B(new double[2]),
	  a(new double[5]),
	  Disc(new double[2])
{
	Disc[0] = ZDistSquare + CenterSquare[1] - RadQuad;
	A[0] = Center[1] * SpectatorZ * ZDistance;
	A[1] = SpectatorZ * Radius;
	A[2] = ZDistSquare - RadQuad;
	A[3] = Center[0] * SpectatorZ * ZDistance;
	if( Disc[0] >= 0.0 ) {
		Disc[1] = sqrt(Disc[0]);
		B[0] = (A[0] - A[1] * Disc[1])/ A[2];
		B[1] = (A[0] + A[1] * Disc[1])/ A[2];
	}
	a[0] = SpectatorZ;
	a[1] = -SpectatorZ * Center[1];
	a[2] = SpecZSquare * Center[2];
	a[3] = A[0] + A[0];
	a[4] = SpecZSquare * ( RadQuad - CenterSquare[1] );
}

ClipCylHCentral::~ClipCylHCentral()
{
	delete [] a;
	delete [] A;
	delete [] B;
	delete [] Disc;
}

int ClipCylHCentral::ClipXY( int dir, double uv, double &min, double &max )
{
	if( Disc[0] < 0.0 ) {
		return false;
	}
	if( dir == 0 ) {
		min = max( B[0], WinMin[1] );
		max = min( B[1], WinMax[1] );
	}      

	if( dir == 1 ) {
		if( uv < B[0] || uv > B[1] ) {
			return false;
		}
		min = WinMin[0];
		max = WinMax[0];
	}
	return true;
}

int ClipCylHCentral::ClipXYZ( double, double uy, double &min, double &max )
{
	if( uy < B[0] || uy > B[1] ) {
		return false;
	}
	double uysq = uy*uy;
	double discr = SpectatorZ * sqrt( -A[2] * uysq + a[3] * uy + a[4] );
	double h = a[0] + uysq + a[1] * uy + a[2];
	double hh = uysq + SpecZSquare;
	min = max((h-discr)/hh,Back);
	max = min((h+discr)/hh,Front);
	return true;
}

// ----------------------------------------------------------------------------
// -------- clipping for vert cylinder -- projection central ------------------
// ----------------------------------------------------------------------------

ClipCylVCentral::ClipCylVCentral( const clip_numeric_t &ClipData,
				  const double mi, const double mx,
				  const double y1, const double y2,
				  const double specz ) 
	: ClipCentral( 2, ClipData, mi, mx, y1, y2, specz ),
	  A(new double[4]),
	  B(new double[2]),
	  a(new double[5]),
	  Disc(new double[2])
{
	Disc[0] = ZDistSquare + CenterSquare[0] + RadQuad;
	A[0] = Center[0] * SpectatorZ * ZDistance;
	A[1] = SpectatorZ * Radius;
	A[2] = ZDistSquare - RadQuad;
	if( Disc[0] >= 0.0 ) {
		Disc[1] = sqrt( Disc[0] );
		B[0] = ( A[0] - A[1] * Disc[1] ) / A[2];
		B[1] = ( A[0] + A[1] * Disc[1] ) / A[2];
	}
	a[0] = SpectatorZ;
	a[1] = -SpectatorZ * Center[0];
	a[2] = SpecZSquare * Center[2];
	a[3] = A[0] + A[0];
	a[4] = SpecZSquare * ( RadQuad - CenterSquare[0] );
}

ClipCylVCentral::~ClipCylVCentral()
{
	delete [] a;
	delete [] A;
	delete [] B;
	delete [] Disc;
}

int ClipCylVCentral::ClipXY( int dir, double uv, double &min, double &max )
{
	if( Disc[0] < 0.0 ) {
		return false;
	}

	if( dir == 0 ) {
		if( uv < B[0] || uv > B[1] ) {
			return false;
		}
		min = WinMin[1];
		max = WinMax[1];
	}
	if( dir == 1 ) {
		min = max( B[0], WinMin[0] );
		max = min( B[1], WinMax[0] );
	}
	return true;
}

int ClipCylVCentral::ClipXYZ( double ux, double, double &min, double &max )
{
	if( ux < B[0] || ux > B[1] ) {
		return false;
	}
	double uxsq = ux * ux;
	double discr = -A[2] * uxsq + a[3] * ux + a[4];
	if( discr < 0.0 ) discr = 0.0;
	discr = sqrt(discr) * SpectatorZ;
	double h = a[0] * uxsq + a[1] * ux + a[2];
	double hh = uxsq * SpecZSquare;
	min = max( (h - discr)/hh, Back );
	max = min( (h + discr)/hh, Front );
	return true;
}

// ----------------------------------------------------------------------------
// ---- clipping for cylinder in z direc -- projection central ----------------
// ----------------------------------------------------------------------------


ClipCylZCentral::ClipCylZCentral( const clip_numeric_t &ClipData,
				  const double mi, const double mx,
				  const double y1, const double y2,
				  const double specz ) 
  : ClipCentral( 3, ClipData, mi, mx, y1, y2, specz )
{
}

int ClipCylZCentral::ClipXY( int dir, double, double &min, double &max )
{
	if( dir == 0 || dir == 1 ) {
		min = WinMin[1-dir];
		max = WinMax[1-dir];
	}
	return true;
}

int ClipCylZCentral::ClipXYZ( double, double, double &min, double &max )
{
	min = Back;
	max = Front;
	return true;
}

// ----------------------------------------------------------------------------
// ----------- clipping for cube -- projection central ------------------------
// ----------------------------------------------------------------------------


ClipCubeCentral::ClipCubeCentral( const clip_numeric_t &ClipData,
				  const double mi, const double mx,
				  const double y1, const double y2,
				  const double specz,
				  Position* position ) 
	: ClipCentral( 4, ClipData, mi, mx, y1, y2, specz ),
	  CubeVec(new Vector[3])
{
	CubeVec[0].Set(1.0,0.0,0.0);
	CubeVec[1].Set(0.0,1.0,0.0);
	CubeVec[2].Set(0.0,0.0,1.0);
	Length[0] = - ( Center[0] + Radius );
	Length[1] = - ( Center[0] - Radius );
	Length[2] = - ( Center[1] + Radius );
	Length[3] = - ( Center[1] - Radius );
	Length[4] = - ( Center[2] + Radius );
	Length[5] = - ( Center[2] - Radius );
	
	for( int i = 0; i < 3; i++ )
		CubeVec[i] = position->Rotate( CubeVec[i] );
}

ClipCubeCentral::~ClipCubeCentral()
{
	delete [] CubeVec;
}

int ClipCubeCentral::ClipXY( int dir, double, double &min, double &max )
{
	if( dir == 0 || dir == 1 ) {
		min = WinMin[1-dir];
		max = WinMax[1-dir];
	}
	return true;
}

int ClipCubeCentral::ClipXYZ( double ux, double uy, double &min, double &max )
{
	min = Back;
	max = Front;
	for( int i = 0; i < 3; i ++ ) {
		double h1 = CubeVec[i].x * ux + CubeVec[i].y * uy;
		double h = h1 - CubeVec[i].z * SpectatorZ;
		if( h != 0.0 ) {
			double h2 = SpectatorZ * ( h1 + Length[2*i] ) / h;
			double h3 = SpectatorZ * ( h1 + Length[2*i+1] ) / h;
			min = max( min, min( h2, h3 ) );
			max = min( max, max( h2, h3 ) );
		} else {
			if( ( h1 + Length[2*i] ) * ( h1 + Length[2*i+1] ) > 0.0 ) {
				min = 1.0;
				max = -1.0;
			}
		}
	}
	return (min < max);
}

//-----------------------------------------------------------------------------
//----------------- replacement for operator new ------------------------------
//---- static creator function --- decides which clip algorithm to use --------
//-----------------------------------------------------------------------------

Clip* Clip::create ( const int psp,            // perspective
		     const int cm,             // clip modus
		     const clip_numeric_t &cd,         // radii and center
		     const WindowGeometry &wingeo,       // to get borders
		     const double sz,                  // z of spectator
		     Position* position,
		     int xmin, int xmax, int ymin, int ymax)              // does rotation
{

// 	int xmin = 0;
// 	int xmax = 0;
// 	int ymin = 0;
// 	int ymax = 0;
	// FIXME
// 	mywindow->GetBorders( 0, xmin, xmax );
// 	mywindow->GetBorders( 1, ymin, ymax );
	double a = wingeo.pixelToUser( 0, xmin );
	double b = wingeo.pixelToUser( 0, xmax );
	double c = wingeo.pixelToUser( 1, ymin );
	double d = wingeo.pixelToUser( 1, ymax );
	if(!psp) {                                     // perspective parallel

		switch( cm ) {                             // clipmodus

		case SPHERE :
			return (::new ClipSphere(cd,a,b,c,d));

		case CYLH :
			return (::new ClipCylH(cd,a,b,c,d));

		case CYLV :
			return (::new ClipCylV(cd,a,b,c,d));

		case CYLZ :
			return (::new ClipCylZ(cd,a,b,c,d));

		case CUBE :
			return (::new ClipCube(cd,a,b,c,d, position));

		case NONE :
			return (::new ClipNone(cd,a,b,c,d));

		default :
			cerr << "trying to create unknown clipping mode. using none." << endl;
			return (::new ClipNone(cd,a,b,c,d));
		}
	} else {                                         // perspective central
		
		switch( cm ) {                             // clipmodus

		case SPHERE :
			return (::new ClipSphereCentral(cd,a,b,c,d,sz));

		case CYLH :
			return (::new ClipCylHCentral(cd,a,b,c,d,sz));

		case CYLV :
			return (::new ClipCylVCentral(cd,a,b,c,d,sz));

		case CYLZ :
			return (::new ClipCylZCentral(cd,a,b,c,d,sz));

		case CUBE :
			return (::new ClipCubeCentral(cd,a,b,c,d,sz, position));

		case NONE :
			return (::new ClipCylZCentral(cd,a,b,c,d,sz));

		default :
			cerr << "trying to create unknown clipping mode. using none." << endl;
			return (::new ClipNone(cd,a,b,c,d));
		}
	}
}

//-----------------------------------------------------------------------------
//-------------- end of file Clip.cc ------------------------------------------
//-----------------------------------------------------------------------------
