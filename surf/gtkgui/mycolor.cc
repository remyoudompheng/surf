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



#include <assert.h>
#include <iostream.h>
#include <stdlib.h>
#include <strstream.h>

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkprivate.h>
#include <gdk/gdkx.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/Xmu/StdCmap.h>

#include "mycolor.h"
#include "Misc.h"

// These are not declared static...they get used in SurfDrawingArea...
Display * xdisplay        = 0;
XStandardColormap *xstandardColormap = 0;
unsigned long pixels[216];
ColorMode myColorMode;




static void inspectVisual (GdkVisual *visual);

static GdkColormap * init_netscape_color (GdkVisual *);
static GdkColormap * init_true_color     (GdkVisual *);
static GdkColormap * init_direct_color   (GdkVisual *);

static int alloc_netscape_colors (GdkColormap *cm);

static const char * visual_type_as_string (GdkVisual *visual)
{
	assert(visual);
	switch (visual->type) {
	case GDK_VISUAL_STATIC_GRAY : return "static gray";
	case GDK_VISUAL_GRAYSCALE   : return "grayscale";  
	case GDK_VISUAL_STATIC_COLOR: return "static color";
	case GDK_VISUAL_PSEUDO_COLOR: return "pseudo color";
	case GDK_VISUAL_TRUE_COLOR  : return "true color";  
	case GDK_VISUAL_DIRECT_COLOR: return "direct color";
	}
	return "unknown visual";
}

static void inspectVisual(GdkVisual *visual)
{
	assert(visual);
#if 0
	cout << "Type         :" << visual_type_as_string(visual) << endl
	     << "depth        :" << visual->depth                 << endl
	     << "colormap_size:" << visual->colormap_size         << endl
	     << "bits_per_rgb :" << visual->bits_per_rgb          << endl;
#endif
}


GdkVisual *my_get_best_visual ()
{
	GdkVisual *visual;
	
	if ((visual = gdk_visual_get_best_with_both(24, GDK_VISUAL_TRUE_COLOR)))
		return visual;

	if ((visual = gdk_visual_get_best_with_both(16, GDK_VISUAL_TRUE_COLOR)))
		return visual;

	if ((visual = gdk_visual_get_best_with_both(15, GDK_VISUAL_TRUE_COLOR)))
		return visual;
	
	if ((visual = gdk_visual_get_best_with_type (GDK_VISUAL_PSEUDO_COLOR)))
		return visual;
	
	if ((visual = gdk_visual_get_best_with_type (GDK_VISUAL_STATIC_COLOR)))
		return visual;
	
	return 0;
}


int initColors()
{
	GdkVisual     *defaultVisual   = 0;
	GdkColormap   *defaultColormap = 0;


	xdisplay = GDK_DISPLAY();
	
// 	GdkVisual *tmp = gdk_visual_get_system();
// 	cout << "system visual is" << endl;
// 	inspectVisual (tmp);
// 	cout << endl;

// 	tmp = gdk_visual_get_best();
// 	cout << "Best visual is" << endl;
// 	inspectVisual (tmp);
// 	cout << endl;

	
	defaultVisual = my_get_best_visual();

// 	cout << "Using visual" << endl;
// 	inspectVisual(defaultVisual);
// 	cout << endl;
	
	GdkColormap* (*func) (GdkVisual *);

	switch (defaultVisual->type) {
	case GDK_VISUAL_PSEUDO_COLOR:  func=init_netscape_color; myColorMode=COLOR_NETSCAPE; break;
	case GDK_VISUAL_STATIC_COLOR:  func=init_netscape_color; myColorMode=COLOR_NETSCAPE; break;		
	case GDK_VISUAL_TRUE_COLOR:    func=init_true_color;     myColorMode=COLOR_TRUE;     break;

	default:                       func=0; break;
	}

	if (func == 0) {
		Misc::alert ("Your visual is not supported...giving up");
		exit(10);
	}

	defaultColormap = func(defaultVisual);

	gtk_widget_set_default_visual(defaultVisual);
	gtk_widget_set_default_colormap (defaultColormap);
	return 0;
}


