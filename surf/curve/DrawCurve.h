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


#ifndef DRAWCURVE_H
#define DRAWCURVE_H

#include "RootFinder3d.h"
#include "RgbBuffer.h"
#include "float_buffer.h"

#include "NewClip.h"

// DrawCurve is used to compute the intersection of two or more polynomials
// in three variables

class DrawCurve : public RootFound3dAction
{
private:
	DrawCurve(const DrawCurve &);
	void operator=(const DrawCurve &);
	
public:
	DrawCurve()
		{
			PointWidth = 3.0;
			gamma_data = 1.0;
			clip = 0;
			memset (additionalPolys, 0, sizeof(additionalPolys));
			additionalPolysCount = 0;
		}
	~DrawCurve()
		{
			int i;
			for (i=0; i<additionalPolysCount; i++) {
				delete additionalPolys[i];
			}
		}
	
	void setCurveWidthAndGamma (double w, double g)
		{
			PointWidth = w;
			gamma_data = g;
		}

	void setBuffers (RgbBuffer *rbuff, float_buffer *fbuff)
		{
			intensity = rbuff;
			zbuffer = fbuff;
		}

	// set the two polys. The resultant of those will be computed.
	void setPolys (const Polyxyz &_p1, const Polyxyz &_p2)
		{
			p1 = _p1;
			p2 = _p2;
			surface = p1;
			rootFinder.setPolys (p1,p2);
		}

	// set additional polynomials
	void setAdditionalPoly (const Polyxyz &p)
		{
			Polyxyz *pnew = new Polyxyz (p);
			additionalPolys[additionalPolysCount++]=pnew;
		}
	
	// set the geometry
	void setGeometry (const WindowGeometry &wm)
		{
			rootFinder.setGeometry (wm);
			wingeo = wm;
		}

	// set clipping area
	void setClip (NewClip *newClip);

	// compute the intersection
	void drawCurve(int x, int y, int width, int height, double zdistance);

private:
	RootFinder3d rootFinder;
	WindowGeometry wingeo;
	NewClip *clip;

	Polyxyz p1,p2;
	Hornerxyz surface; // this is the same as p1 !

	RgbBuffer    *intensity;
	float_buffer *zbuffer;

	double PointWidth;
	double gamma_data;

	Polyxyz * additionalPolys[9];
	int additionalPolysCount;

	bool isZeroOfAdditionalPolys(double x, double y, double z);

private:
	// rootFinder will call this
	virtual bool foundRoot (double x, double y, double z);

	bool setZbuffer( int x, int y, double z );

	int doRow( int px, int py, double CoordZ, int DistY,
		   double MantX, double MantY);

	int doPixel( int px,
		     int ay,
		     double uy,
		     double CoordZ,
		     int DistX,
		     double MantX,
		     double DistanceY);

	void setCurvePixel( int x, int y, double val );
		

	double toUser (int dir, double pix)
		{ return wingeo.pixelToUser(dir,pix);};

	double toPixel (int dir, double u)
		{ return wingeo.userToPixel(dir,u);};

	double getDelta (int dir)
		{ return wingeo.getDelta(dir);};

};
#endif
