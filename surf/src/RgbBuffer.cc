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


#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include<iostream>
#include<cstring>

#include <def.h>
#include <compfn.h>
#include <color.h>
#include <RgbBuffer.h>
#include <oct_quan.h>
#include <RGBToNetscape.h>
#include <ScriptVar.h>

#include <assert.h>

RgbBuffer::RgbBuffer()
	: width(0), height(0),
	  r(0), g(0), b(0), map(0), curv(0), tag(0), n(0), nmap(0)
{
}

RgbBuffer::RgbBuffer(int w, int h)
	: width(w), height(h),
	  r(new byte[w*h]), g(new byte[w*h]), b(new byte[w*h]),	
	  map(new byte[w*h]), curv(new byte[w*h]),
	  tag(new byte[w*h]),         
	  n(w*h), nmap(0)	 	 
           
{
}

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
	std::memset(tag, 0, n*sizeof(byte));
}

void RgbBuffer::clearCurveTags()
{
	for(int i = 0; i < n; i++) {
		tag[i] &= ~(CURVELINEBIT | CURVEBIT);
	}
}

void RgbBuffer::Realloc(int w, int h)
{
	n = w*h;
	width = w;
	height = h;

	delete [] r;
	delete [] g;
	delete [] b;
	delete [] map;
	delete [] curv;
	delete [] tag;			
    					
	r    = new byte[n];
	g    = new byte[n];
	b    = new byte[n];
	map  = new byte[n];
	curv = new byte[n];
	tag  = new byte[n];            
}


// set the whole buffer equal f:

RgbBuffer& RgbBuffer::operator=(byte f)
{
	if(f>255) {
		f=255;
	}

	for(int i = 0; i < n; i++) {
		r[i] = f;
		g[i] = f;
		b[i] = f;
		curv[i] = f;    
	}
	return *this;
}

void RgbBuffer::ClearBuffer()
{
	std::memset(r, ScriptVar::color_background_data[0], n*sizeof(byte));
	std::memset(g, ScriptVar::color_background_data[1], n*sizeof(byte));
	std::memset(b, ScriptVar::color_background_data[2], n*sizeof(byte));
	std::memset(curv, 0, n*sizeof(byte));
	std::memset(tag, 0, n*sizeof(byte));
	
/*	for(int i = 0; i < n; i++) {
		r[i] = ScriptVar::color_background_data[0];
		g[i] = ScriptVar::color_background_data[1];
		b[i] = ScriptVar::color_background_data[2];
		curv[i] = 0;
		tag[i] = 0;
	}
*/
}


// copy values of another rgb buffer:

RgbBuffer& RgbBuffer::operator=(const RgbBuffer& a)
{
	for(int i = 0; i < n; i++) {
		r[i] = a.r[i]; 
		g[i] = a.g[i]; 
		b[i] = a.b[i];
		map[i] = a.map[i];
		curv[i] = a.curv[i];
		tag[i]  = a.tag[i];
	}
	return *this;
}


// copy from a rgb buffer with maybe different size:


void RgbBuffer::Copy(const RgbBuffer& alt)
{
	int xm = MIN(width, alt.width);
	int ym = MIN(height, alt.height);

	for(int y = 0; y < ym; y++) {
		for(int x = 0; x < xm; x++) {
			r[y*width + x] = alt.r[y*width + x];
			g[y*width + x] = alt.g[y*width + x];
			b[y*width + x] = alt.b[y*width + x];
			map[y*width + x]  = alt.map[y*width + x];
			curv[y*width + x] = alt.curv[y*width + x];
			tag[y*width + x]  = alt.tag[y*width + x];        
		}
	}
}


// copy one line of a rgb buffer with maybe different size:

void RgbBuffer::CopyLine(const RgbBuffer& alt, int src, int dest)
{
	if(dest >= 0 && dest < height) {
		int xm = MIN(width, alt.width);

		for(int x = 0; x < xm; x++) {
			r[dest*width + x] = alt.r[src*width + x];
			g[dest*width + x] = alt.g[src*width + x];
			b[dest*width + x] = alt.b[src*width + x];
			map[dest*width + x] = alt.map[src*width + x];
			curv[dest*width + x] = alt.curv[src*width + x];
			tag[dest*width + x] = alt.tag[src*width + x];       
		}
	}
}



// Get a color on different ways:

// Get one color at pixel number:
byte RgbBuffer::Get_one(int num, int color)  const
{
	if(num<0 || num >= n) {
		return 0;
	}
            
	switch (color) {
        case 0:
		return r[num];
        case 1:
		return g[num];
        case 2:
		return b[num];
        case 3:
		return map[num];
        default :
		return 0;
	}
}

