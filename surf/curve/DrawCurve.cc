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



#include "DrawCurve.h"

#define TOLERANZ_LOESCHE_LAY2      0.1
#define TOLERANZ_ZEICHNEN_NEU_LAY2 0.09

bool DrawCurve::isZeroOfAdditionalPolys(double x, double y, double z)
{
	int i;
	for (i=0; i<additionalPolysCount; i++) {
		if (fabs(additionalPolys[i]->evaluate(x,y,z)) > 1e-5) {
			return false;
		}
	}
	return true;
}

void DrawCurve::setClip (NewClip *c)
{
	clip=c;
}

void DrawCurve::drawCurve(int x, int y, int width, int height, double zdistance)
{
	rootFinder.setZDistance (zdistance);
	rootFinder.findRoots(x,y,width,height,-10.0,10.0, this);
}


bool DrawCurve::foundRoot (double CoordX, double CoordY, double CoordZ)
{
	if (clip) {
		if (!clip->clip_user_y(CoordY)) {
			// if the y coordinate is "out of range" we really don't want any more roots
			return false;
		}
		double zmin,zmax;
		if (!clip->clip_user_xy(CoordX, CoordY, zmin, zmax)) {
			// out of clipping area....
			return false;
		}
		if (CoordZ > zmax) {
			return true;
		}
		if (CoordZ < zmin) {
			// the next root's z value would even be smaller than this one...
			return false;
		}
	}
	if (!isZeroOfAdditionalPolys(CoordX, CoordY, CoordZ))
		return true;

	double PixelX = toPixel( VARIABLE_X, CoordX );
	double PixelY = toPixel( VARIABLE_Y, CoordY );
	int PXRound = (int)( PixelX + 0.5 );
	int PYRound = (int)( PixelY + 0.5 );

	surface.SetRow( toUser( VARIABLE_Y, PYRound ) );
	surface.SetVar( toUser( VARIABLE_X, PXRound ) );

	int px = (int)PixelX;
	int py = (int)PixelY;
	
	double MantX = PixelX - (double)px;
	double MantY = PixelY - (double)py;

	// starting with the mid row do all pixel rows
	// that are touched by the circular brush at position (PixelX,PixelY)
	// do mid row
	doRow( px, py, CoordZ, 0, MantX, MantY);
	
	// do other rows
	bool active = true;
	int DistY = 1;
	
	while( active ) {
		active = ( doRow( px, py, CoordZ, -DistY, MantX, MantY) +
			   doRow( px, py, CoordZ, +DistY, MantX, MantY) );
		DistY++;
	}

	return false;
}

int DrawCurve::doRow( int px, int py, double CoordZ, int DistY,
		      double MantX, double MantY)
{
	int ay = py + DistY;
	double DistanceY = MantY - (double)DistY;
	double uy = toUser( VARIABLE_Y, ay );

  
	// set variable y in surface polynom
	surface.SetRow(uy);

	// starting with the mid pixel do all pixels in this row
	// that are touched by the brush
	// do mid pixel
	int active = doPixel( px, ay, uy, CoordZ, 0, MantX, DistanceY);

	// do rest of row
	bool working = true;
	int DistX = 1;

	while( working ) {
		// Ahh, changing "+" into "||" in the next statement will not work,
		// because then the second doPixel won´t get executed 
		// Really nice trap :)
		working = ( doPixel( px, ay, uy, CoordZ,
				     -DistX, MantX, DistanceY) + 
			    doPixel( px, ay, uy, CoordZ,
				     +DistX, MantX, DistanceY) );	 
		DistX++;
	}
	// return true if any pixel in this row was modified
	return ( active || DistX > 2 );
}


