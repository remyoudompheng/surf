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





#ifndef CLIP_H
#define CLIP_H

#include <WindowGeometry.h>
#include <ScriptVar.h>

class Position;
class Vector;
// class Mywindow;
// ----------------------------------------------------------------------------
// --------------- base for all clip classes ----------------------------------
// ----------------------------------------------------------------------------

class Clip
{
protected:
	int ClipModus;
	double Front;
	double Back;
	double Radius;
	double Center[3];
	double RadQuad;
	double WinMin[2];
	double WinMax[2];

public:
	Clip( const int _ClipModus, const clip_numeric_t & _ClipData, 
	      const double win_min_0, const double win_max_0, 
	      const double win_min_1, const double win_max_1);

	static Clip* create (const int, const int, const clip_numeric_t &,
			     const WindowGeometry &wingeo, const double, Position*,
			     int xmin, int xmax, int ymin, int ymax);

	// replacement for operator new

	virtual int ClipXY( int, double, double&, double& ) = 0;
	virtual int ClipXYZ( double, double, double&, double& ) = 0;
	// pure virtual functions
};

// ----------------------------------------------------------------------------
// --------------- base for central clip classes ------------------------------
// ----------------------------------------------------------------------------


class ClipCentral : public Clip
{
protected:
	double SpectatorZ;
	double ZDistance;
	double ZDistSquare;
	double CenterSquare[3];
	double SpecZSquare;

public:
	ClipCentral(const int, const clip_numeric_t &, const double, const double,
		    const double, const double, const double);

	virtual int ClipXY( int, double, double&, double& ) = 0;
	virtual int ClipXYZ( double ux, double uy, double &min, double &max ) = 0;

};

// ----------------------------------------------------------------------------
// --------------- clipping on sphere -- parallel projection ------------------
// ----------------------------------------------------------------------------

  
class ClipSphere : public Clip
{
public:
	ClipSphere( const clip_numeric_t &, const double, const double, const double,
		    const double);
	virtual int ClipXY( int, double, double&, double& );
	virtual int ClipXYZ( double ux, double uy, double &min, double &max );
};

// ----------------------------------------------------------------------------
// --------------- clipping on horizontal cylinder -- parallel projection -----
// ----------------------------------------------------------------------------

class ClipCylH : public Clip
{
public:
	ClipCylH( const clip_numeric_t &, const double, const double, const double,
		  const double);
	virtual int ClipXY( int, double, double&, double& );
	virtual int ClipXYZ( double, double uy, double &min, double &max );
};

// ----------------------------------------------------------------------------
// --------------- clipping on vertical cylinder -- parallel projection -------
// ----------------------------------------------------------------------------

class ClipCylV : public Clip
{
public:
	ClipCylV( const clip_numeric_t &, const double, const double, const double,
		  const double);
	virtual int ClipXY( int, double, double&, double& );
	virtual int ClipXYZ( double ux, double, double &min, double &max );
};

// ----------------------------------------------------------------------------
// --------------- clipping on cylinder z direction-- parallel projection -----
// ----------------------------------------------------------------------------

class ClipCylZ : public Clip
{
public:
	ClipCylZ( const clip_numeric_t &, const double, const double, const double,
		  const double);
	virtual int ClipXY( int, double, double&, double& );
	virtual int ClipXYZ( double ux, double uy, double &min, double &max );
};

// ----------------------------------------------------------------------------
// --------------- clipping on cube -- parallel projection --------------------
// --------------- cube is rotated with the surface ---------------------------
// ----------------------------------------------------------------------------

class ClipCube : public Clip
{
	double  Length[6];
	Vector  *CubeVec;
public:
	ClipCube( const clip_numeric_t &, const double, const double, const double,
		  const double, Position*);
	virtual ~ClipCube();
	virtual int ClipXY( int, double, double&, double& );
	virtual int ClipXYZ( double ux, double uy, double &min, double &max );
};

// ----------------------------------------------------------------------------
// --------------- clipping on view only -- parallel projection ---------------
// ----------------------------------------------------------------------------

class ClipNone : public Clip
{
public:
	ClipNone( const clip_numeric_t &, const double, const double, const double,
		  const double);
	virtual int ClipXY( int, double, double&, double& );
	virtual int ClipXYZ( double, double, double &min, double &max );
};

// ----------------------------------------------------------------------------
// --------------- clipping on spere -- central projection --------------------
// ----------------------------------------------------------------------------

class ClipSphereCentral : public ClipCentral
{
	double *A;
	double *B;
	double *a;
	double *Disc;
public:
	ClipSphereCentral( const clip_numeric_t &, const double, const double, const double,
			   const double, const double );
	virtual ~ClipSphereCentral();
	virtual int ClipXY( int, double, double&, double& );
	virtual int ClipXYZ( double, double, double&, double& );
};

// ----------------------------------------------------------------------------
// --------------- clipping on horizontal cylinder -- central projection ------
// ----------------------------------------------------------------------------

class ClipCylHCentral : public ClipCentral
{
	double *A;
	double *B;
	double *a;
	double *Disc;
public:
	ClipCylHCentral( const clip_numeric_t &, const double, const double, const double,
			 const double, const double );
	virtual ~ClipCylHCentral();
	virtual int ClipXY( int, double, double&, double& );
	virtual int ClipXYZ( double, double, double&, double& );
};

// ----------------------------------------------------------------------------
// --------------- clipping on vertical cylinder -- central projection --------
// ----------------------------------------------------------------------------

class ClipCylVCentral : public ClipCentral
{
	double *A;
	double *B;
	double *a;
	double *Disc;
public:
	ClipCylVCentral( const clip_numeric_t &, const double, const double, const double,
			 const double, const double );
	virtual ~ClipCylVCentral();
	virtual int ClipXY( int, double, double&, double& );
	virtual int ClipXYZ( double, double, double&, double& );
};

// ----------------------------------------------------------------------------
// --------------- clipping on cylinder in z direction -- central projection --
// ----------------------------------------------------------------------------

class ClipCylZCentral : public ClipCentral
{
public:
	ClipCylZCentral( const clip_numeric_t &, const double, const double, const double,
			 const double, const double );
	virtual int ClipXY( int, double, double&, double& );
	virtual int ClipXYZ( double, double, double &min, double &max );
};

// ----------------------------------------------------------------------------
// --------------- clipping on cube -- central projection ---------------------
// --------------- cube is rotated with the surface ---------------------------
// ----------------------------------------------------------------------------

class ClipCubeCentral : public ClipCentral {
	double Length[6];
	Vector *CubeVec;
public:
	ClipCubeCentral( const clip_numeric_t &, const double, const double, const double,
			 const double, const double, Position* );
	virtual ~ClipCubeCentral();
	virtual int ClipXY( int, double, double&, double& );
	virtual int ClipXYZ( double ux, double uy, double &min, double &max );
};


#endif // CLIP_H
