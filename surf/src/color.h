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




#ifndef COLOR_H
#define COLOR_H

// ----------------------------------------------------------------------------
//  Klasse für Farben mit den Intensitäten rgb vom Datentyp double, 
//         die die Farbwerte während der Berechnung eines Pixels speichert.  
// ----------------------------------------------------------------------------

class colorrgb
{
public:

	double red;    
	double green;  
	double blue;   
   

	unsigned char getRedByte() const
		{
			return red >= 1.0 ? 255: (int) (red*255);
		}

	unsigned char getGreenByte() const
		{
			return green >= 1.0 ? 255: (int) (green*255);
		}

	unsigned char getBlueByte() const
		{
			return blue >= 1.0 ? 255: (int) (blue*255);
		}

        double   getBW( ) const
	        {
                        return  0.299*red+0.587*green+0.114*blue;
		}
	void operator= (double c)
		{ red = green = blue = c;}

	void operator *= (double f)
		{ red *= f; green *= f; blue *= f;}

	void operator += (const colorrgb &c)
		{ red += c.red; green += c.green; blue += c.blue;}

	void set (double r, double g, double b)
		{ red = r; green = g; blue = b;}

	void setBlack()
		{ red = green = blue = 0.0;}

	void add (double r, double g, double b)
		{ red+=r; green+=g; blue+=b;}

	void cutAtMax ()
		{
			if (red   > 1.0)
				red   = 1.0;

			if (green > 1.0)
				green = 1.0;

			if (blue  > 1.0)
				blue  = 1.0;
		}
};


#endif
