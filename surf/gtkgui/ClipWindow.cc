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


#include <string.h>
#include <iostream.h>
#include "ClipWindow.h"
#include "RadioButtonBuilder.h"
#include "OptionMenuBuilder.h"
#include "TableBuilder.h"

void ClipWindow::toggled_clip_method( GtkWidget *w,gpointer data )
{
  	ClipWindow *cw = (ClipWindow*)data;
  	cw->clipMethodChanged( w );
}

void ClipWindow::clipMethodChanged( GtkWidget *w ) 
{
        if( gtk_menu_get_active( GTK_MENU( gtk_option_menu_get_menu( GTK_OPTION_MENU( clip_method ) ) ) ) == w )
	{
	    const char *val = (const char*)gtk_object_get_data( GTK_OBJECT( w ),"value" );

            if( !strncmp( val,"none",4 ) )
	    {
                gtk_widget_set_sensitive( radius,  FALSE );
                gtk_widget_set_sensitive( center_x,FALSE );
                gtk_widget_set_sensitive( center_y,FALSE );
                gtk_widget_set_sensitive( center_z,FALSE );
	    }
            else
	    {
                gtk_widget_set_sensitive( radius,  TRUE );
                gtk_widget_set_sensitive( center_x,TRUE );
                gtk_widget_set_sensitive( center_y,TRUE );
                gtk_widget_set_sensitive( center_z,TRUE );
	    }
	}
}

ClipWindow::ClipWindow( )
{
        // mygtk_new_spin( value,lower,upper,inc1,inc2,accel,digits );

	clip_front = mygtk_new_spin(  10.0,-100.0,100.0,0.1,1.0,0.2,4 );
	clip_back  = mygtk_new_spin( -10.0,-100.0,100.0,0.1,1.0,0.2,4 );

	OptionMenuBuilder omb;
	omb.addOption( "sphere",      "sphere"       );
	omb.addOption( "tetrahedron", "tetrahedron"  );
	omb.addOption( "cube",	      "cube"         );
	omb.addOption( "octahedron",  "octahedron"   );
	omb.addOption( "dodecahedron","dodecahedron" );
	omb.addOption( "icosahedron", "icosahedron"  );
	omb.addOption( "cylinder",    "cylinder"     );
	omb.addOption( "user defined","user"         );
	omb.addOption( "none",        "none"         );

	omb.connect_all((GtkSignalFunc)toggled_clip_method,this );

	clip_method = omb.getOptionMenu( );
	controlWidget( clip_method,"clip");

	radius     = mygtk_new_spin(  10.0,   0.0,10000000.0,0.1,1.0,0.2,4 );
	center_x   = mygtk_new_spin(   0.0,-10000000.0,10000000.0,0.1,1.0,0.2,4 );
	center_y   = mygtk_new_spin(   0.0,-10000000.0,10000000.0,0.1,1.0,0.2,4 );
	center_z   = mygtk_new_spin(   0.0,-10000000.0,10000000.0,0.1,1.0,0.2,4 );

	TableBuilder tb;
	tb.setSize( 9 );
	tb.newLine( "clip front:",clip_front );
	tb.newLine( "clip back:" ,clip_back  );

	tb.separator( );

	tb.newLine ("clip at:",clip_method );

	tb.separator( );

	tb.newLine( "radius:"  ,radius   );
	tb.newLine( "center x:",center_x );
	tb.newLine( "center y:",center_y );
	tb.newLine( "center z:",center_z );

        table = tb.getTable( );

	controlWidget( clip_front,"clip_front" );
	controlWidget( clip_back, "clip_back"  );
	controlWidget( radius,    "radius"     );
	controlWidget( center_x,  "center_x"   );
	controlWidget( center_y,  "center_y"   );
	controlWidget( center_z,  "center_z"   );
}
