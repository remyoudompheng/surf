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





#include <stdio.h>
#include <math.h>

#include "DrawfuncData.h"
#include "MultiVariatePolynom.h"
#include "Hornergroup.h"
#include "Clip.h"
#include "gui_config.h"
#include "def.h"
#include "float_buffer.h"
#include "RgbBuffer.h"

// #define DEBUG
#include "debug.h"
#include "stop.h"


#define TOLERANZ_LOESCHE_LAY2      0.1
#define TOLERANZ_ZEICHNEN_NEU_LAY2 0.09


// ----------------------------------------------------------------------------
// --------- constructor -- bring it all together -----------------------------
// ----------------------------------------------------------------------------

DrawfuncData::DrawfuncData( RgbBuffer *rgbbuff, float_buffer *zbuff,
			    RationalHornerXY* PLANE,
			    HornergroupXYZ* SURF,
			    HornergroupXY* CURVE,
			    Clip *CL,
			    Position *POS,
			    double PW )
	:  Plane(PLANE),
	   Surface(SURF),
	   Curve(CURVE),
	  
	   clip(CL),
	   position(POS),                      
	   PointWidth(PW),                     // curve width
	   PointDiv(4*PW),                      
	   MaxDist(pow(PW+1,2.0)/(4.0*PW)),
	   WinSizeFactor((double)min(main_width_data,main_height_data)/20.0),
	   intensity(rgbbuff),
	   zbuffer(zbuff)
{
	Delta[VARIABLE_X] = GetDelta( VARIABLE_X );// coordinate distances
	Delta[VARIABLE_Y] = GetDelta( VARIABLE_Y );
}


DrawfuncData::~DrawfuncData()
{
	delete Curve;
	delete Surface;
	delete Plane;
}

// ----------------------------------------------------------------------------
// -------------- draw cutlines working in y direction ------------------------
// ----------------------------------------------------------------------------

void DrawfuncData::PrintCurve( int direction )
{
	BEGIN("DrawfuncData::PrintCurve");
	assert(direction == 0 || direction == 1);

	double Root[100];                 // array to store roots
	double Estimate[100];             // array to store estimates
	int    NumberOfEstimates = 0;     // counter
	int NumberOfRoots = 0;
	double Coord[2];
	double minimal = 0.0;   	    // coordinate range for current line/col clip
	double maximal = 0.0;
	int min = 0;
	int max = 0;
	GetBorders( direction, min, max );

	// go through all lines resp. columns
	for( int Pixel = min; Pixel < max && !stop ; Pixel++ ) {

		// transform pixel y to y coordinate
		Coord[direction] = ToUser( direction, Pixel );

		// get minimum and maximum x coordinate from clipping
		if( clip->ClipXY( direction, Coord[direction], minimal, maximal ) ) {
			// set value in function
			Curve->SetVar( direction, Coord[direction] );
	  
			// find all roots of function in min...max
			// with estimates at estimate(0)...estimate(NumberOfEstimates-1)
			if( ( NumberOfRoots = Curve->Zero( direction,
							   maximal,
							   minimal,
							   Root,
							   Estimate,
							   NumberOfEstimates ) ) ) {
				// reset number of estimates
				NumberOfEstimates = 0;
				// go through all roots found
				for( int RootNumber = 0;
				     RootNumber < NumberOfRoots;
				     RootNumber++ ) {
					Coord[1-direction] = Root[RootNumber];
					PaintPoint( Coord[0], Coord[1] );
					// set estimate at old root
					Estimate[NumberOfEstimates] = Root[RootNumber]
						+ Delta[direction] * Curve->EstimateDelta( direction, Root[RootNumber] );
					
					// only use estimate if smaller than previous
					if( NumberOfEstimates == 0 || Estimate[NumberOfEstimates] < Estimate[NumberOfEstimates-1] )
						NumberOfEstimates++;
				}
			}
		}
	}
}



// ----------------------------------------------------------------------------
// (sk :) PAINT - POINT  
//  sk1:lösche HiddenLine flag, nicht mehr nötig, da nun möglich durch transparence
// ----------------------------------------------------------------------------

