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







// ----------------------------------------------------------------------------
//  File                rgb_buffer.H
//  implementation by kai
//  date                25.06.97
//  sk :ganzer File neu
// ----------------------------------------------------------------------------

#ifndef RGBBUFFER_H
#define RGBBUFFER_H


//sk :Namen für die Flags, die im TAG-Byte(siehe rgb_buffer) ...
//    an jedem Pixel gesetzt werden können

#define CURVEBIT 	(1<<1)
#define SURFBIT         (1<<2)
#define CURVELINEBIT    (1<<3)
#define DATABIT         (1<<4)

#define CURVEBIT3D 	(1<<5)
#define SURFBIT3D       (1<<6)
#define CURVELINEBIT3D  (1<<7)
#define DATABIT3D       (1<<0)

#include <math.h>
#include <stdio.h>

#include "color.h"
#include "gui_enum.h"

#include "ImageFormats.h"

#include "mytypes.h"


// ----------------------------------------------------------------------------
//  Image buffer for intensity rgb values (image in memory) and curve intensity
//          plus one byte for different flags
// ----------------------------------------------------------------------------

class RgbBuffer
{
private:
	RgbBuffer (const RgbBuffer &);

public:
        RgbBuffer();
        RgbBuffer( int, int );
        ~RgbBuffer();


	void clearTags();
	void clearCurveTags();

	int getWidth() const { return width; }
	int getHeight() const { return height; }
	byte* getRData() const { return r; }
	byte* getGData() const { return g; }
	byte* getBData() const { return b; }
	const byte* getMap() const { return map; }
	const byte* getRMap() const { return rmap; }
	const byte* getGMap() const { return gmap; }
	const byte* getBMap() const { return bmap; }
	int getNumCols() const { return nmap; }
	
        RgbBuffer& operator=( const RgbBuffer& );
        RgbBuffer& operator=( byte );
  
        void    Set_whole( int, int ,int );		//skbackgr

        void   Realloc( int, int );
        void   NullInit();
        void   NullInit_one();
        void   NullInit_two();
        void   NullInit_three();
  
        void    Copy( const RgbBuffer& );
        void    CopyLine( const RgbBuffer&, int, int );

	void GetPixelColor (int x, int y, colorrgb &c)
		{
			if( x<0 || x>=width || y<0 || y>=height ) {
				c.setBlack();
				return;
			}
			
			int n = y*width+x;
			c.set (r[n]/255.0, g[n]/255.0, b[n]/255.0);
		}
	
	double GetPixelColorR (int x, int y)
		{
			if( x<0 || x>=width || y<0 || y>=height ) {
				return 0.0;
			}
			
			int n = y*width+x;
			return r[n]/255.0;
		}
	double GetPixelColorG (int x, int y)
		{
			if( x<0 || x>=width || y<0 || y>=height ) {
				return 0.0;
			}
			
			int n = y*width+x;
			return g[n]/255.0;
		}
	double GetPixelColorB (int x, int y)
		{
			if( x<0 || x>=width || y<0 || y>=height ) {
				return 0.0;
			}
			
			int n = y*width+x;
			return b[n]/255.0;
		}

	void SetPixelColor (int x, int y, const colorrgb &c)
		{
			if( x<0 || x>=width || y<0 || y>=height ) {
				return;
			}
			
			int n = y*width+x;
			r[n] = c.getRedByte();
			g[n] = c.getGreenByte();
			b[n] = c.getBlueByte();
		}
	void SetPixelColorR (int x, int y, const colorrgb &c)
		{
			if( x<0 || x>=width || y<0 || y>=height ) {
				return;
			}
			
			int n = y*width+x;
			r[n] = c.getRedByte();
		}
	void SetPixelColorGB (int x, int y, const colorrgb &c)
		{
			if( x<0 || x>=width || y<0 || y>=height ) {
				return;
			}
			
			int n = y*width+x;
			g[n] = c.getGreenByte();
			b[n] = c.getBlueByte();
		}

        byte    Get_one( int , int  ) const;
        byte    Get_one( int , int   , int ) const;
  
        void    Set( int , int , int ,int );
        void    Set( int , int , int , int , int );
        void    Set_one( int , int ,int );
        void    Set_one( int , int , int ,int  );

        void    Mult( int, float );
        void    Mult( int, int, float );

        float   Get_Gray_value(int , int );

        void    CutAtMax( int, int, byte );

        bool    GetTag( int, int,int) const; 
        void    SetTag( int, int,int) ;      
        void    DelTag( int, int,int) ;      

        byte 	GetLayerTwo( int, int ) const;   
        void 	SetLayerTwo( int, int ,int ) ;   
     
        void    SetLayerTwoIfHigher( int, int, int );
	void    AddCurve(int curve_r, int curve_g, int curve_b);


        void    StereoLeft( void );
        void    StereoRight( float,float,float,int,int );

public:
        void NetscapeColor();
        void OptimizedColor(bool dither, double ditherval);

protected:
        int width;
        int height;

        byte *r;           // red bytes
        byte *g;           // green bytes
        byte *b;           // blue bytes
        byte *map;         // colormap bytes
        byte *curv;        // curve bytes
	
        byte *tag;         // tag bits
        int n;             // length
	
        byte rmap[256];
        byte gmap[256];
        byte bmap[256];
	
        int  nmap;
	
	friend class DrawingArea;
};

#endif // RGBBUFFER_H
