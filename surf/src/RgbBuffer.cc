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







// ----------------------------------------------------------------------------
// file                rgb_buffer.cc
// implementation by kai
// date                25.06.96
// sk :ganzer File neu
// ----------------------------------------------------------------------------



#include <math.h>
#include <stdlib.h>
#include <iostream.h>
#include <stdio.h>
#include <string.h>


#include "def.h"
#include "compfn.h"
#include "color.h"

#include "RgbBuffer.h"
#include "oct_quan.h"

#include "RGBToNetscape.h"


// ----------------------------------------------------------------------------
// constructors for RgbBuffer
// ----------------------------------------------------------------------------
RgbBuffer::RgbBuffer()
	: width(0), height(0),
	  r( NULL ),
	  g( NULL ),
	  b( NULL ), 		 	
	  map( NULL ),
	  curv( NULL),
	  
	  tag ( NULL ),               
	  n(0),
	  nmap(0)
{
}


RgbBuffer::RgbBuffer( int w, int h )
	: width(w),
	  height(h),
	  r(new byte [w*h] ),
	  g(new byte [w*h] ),
	  b(new byte [w*h] ),	
	  map(new byte [w*h] ),		
	  curv(new byte[w*h] ),
	  
	  tag(new byte[w*h]),         
	  n(w*h), 
	  nmap(0)	 	 
           
{
}

// ----------------------------------------------------------------------------
// release memory 
// ----------------------------------------------------------------------------
RgbBuffer::~RgbBuffer()
{
	delete [] r;
	delete [] g;
	delete [] b;
	delete [] map;
	delete [] curv;
	delete [] tag;              
}

void RgbBuffer::clearTags()
{
	memset (tag, 0, width*height*sizeof(byte));
}


void RgbBuffer::clearCurveTags()
{
	int i;
	for (i=0; i<width*height; i++) {
		tag[i] &= ~ (CURVELINEBIT | CURVEBIT);
	}
}

// ----------------------------------------------------------------------------
// change size of buffer
// ----------------------------------------------------------------------------
void RgbBuffer::Realloc( int w, int h )
{
	n      = w * h;
	width  = w;
	height = h;

	
	delete [] r;
	delete [] g;
	delete [] b;
  
	delete [] map;

	delete [] curv;
  
	delete [] tag;			
    					
	r    = new byte [n];
	g    = new byte [n];
	b    = new byte [n];
	map  = new byte [n];
	curv = new byte [n];
	tag  = new byte [n];            
}

// ----------------------------------------------------------------------------
// set the whole buffer equal f
// ----------------------------------------------------------------------------
RgbBuffer& RgbBuffer::operator=( byte f )
{
	if (f>255)
		f=255;

	int i;
	for( i=0; i<n; i++ ) {
		r[i]    = f;
		g[i]    = f;
		b[i]    = f;
		curv[i] = f;    
	}
	return *this;
}

void    RgbBuffer::Set_whole( int value0, int value1 ,int value2)
{
	value0 = (value0>255) ? 255 : value0;
	value1 = (value1>255) ? 255 : value1;
	value2 = (value2>255) ? 255 : value2;
	int i;
	for( i=0; i<n; i++ ) {
		r[i] = (byte)value0; 			
		g[i] = (byte)value1; 			
		b[i] = (byte)value2; 			
	}
}

// ----------------------------------------------------------------------------
// set the whole buffer equal zero 
// ----------------------------------------------------------------------------
void RgbBuffer::NullInit()
{
	int i;
	for( i=0; i<n; i++ ) {
		r[i]    = 0;  			
		g[i]    = 0;
		b[i]    = 0;
		map[i]  = 0;
		curv [i]= 0;
		tag[i]  = 0;                          
	}
}

// ----------------------------------------------------------------------------
// set only first layer equal zero 
// ----------------------------------------------------------------------------
void RgbBuffer::NullInit_one()
{
	int i;
	for( i=0; i<n; i++ ) {
		r[i] = 0;                       
		g[i] = 0;                        
		b[i] = 0;                     
		tag[i]=0;             		           
	}
}

// ----------------------------------------------------------------------------
// set only second layer equal zero 
// ----------------------------------------------------------------------------
void RgbBuffer::NullInit_two()
{
	int i;
	for( i=0; i<n; i++ ) {
		curv[i] = 0;                     
	}
}

void RgbBuffer::NullInit_three( )
{
	int i;
	for( i=0; i<n; i++ ) {
		r[i]    = 0;
		g[i]    = 0;
		b[i]    = 0;
		curv[i] = 0;
		tag[i]   =0;
	}
}