//  Get one color at pixel x,y:
byte RgbBuffer::Get_one(int x, int y , int color) const
{
	if(x < 0 || x >= width || y < 0 || y >= height) {
		return 0;
	}
        
	switch (color) {
        case 0:
		return r[y*width + x];
        case 1:
		return g[y*width + x];
        case 2:
		return b[y*width + x];
        case 3:
		return map[y*width + x];
        default :
		return 0;
	}
}

// Set a color or all on different ways at a pixel:

//  Set all colors at pixel number:
void RgbBuffer::Set(int num, int value0, int value1, int value2)
{
	if(num >= 0 && num < n) {
		r[num] = byte(value0);
		g[num] = byte(value1);
		b[num] = byte(value2);
	}
}

//  Set all colors at pixel x,y:
void RgbBuffer::Set(int x, int y, int value0, int value1, int value2)
{
	if(x >= 0 && x < width && y >= 0 && y < height) {
		r[y * width + x] = (value0>255.0) ? 255 : value0;  	
		g[y * width + x] = (value1>255.0) ? 255 : value1;  	
		b[y * width + x] = (value2>255.0) ? 255 : value2;  	
	}
}

//  Set one color at pixel number:
void RgbBuffer::Set_one(int num, int value, int color)
{
	if(num >= 0 && num < n) {  
		switch (color) {
		case 0:
			r[num] = byte(value);
			break;
		case 1:
			g[num] = byte(value);
			break;
		case 2:
			b[num] = byte(value);
			break;
		case 3:
			map[num] = byte(value);
			break;
		default:
			break;     
		}
	}
}

//  Set one color at pixel x,y:
void RgbBuffer::Set_one(int x, int y, int value, int color)
{
	if(x >= 0 && x < width && y >= 0 && y < height) {  
		switch (color) {
		case 0:
			r[y * width + x] = byte(value);
			break;
		case 1:
			g[y * width + x] = byte(value);
			break;
		case 2:
			b[y * width + x] = byte(value);
			break;
		case 3:
			map[y * width + x] = byte(value);
			break;
		default:
			break;
		}
	}
}

  
// ---- multiply element with a value on different ways:

//  Multiply one value with all colors at pixel number:
void RgbBuffer::Mult(int num, float value)
{
	if(num >= 0 && num < n) {
		r[num] = byte(r[num]*value);
		g[num] = byte(g[num]*value);
		b[num] = byte(b[num]*value);
	}
}

//  Multiply one value with all colors at pixel x,y:
void RgbBuffer::Mult(int x, int y, float value)
{
	if(x >= 0 && x < width && y >= 0 && y < height) {
		r[y * width + x] = byte(r[y * width + x]*value);
		g[y * width + x] = byte(g[y * width + x]*value);
		b[y * width + x] = byte(b[y * width + x]*value);
	}
}

float RgbBuffer::Get_Gray_value(int x, int y)
{
	if(x < 0 || x >= width || y < 0 || y >= height) {
		return 0.0;
	}

	int n = y*width + x;
	return (0.299*r[n] + 0.587*g[n] + 0.114*b[n])/255.0;
}

// set element to a max value if greater:

void RgbBuffer::CutAtMax(int x, int y, byte value)
{
	if(x >= 0 && x < width && y >= 0 && y < height) {
		int i = y*width + x;
		if(r[i] > value) {
			r[i] = value;
		}
		if(g[i] > value) {
			g[i] = value;
		}
		if(b[i] > value) {
			b[i] = value;
		}
	}
}

// ----------------------------------------------------------------------------
// get, set or del a flag at pixel x,y ,
// use single bit of a byte to save a flag-infos(On/Off) at definite position 
// ----------------------------------------------------------------------------

bool RgbBuffer::GetTag(int x, int y, int position) const     
{
	if(x < 0 || x >= width || y < 0 || y >= height) {
		return false;
	}		
  
	return tag[y*width + x] & position;
}

void RgbBuffer::SetTag(int x, int y, int position)
{
	if(x < 0 || x >= width || y < 0 || y >= height) {
		return;
	}
	
	tag[y*width + x] |= position;
}

void RgbBuffer::DelTag(int x, int y, int position) 
{
	if(x < 0 || x >= width || y < 0 || y >= height) {
		return;
	}

	tag[y*width + x] &= ~position;
}

// set value of curve intensity at pixel x,y:

void RgbBuffer::SetLayerTwo(int x, int y, int value)
{
	if(x >= 0 && x < width && y >= 0 && y < height) {  
		value = (value > 255) ? 255 : value;
		curv[y*width + x] = value;  
	}
}


// set value of curve intensity at pixel x,y ; if greater than old value:

void RgbBuffer::SetLayerTwoIfHigher(int x, int y, int value) 
{
	if(x >= 0 && x < width && y >= 0 && y < height) {
		int i = y*width + x;
		
		if(value > curv[i]) {
			curv[i] = (value > 255) ? 255 : byte(value);
		}
		
		SetTag(x, y, CURVEBIT);
		SetTag(0, y, CURVELINEBIT);
	}
}


// get intensity of curve at pixel x,y:

byte RgbBuffer::GetLayerTwo(int x, int y) const     
{
	if(x < 0 || x >= width || y < 0 || y >= height) {
		return 0;
	}

	return curv[y*width + x];
}


//  map the RGB colors to the Netscape 216 color cube (6,6,6):


void RgbBuffer::NetscapeColor()
{
	//  Map the RGB values:

	int i = 0;
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			map[i] = RGBToNetscape(float(r[i])/255.0,
					       float(g[i])/255.0,
					       float(b[i])/255.0, x, y);
			i++;
		}
	}

	//  Create the Netscape colormap:

	i = 0;
	nmap = 216;
	for(int rcount = 0; rcount < 6; rcount++) {
		for(int gcount = 0; gcount < 6; gcount++) {
			for(int bcount = 0; bcount < 6; bcount++) {
				rmap[i] = byte(rcount*51);
				gmap[i] = byte(gcount*51);
				bmap[i] = byte(bcount*51);
				i++; 
			}
		}
	}
}

//  map the RGB colors to 216 optimized colors:

void RgbBuffer::OptimizedColor(bool dither, double ditherval)
{
	//  Map the RGB values:
	
	// Trees.OctQuant( );
	OctQuant Trees;
	Trees.width = width;
	Trees.height = height;
	Trees.BuildTree(*this, dither, ditherval);
	int anull = 0;
	Trees.FillPalette(Trees.proot, &anull);
	
	int i = 0;
	OctColor rgb;
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			rgb.SetRed(r[i]);
			rgb.SetGreen(g[i]);
			rgb.SetBlue(b[i]);
			
			if(dither) {
				rgb.Dither(x, y, ditherval);
			}
			
			map[i] = Trees.QuantizeColor(Trees.proot, rgb);
			i++;
		}
	}

	//  Copy the optimized colormap:

	for(i = 0; i < 216; i++) {
		rmap[i] = Trees.Palette[i].GetRed();
		gmap[i] = Trees.Palette[i].GetGreen();
		bmap[i] = Trees.Palette[i].GetBlue();
	}
	
	nmap=216;
}


void RgbBuffer::StereoLeft()
{
	for(int i = 0; i < n; i++) {             
		map[i] = int(0.299*float(r[i])
			     + 0.587*float(g[i])
			     + 0.114*float(b[i]));
	}
}

void RgbBuffer::StereoRight(float rm, float gm, float bm, int dist, int back)
{
	for(int i = 0; i < n; i++) {             
		curv[i] = int(0.299*float(r[i])
			      + 0.587*float(g[i])
			      + 0.114*float(b[i]));
	}             

	for(int y = 0; y < height; y++) {
		for(int x = 0, xl = -dist, xr = dist;
		    x < width;
		    x++, xl++, xr++) {
			int i = width*y + x;
			int il = i - dist;
			int ir = i + dist;

			r[i] = (xr < width ? byte(rm*float( map[ir])) : back);
			g[i] = (xl >=0     ? byte(gm*float(curv[il])) : back);
			b[i] = (xl >=0     ? byte(bm*float(curv[il])) : back);
		}
	}
}

void RgbBuffer::AddCurve(int curve_r, int curve_g, int curve_b)
{
	int num = 0;
	
	for(int y = 0; y < height; y++) {    
		for(int x = 0; x < width; x++, num++) {
			if(GetTag(x, y, CURVEBIT)) {
				int Layer1r = r[num] ; 
				int Layer1g = g[num] ; 
				int Layer1b = b[num] ;
				// get intensity from curve at this pixel
				double Layer2  = double(curv[num]/255.0);
				
				int r_tmp = int((Layer1r*(1 - Layer2)) + (curve_r*Layer2));
				int g_tmp = int((Layer1g*(1 - Layer2)) + (curve_g*Layer2));
				int b_tmp = int((Layer1b*(1 - Layer2)) + (curve_b*Layer2));
				
				r[num] = Layer1r = (r_tmp >255) ? 255 : r_tmp;
				g[num] = Layer1g = (g_tmp >255) ? 255 : g_tmp;
				b[num] = Layer1b = (b_tmp >255) ? 255 : b_tmp;

				DelTag(x, y, CURVEBIT);
				SetTag(x, y, DATABIT);
			}
		}
	}
}