static GdkColormap *init_true_color(GdkVisual *visual)
{
	assert(visual);
	assert(visual->type == GDK_VISUAL_TRUE_COLOR);
	
	int count;
	if (XGetRGBColormaps (xdisplay, RootWindow(xdisplay, DefaultScreen(xdisplay)), 
			      &xstandardColormap, &count, XA_RGB_BEST_MAP)==0)  {
		if( XmuVisualStandardColormaps( xdisplay, DefaultScreen(xdisplay), 
						((GdkVisualPrivate *)visual)->xvisual->visualid,
						visual->depth, False, True ) == 0  ) {
			Misc::alert ("XmuVisualStandardColormaps call failed\ngiving up...");
			exit( 10 );
		}
		
		cout << "created a new XA_RGB_BEST_MAP definition" << endl;
		if( XGetRGBColormaps(xdisplay,
				     RootWindow(xdisplay, DefaultScreen(xdisplay)),&(xstandardColormap),
				     &count,XA_RGB_BEST_MAP)==0) {
			Misc::alert ("XGetRGBColormaps call failed\ngiving up");
			exit( 10 );
		}
	}
	
	cout << "found " << count << " definition(s) for XA_RGB_BEST_MAP" << endl;
	
	if( count > 0 ) {
		// -------------------------------
		//  allocate the StandardColormap
		// -------------------------------
		
		if( XmuCreateColormap( xdisplay, xstandardColormap ) == 0 ) {
			Misc::alert ("XmuCreateColormap call failed\ngiving up");
			exit( 10 );
		}
		
		cout << "StandardColormap XA_RGB_BEST_MAP created" << endl;
	} else {
		Misc::alert ("Could not get any Standard Colormap definition\ngiving up...");
		exit( 10 );
	}		
	
	return gdk_colormap_new(visual,0);
}



static GdkColormap * init_netscape_color(GdkVisual *visual)
{
	int count;
	assert(visual);

	GdkColormap *cmap = gdk_colormap_get_system();
	if ((count=alloc_netscape_colors(cmap))==216) {
// 		cout << "allocated 216 colors from system colormap" << endl;
		return cmap;
	}

#ifdef DEBUG
	cout << "only got " << count << " out of 216 colors...trying private colormap" << endl;	
#endif

	cmap = gdk_colormap_new (visual,0);
	count = alloc_netscape_colors(cmap);

	if (count != 216) {
		ostrstream ostr;
		ostr << "private colormap allocation failed...only got " 
		     << count << " out of 216 colors...giving up";
		Misc::alert (ostr);
		exit(10);
	} 

	return cmap;
}




/* returns number of allocated colors */
static int alloc_netscape_colors (GdkColormap *cm)
{
	assert(cm);
	Colormap xcm = ((GdkColormapPrivate*)cm)->xcolormap;

	Display *xdisplay = ((GdkColormapPrivate*)cm)->xdisplay;
	// xcm = XCopyColormapAndFree(xdisplay, xcm);
	assert(xdisplay);

	int r;
	int g;
	int b;
	XColor xcolor;
	int c=0;
	for (r=0; r<6; r++) {
		for (g=0; g<6; g++) {
			for (b=0; b<6; b++) {
				// xcolor.pixel = 0;

				xcolor.red   = 51*r*256;
				xcolor.green = 51*g*256;
				xcolor.blue  = 51*b*256;
				if (XAllocColor (xdisplay, xcm, &xcolor) == 0) {
					if (c > 0)
						XFreeColors(xdisplay, xcm, pixels, c, 0);
					return c;
				}
				pixels[c] = xcolor.pixel;
				c++;
			}
		}
	}
	
	return c;
}