void DrawfuncData::PaintPoint( double CoordX, double CoordY )
{
	double CoordZ = -100000.0;
	double zMinimal = 0.0;
	double zMaximal = 0.0;
	double PixelX = ToPixel( VARIABLE_X, CoordX );
	double PixelY = ToPixel( VARIABLE_Y, CoordY );
	int PXRound = (int)( PixelX + 0.5 );
	int PYRound = (int)( PixelY + 0.5 );
	double ZDelta[2];

	// get z range and clip
	if( !clip->ClipXYZ( CoordX, CoordY, zMinimal, zMaximal ) )
		return;
  
	// surface clipping for mid pixel(s)
	// if drawing curve on a surface and hidden line enabled
	if( Surface ) {							/* sk1*/

		// get z of point and clip
		CoordZ = Plane->ZValue( CoordX, CoordY );
		
		// printf("z-Plane = %f at x %f, y %f\n",CoordZ, CoordX, CoordY );

		if( CoordZ < zMinimal )
			return;
		if( CoordZ > zMaximal )
			return;

		// see if surface or ball is in front
		Surface->SetRow( CoordY );
		Surface->SetVar( CoordX );

		// sk1: "if (surface in front)" : nicht sinnvoll, da alle NUS gefunden werden sollen 

		// estimate deltas according to surface derivatives
		ZDelta[VARIABLE_X] = Surface->EstimateDelta( VARIABLE_X, CoordZ ) * Delta[VARIABLE_X];
		ZDelta[VARIABLE_Y] = Surface->EstimateDelta( VARIABLE_Y, CoordZ ) * Delta[VARIABLE_Y];
		// no one is really interested in ZDelta...[RS]
		
		// get max surface z of closest pixel and compare with z buffer
		Surface->SetRow( ToUser( VARIABLE_Y, PYRound ) );
		Surface->SetVar( ToUser( VARIABLE_X, PXRound ) );
       
	}


	int px = (int)PixelX;
	int py = (int)PixelY;

	double MantX = PixelX - (double)px;
	double MantY = PixelY - (double)py;
  

  
	// starting with the mid row do all pixel rows
	// that are touched by the circular brush at position (PixelX,PixelY)
	// do mid row
	DoRow( px, py, CoordZ, 0, MantX, MantY, ZDelta );
	
	// do other rows
	int active = TRUE;
	int DistY = 1;
	
	while( active ) {
		active = ( DoRow( px, py, CoordZ, -DistY, MantX, MantY, ZDelta ) +
			   DoRow( px, py, CoordZ, +DistY, MantX, MantY, ZDelta ) );
		DistY++;
	}
}
// ----------------------------------------------------------------------------
// (sk :) DoRow     
//  sk1:lösche HiddenLine flag, nicht mehr nötig, da nun möglich durch transparence
// ----------------------------------------------------------------------------

int DrawfuncData::DoRow( int px, int py, double CoordZ, int DistY,
			  double MantX, double MantY, double *ZDelta )
{
	int ay = py + DistY;
	double DistanceY = MantY - (double)DistY;
	double uy = ToUser( VARIABLE_Y, ay );

  
	// if drawing on surface with hidden line enabled
	// set variable y in surface polynom
	if( Surface )							/* sk1*/
		Surface->SetRow( uy );

	// starting with the mid pixel do all pixels in this row
	// that are touched by the brush
	// do mid pixel
	int active = DoPixel( px, ay, uy, CoordZ, 0, MantX, DistanceY, ZDelta );

	// do rest of row
	int working = TRUE;
	int DistX = 1;

	while( working ) {
		// Ahh, changing "+" into "||" in the next statement will not work,
		// because then the second DoPixel won´t get executed 
		// Really nice trap :)
		working = ( DoPixel( px, ay, uy, CoordZ,
				     -DistX, MantX, DistanceY, ZDelta ) + 
			    DoPixel( px, ay, uy, CoordZ,
				     +DistX, MantX, DistanceY, ZDelta ) );	 
		DistX++;
	}
	// return true if any pixel in this row was modified
	return ( active || DistX > 2 );
}

// ----------------------------------------------------------------------------
// (sk :) DoPixel 
//  sk1:lösche HiddenLine flag, nicht mehr nötig, da nun möglich durch transparence
// ----------------------------------------------------------------------------
  