int DrawCurve::doPixel( int px,
			int ay,
			double uy,
			double CoordZ,
			int DistX,
			double MantX,
			double DistanceY)
{
	int ax = px + DistX;

	// calculate x distance of pixel from curve point
	double DistanceX = MantX - (double)DistX;
	double ux = toUser( VARIABLE_X, ax );


	// sk :Suche NUS mit Surface->Zero in einem kugeligen Bereich um ...
	//     den Schnittpunkt ( von  Plane und Surface) auf der Surface . ...
	//     Damit wird perspektivische Verkürzung der Kurve auf der Fläche ...
	//     möglich, und zum einen werden vernünftige z-Werte berechnet ...
	//     und zum anderen wird die Kurve korrekt auf die Fläche gezeichnet ...
	//     Graphik(unten) zeigt die Kreisscheibe ...
	//
	//      A      ****
	//    Y I    **    **
	//      I   *  O     *   O = z.B DERZEITIG_ERREICHTER_PUNKT,  
	//      I   *        *       dieser liegt auf der x-y Ebene mit ...
	//      I   *        *       z = z-Wert vom Schnittpkt (Plane mit Surface)
	//      I    **    **  	( also : z = CoordZ )
	//      I      ****
	//     -x------------------> X
	
	// sk :angleichen des Kugelradius in z-Richtung auf Fenstergröße, da ...
	//     Kurvenbreite abhängig von Fensterausmaßen 
	double diameter=wingeo.WinSizeFactor()*PointWidth/10.0;
  
	// sk:ermittle entsprechenden Radius  
	double radius  = diameter/2.0;	
  
	// sk:ermittle distance vom Mittelpunkt des DERZEITIG_ERREICHTER_...
	//    PUNKTes (siehe oben) im Kreis   	 
	double distance=sqrt(DistanceX*DistanceX+DistanceY*DistanceY);
  
	if( radius < distance ) {
		return false;
	}

	// sk:rechne z-Wert vom derzeitigen Punkt zum Kugelrand    
	double D =sqrt( radius*radius - distance*distance ); // in Pixeln
	double DU = D/wingeo.WinSizeFactor();
  
  
	// sk :setze Startwert genug weit weg
	double zEstimate= -100000.0;

	//sk:errechne zmin und zmax von dem Bereich in
	// dem die NUSen gesucht werden sollen  	
	double zMinimal = CoordZ-D/wingeo.WinSizeFactor();		
	double zMaximal = CoordZ+D/wingeo.WinSizeFactor();
	
	double brightness=0.0;

	surface.SetVar( ux );	

	// sk :Array to save all roots of plane with surface
	double SRoots[100];
	int    NUS=0;
      
	// sk: NUS = Anzahl, der gefundenen Nullstellen
	NUS = surface.Zero( zMaximal,zMinimal,SRoots,NULL,0) ; 
      
	// sk: Wenn NUS gefunden 
	if ( NUS ) {
		// sk :Suche nach dieser NUS, die in dem Bereich dem Schnittpkt ...
		//     (Plane mit surface) am nächsten liegt
		
		double eps_min=10000.0;
		double eps;
		int i=0;
		
		while( i<NUS && (eps=fabs(SRoots[i] - CoordZ))<eps_min ) { 
			eps_min=eps;
			i++;
		}
         
		// sk :setze zEstimate gleich der gefundene NUS 
		zEstimate = SRoots[i-1];      
		
		// sk :wenn noch innerhalb relativen Radius
		if ( eps_min < DU  ) {  
			// clip pixel in z range
			if( zEstimate > zMaximal ) {
				return false;
			}
			if( zEstimate < zMinimal ) {
				return false;
			}
			
				// sk : setze brightness je nach dem wie
				// weit entfernt der Punkt vom Mittelpkt ist
			brightness=( eps_min*eps_min*wingeo.WinSizeFactor()*wingeo.WinSizeFactor()
				     +DistanceX*DistanceX 
				     +DistanceY*DistanceY  )/(radius*radius);
			
		} else { // end if NUS in "BrushBall"
			
			return false;
		} 
	} else { // end if NUS
		
		// sk :zeichne auch trotzdem wenn überhaupt keine Fläche gefunden
		//  erhoffe mir dadurch Stellen zu zeichnen wenn wie z.B. bei der
		//  Cyclide möglich, der "Ring" der Cyclide genau auf der dem
		//  Plane liegt. 

			
		return false;
	}

  
	// skip pixel if dark
	if( brightness <= 0.0 ) {
		return false;
	}
	
	// sk :     wenn nur Kurve ohne Fläche gezeichnet wird ...
	//     oder wenn neuer z-wert gesetzt, weil er vor dem vorherigen liegt,
	//     dann setze Pixel 
	if( setZbuffer (ax, ay, zEstimate) ) {
		setCurvePixel( ax,ay,1.0 - pow( brightness,/*curve_*/ gamma_data ) );   
	}

	return true;
}


// ----------------------------------------------------------------------------
//  set one pixel of the curve
// ----------------------------------------------------------------------------

void DrawCurve::setCurvePixel( int x, int y, double val )
{
        if( val > 1.0 ) {
	        val = 1.0;
	}		
	if( val > 0.0 ) {
		intensity->SetLayerTwoIfHigher( x,y,(int)floor( val*255.0 ) );
	}
}


// ----------------------------------------------------------------------------
//  Set z-buffer at pixel (x,y) to the value z
// ----------------------------------------------------------------------------
bool DrawCurve::setZbuffer( int x, int y, double z )
{  
        // -----------------
	//  get old z-value
        // -----------------
	double zold = zbuffer->Get(x,y);

	// -----------------------------------------------
	//  if there is not already a curve in this point
	//  delete value in layer two
	// -----------------------------------------------
	if( !intensity->GetTag( x,y,CURVEBIT ) ) {
		intensity->SetLayerTwo(x,y,0);
	}

	// -------------------------------------------
	//  if new z-value is bigger than old z-value
	//  delete value in layer two
	// -------------------------------------------
        if( z > zold + TOLERANZ_LOESCHE_LAY2 ) {
		intensity->SetLayerTwo(x,y,0);
	}

	// -------------------------------------------
	//  if new z-value is up to a tolerance
	//  bigger than old z-value then return  true
	// -------------------------------------------
	if( z > zold - TOLERANZ_ZEICHNEN_NEU_LAY2 ) {
	        // -------------------------------------
		//  change z-buffer only if new z-value
	        //  is strictly bigger
	        // -------------------------------------
		if( z>zold ) {
			zbuffer->Set(x,y,z);
		}
		return true;
	}
	return false;
}