// ----------------------------------------------------------------------------
// copy absolut values of a rgb buffer 
// ----------------------------------------------------------------------------
RgbBuffer& RgbBuffer::operator=( const RgbBuffer& a )
{
	int i;
	for( i=0; i < n; i++ ) {
		r[i]    = (byte)abs( a.r[i] ); 
		g[i]    = (byte)abs( a.g[i] ); 
		b[i]    = (byte)abs( a.b[i] ); 
		map[i]  = (byte)abs( a.map[i] ); 
		curv[i] = (byte)abs( a.curv[i] ); 
		tag[i]  = (byte)abs( a.tag[i]);        
	}
	return *this;
}

// ----------------------------------------------------------------------------
// copy from a rgb buffer with maybe different size 
// ----------------------------------------------------------------------------

void RgbBuffer::Copy( const RgbBuffer& alt )
{
	int xm = min( width, alt.width );
	int ym = min( height, alt.height );

	for( int y = 0; y < ym; y++ ) {
		for( int x = 0; x < xm; x++ ) {
			r[y*width+x]    = alt.r[y*width+x];
			g[y*width+x]    = alt.g[y*width+x];
			b[y*width+x]    = alt.b[y*width+x];
			map[y*width+x]  = alt.map[y*width+x];
			curv[y*width+x] = alt.curv[y*width+x];
			tag[y*width+x]  = alt.tag[y*width+x];        
		}
	}
}

// ----------------------------------------------------------------------------
// copy one line of a rgb buffer with maybe different size 
// ----------------------------------------------------------------------------
void RgbBuffer::CopyLine( const RgbBuffer& alt, int src, int dest )
{
	if( dest >= 0 && dest < height ) {
		int xm = min( width, alt.width );

		for( int x = 0; x < xm; x++ ) {
			r[dest*width+x]    = alt.r[src*width+x];
			g[dest*width+x]    = alt.g[src*width+x];
			b[dest*width+x]    = alt.b[src*width+x];
			map[dest*width+x]  = alt.map[src*width+x];
			curv[dest*width+x] = alt.curv[src*width+x];
			tag[dest*width+x]  = alt.tag[src*width+x];       
		}
	}
}



// ----------------------------------------------------------------------------
// Get a color on different ways     
// ----------------------------------------------------------------------------

// Get one color at pixel number      -----------------------------------------
byte   RgbBuffer::Get_one( int num, int color )  const
{
	if( num<0 || num>=n )     
		return 0;
            
	switch (color) {
        case 0:    return r[num];
        case 1:	   return g[num];
        case 2:    return b[num];
        case 3:    return map[num];

        default :  return 0;
	}
}

//  Get one color at pixel x,y   ---------------------------------------------          
byte   RgbBuffer::Get_one( int x, int  y , int color) const
{
	if( x<0 || x>=width || y<0 || y>=height )
		return 0;
        
	switch (color) {
        case 0:    return r[ y*width + x ];
        case 1:    return g[ y*width + x ];
        case 2:    return b[ y*width + x ];
        case 3:    return map[ y*width + x ];
      
        default :  return 0;
	}

}

// ----------------------------------------------------------------------------
// Set a color or all on different ways at a pixel      
// ----------------------------------------------------------------------------

//  Set all colors at pixel number --------------------------------------------          
void    RgbBuffer::Set( int num, int value0, int value1 ,int value2)
{
	if( num >= 0 && num < n ) {
		r[num] = (byte)value0; 			
		g[num] = (byte)value1; 			
		b[num] = (byte)value2; 			
	}

}
//  Set all colors at pixel x,y -----------------------------------------------          
void    RgbBuffer::Set( int x, int y, int value0, int value1, int value2)
{
	if( x >= 0 && x < width && y >= 0 && y < height ) {
		r[ y * width + x ] = (value0>255.0) ? 255 : value0;  	
		g[ y * width + x ] = (value1>255.0) ? 255 : value1;  	
		b[ y * width + x ] = (value2>255.0) ? 255 : value2;  	
	}
}
//  Set one color at pixel number      ---------------------------------------- 
void    RgbBuffer::Set_one( int num, int value,int color)
{
	if( num >= 0 && num < n ) {  
		switch (color) {
		case 0:   	r[num] = (byte)value;break;
		case 1:   	g[num] = (byte)value;break;
		case 2:   	b[num] = (byte)value;break;
		case 3:   	map[num] = (byte)value;break;
            
		default : 	break;     
		}
	}
}
//  Set one color at pixel x,y  -----------------------------------------------           
void    RgbBuffer::Set_one( int x, int y, int value ,int color )
{
	if( x >= 0 && x < width && y >= 0 && y < height ) {  
		switch (color) {
		case 0:	r[ y * width + x ] =(byte)value;break;
		case 1:	g[ y * width + x ] =(byte)value;break;
		case 2:	b[ y * width + x ] =(byte)value;break;
		case 3:	map[ y * width + x ] =(byte)value;break;
            
		default :	break;
		}
	}
}

  
// ----------------------------------------------------------------------------
// ----------- multiply element with a value on different ways-----------------
// ----------------------------------------------------------------------------

