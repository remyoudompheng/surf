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




#ifndef FLOAT_BUFFER_H
#define FLOAT_BUFFER_H

// ----------------------------------------------------------------------------
//  Image buffer for intensity float values (image in memory)
// ----------------------------------------------------------------------------

class float_buffer
{
private:
	float_buffer (const float_buffer &);
	void operator=(const float_buffer &);
public:
	
        float_buffer();
        float_buffer( int, int );
        ~float_buffer();
	
        float_buffer& operator=( float );

        void Realloc( int, int );
        void NullInit();


	float   Get( int x, int  y ) const
		{
			if( x < 0 || x >= width || y < 0 || y >= height )
				return 0.0;
			return b[ y*width + x ];
		}
	void    Set( int x, int y, float value)
		{
			if( x >= 0 && x < width && y >= 0 && y < height )
				b[ y * width + x ] = value;
		}

	void    Add( int x, int y, float value)
		{
			if( x >= 0 && x < width && y >= 0 && y < height )
				b[ y * width + x ] += value;
		}

	void    CorrectGamma( double );
	void    EnhanceEdges( double);
	void    AdjustToneScale();

	int getWidth  () const { return width;  };
	int getHeight () const { return height; };


protected:
	float   GetAbsFieldModuloHeight (int, int );
        void    Copy (const float_buffer &);
        void    CopyLine (const float_buffer&, int, int );

protected:
        float *b;
        int n;

        int width;
        int height;

};

#endif
