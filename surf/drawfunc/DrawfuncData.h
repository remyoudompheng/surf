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

#ifndef DRAWFUNCDATA_H
#define DRAWFUNCDATA_H

#include <assert.h>

#include <polyarith.h>
#include <WindowGeometry.h>


class Clip;
class Vector;
class Position;
class RationalHornerXY;
class HornergroupXY;
class HornergroupXYZ;
class float_buffer;
class RgbBuffer;

class DrawfuncData
{
private:
	DrawfuncData(const DrawfuncData&);
	void operator=(const DrawfuncData&);

public:
	DrawfuncData(RgbBuffer* intensity, float_buffer* zbuff,
		     RationalHornerXY*, HornergroupXYZ*, HornergroupXY*,
		     Clip*, Position*, double);
	~DrawfuncData();

	void SetBorders(int xmin, int xmax, int ymin, int ymax) {
		min[0] = xmin;
		min[1] = ymin;
		max[0] = xmax;
		max[1] = ymax;
	}

	void setGeometry(const WindowGeometry& wg) {
		wingeo = wg;
	}

	// draw curves or cutlines working x/y direction
	void PrintCurve(int);

	// paint point if visible at user coordinates
	void PaintPoint(double, double);
	bool DoRow(int, int, double, int, double, double, double*);
        bool DoPixel(int, int, double, double, int, double, double, double*);

private:
	// set min and max pixel value
	void GetBorders(int dir, int& vmin, int& vmax) {
		assert(dir == 0 || dir == 1);
		vmin = min[dir];
		vmax = max[dir];
	}

	bool SetZbuffer(int x, int y, double val);
	
	void SetCurvePixel(int x, int y, double val);

	double ToUser(int dir, double pix) {
		return wingeo.pixelToUser(dir, pix);
	}

	double ToPixel(int dir, double u) {
		return wingeo.userToPixel(dir, u);
	}

	double GetDelta (int dir) {
		return wingeo.getDelta(dir);
	}
	
private:
	RationalHornerXY* Plane;         // rational equation for plane solved to z
	HornergroupXYZ* Surface;         // surface with derivates
	HornergroupXY*    Curve;         // curve with orth derivative,
	                                 //  xy and yx
	Clip            *clip;                   // clipping
	Position        *position;               // position : turn shift and scale

	double PointWidth;              // width of curve
	double PointDiv;                // 4*pointwidth
	double MaxDist;                 // calculated from pointwidth
	double Delta[2];                // distance between rows/cols
	double WinSizeFactor;	   // for relative PointWidth (Size of Window)
	
	WindowGeometry wingeo;

	RgbBuffer* intensity;
	float_buffer* zbuffer;
	int min[2], max[2];
};

#endif //!DRAWFUNCDATA_H