//  Multiply one value with all colors at pixel number ------------------------  
void RgbBuffer::Mult( int num, float value )
{
	if( num >= 0 && num < n ) {
		r[num] =(byte)(r[num]* value);
		g[num] =(byte)(g[num]* value);
		b[num] =(byte)(b[num]* value);
	}
}
//  Multiply one value with all colors at pixel x,y ---------------------------  
void RgbBuffer::Mult( int x, int y, float value )
{
	if( x >= 0 && x < width && y >= 0 && y < height ) {
		r[ y * width + x ] = (byte)(r[ y * width + x ]* value);
		g[ y * width + x ] = (byte)(g[ y * width + x ]* value);
		b[ y * width + x ] = (byte)(b[ y * width + x ]* value);
	}
}

float RgbBuffer::Get_Gray_value(int x, int y)
{
	if( x<0 || x>=width || y<0 || y>=height ) {
		return 0.0;
	}

	int n = y*width+x;
	return (0.299*r[n]+0.587*g[n]+0.114*b[n])/255.0;
//  	return  (float)( (   0.299 * (float)Get_one(x,y,red  ) 
//  			     + 0.587 * (float)Get_one(x,y,green) 
//  			     + 0.114 * (float)Get_one(x,y,blue ) )/255.0);                 
}

// ----------------------------------------------------------------------------
// set element to a max value if greater 
// ----------------------------------------------------------------------------
void RgbBuffer::CutAtMax( int x, int y, byte value )
{
	if( x>=0 && x<width && y>=0 && y<height ) {
		int i = y * width + x;
		if( r[i] > value )     
			r[i] = value;
		if( g[i] > value )     
			g[i] = value;
		if( b[i] > value )     
			b[i] = value;
	}
}

// ----------------------------------------------------------------------------
// get, set or del a flag at pixel x,y ,
// use single bit of a byte to save a flag-infos(On/Off) at definite position 
// ----------------------------------------------------------------------------
bool RgbBuffer::GetTag( int x, int y, int position ) const     
{
	if( x < 0 || x >= width || y < 0 || y >= height)
		return false;    						
  
	return tag[y*width+x] & position;
}

void RgbBuffer::SetTag( int x, int y, int position )
{
	if( x < 0 || x >= width || y < 0 || y >= height) 
		return;
	
	tag[y*width+x] |= position;
}

void RgbBuffer::DelTag( int x, int y, int position ) 
{
	if( x < 0 || x >= width || y < 0 || y >= height)
		return;

	tag[y*width+x] &= ~position;
}

// ----------------------------------------------------------------------------
// set value of curve intensity at pixel x,y 
// ----------------------------------------------------------------------------
void RgbBuffer::SetLayerTwo( int x, int y, int value  )
{
	if( x >= 0 && x < width && y >= 0 && y < height ) {  
		value=(value>255) ? 255 :value;
		
		curv[ y * width + x ] =value;  
	}
}

// ----------------------------------------------------------------------------
// set value of curve intensity at pixel x,y ; if greater than old value ------
// ----------------------------------------------------------------------------
void RgbBuffer::SetLayerTwoIfHigher( int x, int y, int value  ) 
{
	if( x >= 0 && x < width && y >= 0 && y < height ) {
		int i = y*width+x;
		
		if( value > curv[i]) {
			curv[i] = (value>255) ? 255 : (byte)value;
		}

		SetTag(x,y,CURVEBIT);
		SetTag(0,y,CURVELINEBIT);  	
	}
}

// ----------------------------------------------------------------------------
// get intensity of curve at pixel x,y 
// ----------------------------------------------------------------------------
byte RgbBuffer::GetLayerTwo( int x, int y ) const     
{
	if( x < 0 || x >= width || y < 0 || y >= height )
		return 0;    	

	return curv[y*width+x];
}

// ----------------------------------------------------------------------------
//  map the RGB colors to the Netscape 216 color cube (6,6,6)
// ----------------------------------------------------------------------------