int DrawfuncData::DoPixel( int px,
			   int ay,
			   double uy,
			   double CoordZ,
			   int DistX,
			   double MantX,
			   double DistanceY,
			   double *ZDelta )
{
	int ax = px + DistX;

	// calculate x distance of pixel from curve point
	double DistanceX = MantX - (double)DistX;
	double ux = ToUser( VARIABLE_X, ax );


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
	double diameter=WinSizeFactor*PointWidth/10.0;
  
	// sk:ermittle entsprechenden Radius  
	double radius  = diameter/2.0;	
  
	// sk:ermittle distance vom Mittelpunkt des DERZEITIG_ERREICHTER_...
	//    PUNKTes (siehe oben) im Kreis   	 
	double distance=sqrt(DistanceX*DistanceX+DistanceY*DistanceY);
  
	if( radius < distance ) 
		return FALSE;

	// sk:rechne z-Wert vom derzeitigen Punkt zum Kugelrand    
	double D =sqrt( radius*radius - distance*distance ); // in Pixeln
	double DU = D/WinSizeFactor;
  
  
	// sk :setze Startwert genug weit weg
	double zEstimate= -100000.0;

	//sk:errechne zmin und zmax von dem Bereich in
	// dem die NUSen gesucht werden sollen  	
	double zMinimal = CoordZ-D/WinSizeFactor;		
	double zMaximal = CoordZ+D/WinSizeFactor;
	
	
	// get z range at pixel and clip
	double zMin,zMax;
	if( !clip->ClipXYZ( ux, uy, zMin, zMax ) ) 
		return FALSE;
	
	if( zMaximal > zMax ) 
		zMaximal = zMax;
	if( zMinimal < zMin ) 
		zMinimal = zMin; 
	double brightness=0.0;
	
	if( Surface ) {
		Surface->SetVar( ux );	

		// sk :Array to save all roots of plane with surface
		double SRoots[100];
		int    NUS=0;
      
		// sk: NUS = Anzahl, der gefundenen Nullstellen
		NUS = Surface->Zero( zMaximal,zMinimal,SRoots,NULL,0) ; 
      
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
				if( zEstimate > zMaximal ) 
					return FALSE;
				if( zEstimate < zMinimal )
					return FALSE;

				// sk : setze brightness je nach dem wie
				// weit entfernt der Punkt vom Mittelpkt ist
				brightness=( eps_min*eps_min*WinSizeFactor*WinSizeFactor
					     +DistanceX*DistanceX 
					     +DistanceY*DistanceY  )/(radius*radius);

			} else { // end if NUS in "BrushBall"

				return FALSE;
			} 
		} else { // end if NUS

			// sk :zeichne auch trotzdem wenn überhaupt keine Fläche gefunden
			//  erhoffe mir dadurch Stellen zu zeichnen wenn wie z.B. bei der
			//  Cyclide möglich, der "Ring" der Cyclide genau auf der dem
			//  Plane liegt. 

			
// 			if ( ( fabs(DistanceX) < 2.0 ) && ( fabs(DistanceY) < 2.0 ) ) {
// 				zEstimate = CoordZ- DistanceX * ZDelta[VARIABLE_X]     
// 					- DistanceY * ZDelta[VARIABLE_Y];
			  
// 				brightness=MaxDist-( 
// 					DistanceX*DistanceX 
// 					+DistanceY*DistanceY  )/PointDiv;
// 			} else {
				return FALSE;
// 			}
		}
	} else {//end if surface
		// sk :calculate curve_intensity without surface
		brightness = ( DistanceX*DistanceX +
			       DistanceY*DistanceY )/(radius*radius);
	}
  
	// skip pixel if dark
	if( brightness <= 0.0 ) {
		return FALSE;
	}
	
	// sk :     wenn nur Kurve ohne Fläche gezeichnet wird ...
	//     oder wenn neuer z-wert gesetzt, weil er vor dem vorherigen liegt,
	//     dann setze Pixel 
	if( SetZbuffer (ax, ay, zEstimate) || !Surface ) {
		SetCurvePixel( ax,ay,1.0 - pow( brightness,curve_gamma_data ) );   
	}

	return TRUE;
}


// ----------------------------------------------------------------------------
// sk :Set Z-Buffer at pixel x,y
// ----------------------------------------------------------------------------
bool DrawfuncData::SetZbuffer( int x, int y, double z )
{  
	// get old value
	double zold=zbuffer->Get(x,y);
  
	// sk :     wenn hier noch keine Kurve gesetzt  ...
	//     oder wenn hier ein z-Wert einer curve schon im hintergrund, ...
	//     dann lösche Pixel im Layer2  
	if(!intensity->GetTag(x,y,CURVEBIT)|| (z>zold+TOLERANZ_LOESCHE_LAY2) ) {
		intensity->SetLayerTwo(x,y,0);
	}
          
	// sk :wenn z-Wert geringfügig (TOLERANZ_ZEICHEN_NEU_LAY2) ...  
	//     dahinter liegt , dann ...
	//     trotzdem setzen und damit zeichnen
	if( z+TOLERANZ_ZEICHNEN_NEU_LAY2 > zold ) {
		// sk :aber nur zbuffer setzen wenn neuer Wert echt größer ist
		if( z>zold ) {    
			zbuffer->Set(x,y,z);
		}
		return true;
	}
	return false;
}





// ----------------------------------------------------------------------------
// (sk :) set one pixel of the curve     ( Umstellung auf RGB-Modell )
// ----------------------------------------------------------------------------
void DrawfuncData::SetCurvePixel( int x, int y, double val )
{
	if( x > min[0] && x < max[0] && y > min[1] && y < max[1] ) {
		if( val > 1.0 ) 
			val = 1.0;
		
		if( val > 0.0 ) {
			intensity->SetLayerTwoIfHigher( x, y,(int)floor( val*255.0 ));
		}
	}
}
  
// ----------------------------------------------------------------------------
// --------------------- end of file DrawData.cc ------------------------------
// ----------------------------------------------------------------------------