void RgbBuffer::NetscapeColor ()
{
	// --------------------
	//  Map the RGB values
	// --------------------

	int x,y,i=0;
	
	for( y=0; y<height; y++ ) {
		for( x=0; x<width; x++ ) {
			map[i] = RGBToNetscape( ((float)r[i])/255.0,
						((float)g[i])/255.0,
						((float)b[i])/255.0,x,y );
			i++;
		}
	}

	// ------------------------------
	//  Create the Netscape colormap
	// ------------------------------

	int rcount,gcount,bcount;
	i=0 ;
	nmap=216;
	
	for( rcount=0; rcount<6; rcount++ ) {
		for( gcount=0; gcount<6; gcount++ ) {
			for( bcount= 0; bcount<6; bcount++ ) {
				rmap[i] = (byte)(rcount*51);
				gmap[i] = (byte)(gcount*51);
				bmap[i] = (byte)(bcount*51);
				
				i++; 
			}
		}
	}
}

// ----------------------------------------------------------------------------
//  map the RGB colors to 216 optimized colors
// ----------------------------------------------------------------------------

void RgbBuffer::OptimizedColor (bool dither, double ditherval)
{
	// --------------------
	//  Map the RGB values
	// --------------------

	int anull=0;
	int x,y,i=0;
	OctQuant Trees;
	OctColor rgb;
	
	// Trees.OctQuant( );
	Trees.width =width;
	Trees.height=height;
	Trees.BuildTree (*this, dither, ditherval );
	Trees.FillPalette( Trees.proot,&anull );
	
	for( y=0; y<height; y++ ) {
		for( x=0; x<width; x++ ) {
			rgb.SetRed  (r[i]);
			rgb.SetGreen(g[i]);
			rgb.SetBlue (b[i]);
			
			if (dither)
				rgb.Dither (x, y, ditherval);
			
			map[i]=Trees.QuantizeColor( Trees.proot,rgb );
			
			i++;
		}
	}

	// -----------------------------
	//  Copy the optimized colormap
	// -----------------------------
	for( i=0; i<216; i++ ) {
		rmap[i] = Trees.Palette[i].GetRed();
		gmap[i] = Trees.Palette[i].GetGreen();
		bmap[i] = Trees.Palette[i].GetBlue();
	}
	nmap=216;
}


void    RgbBuffer::StereoLeft( void )
{
	for( int i=0; i<n; i++ ) {             
		map[i] =(int)( 0.299*((float)(r[i]))
			       +0.587*((float)(g[i]))
			       +0.114*((float)(b[i])));
	}             
}

void    RgbBuffer::StereoRight( float rm,float gm,float bm,int dist,int back )
{
	int x,y,xl,xr,il,i,ir;
	
	for( i=0; i<n; i++ ) {             
		curv[i] =(int)( 0.299*((float)(r[i]))
				+0.587*((float)(g[i]))
				+0.114*((float)(b[i])));
	}             

	for( y=0; y<height; y++ ) {
		for( x=0, xl=-dist, xr=dist; x<width; x++, xl++, xr++ ) {
			i = width*y+x;
			il = i-dist;
			ir = i+dist;

			r[i] = ( xr < width ? (int)(rm*((float)( map[ir]))):back );
			g[i] = ( xl >=0     ? (int)(gm*((float)(curv[il]))):back );
			b[i] = ( xl >=0     ? (int)(bm*((float)(curv[il]))):back );
		}
	}
}


void RgbBuffer::AddCurve (int curve_r, int curve_g, int curve_b)
{
	int    r_tmp,Layer1r;
	int    g_tmp,Layer1g;
	int    b_tmp,Layer1b;
	float  rr,gg,bb;
	double Layer2;
	int    num = 0;
	
	for( int y = 0; y < height; y++ ) {    
		for( int x = 0; x < width; x++, num++ ) {
			if( GetTag(x,y,CURVEBIT) ) {            
				Layer1r = r[num] ; 
				Layer1g = g[num] ; 
				Layer1b = b[num] ;
				// get intensity from curve at this pixel
				Layer2  = (double)curv[num]/255.0;
				
				r_tmp=(int)( (Layer1r*(1-Layer2)) + ( curve_r*Layer2));	
				g_tmp=(int)( (Layer1g*(1-Layer2)) + ( curve_g*Layer2));	
				b_tmp=(int)( (Layer1b*(1-Layer2)) + ( curve_b*Layer2));	
				
				r[num]=Layer1r = (r_tmp >255) ? 255 : r_tmp;
				g[num]=Layer1g = (g_tmp >255) ? 255 : g_tmp;
				b[num]=Layer1b = (b_tmp >255) ? 255 : b_tmp;
				
				DelTag(x,y,CURVEBIT);
				SetTag(x,y,DATABIT);
				
				rr = Layer1r/255.0;
				gg = Layer1g/255.0;
				bb = Layer1b/255.0;				
			}
		}
	}    
}
