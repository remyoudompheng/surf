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


#include <strstream.h>

#include "SaveImageDialog.h"

void SaveImageDialog::toggled_dither_method (GtkWidget *w, gpointer data)
{
	SaveImageDialog *sid = (SaveImageDialog *)data;
	sid->ditherChanged (w);
}

void SaveImageDialog::ditherChanged(GtkWidget *w)
{
        if( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w))) {
                gtk_widget_set_sensitive(ditherSteps,FALSE);
	} else{
		gtk_widget_set_sensitive(ditherSteps,TRUE);
	}
}

void SaveImageDialog::hide_dither_method( GtkWidget *w, gpointer data )
{
          gtk_widget_set_sensitive((GtkWidget *)data,FALSE);
}

void SaveImageDialog::show_dither_method( GtkWidget *w, gpointer data )
{
          gtk_widget_set_sensitive((GtkWidget *)data,TRUE);
}

char *SaveImageDialog::generateScript()
{
	SymbolTable st;
	wrw.writeSymbols (st);
	ostrstream ostr;
	ostr << st;
	ostr << "filename = \"" << gtk_file_selection_get_filename(getGtkFileSelection()) << "\";" << endl
	     << (type==color ? "save_color_image;" : "save_dithered_image;") << endl
	     << ends;
	return ostr.str();
}

SaveImageDialog::SaveImageDialog ()
{
	fileselectiondialog = gtk_file_selection_new ("Save image...");
	gtk_signal_connect (GTK_OBJECT(fileselectiondialog), "delete_event", GTK_SIGNAL_FUNC(handle_delete), this);


	
	GtkWidget *box = gtk_hbox_new (TRUE, 10);
	GtkWidget *xbox = gtk_vbox_new (FALSE, 10);
	colorBox   = gtk_vbox_new (FALSE, 2);
	bwBox      = gtk_vbox_new (FALSE, 2);
        ditherBox  = gtk_hbox_new( FALSE,0 );
        ditherBBox = gtk_hbox_new( FALSE,0 );
			
	gtk_box_pack_start (GTK_BOX(box), colorBox, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX(box), bwBox,    TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX(xbox), box,    TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX(ditherBBox), ditherBox, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX(xbox), ditherBBox,    TRUE, TRUE, 0);
			
	OptionMenuBuilder omb;
	omb.addOption ("portable pixmap (.ppm)", "ppm");
	omb.addOption ("sun rasterfile (.ras)" , "sun");
	omb.addOption ("X window dump (.xwd)"  , "xwd");
	omb.addOption ("JPEG file (.jpg)", "jpg");
			
	GtkWidget *om=omb.getOptionMenu();
	gtk_box_pack_start (GTK_BOX(colorBox), om, TRUE, TRUE, 0);
	wrw.addWidget (om, "color_file_format");
	
	GtkWidget * trueColor = omb.addOption ("true color"        , "TrueColor");
	GtkWidget * netscape  = omb.addOption ("netscape colormap" , "Netscape" );
	GtkWidget * optimized = omb.addOption ("optimized colormap", "Optimized");

        gtk_signal_connect (GTK_OBJECT (trueColor),"activate",(GtkSignalFunc)hide_dither_method,GTK_OBJECT(ditherBox));
        gtk_signal_connect (GTK_OBJECT (netscape), "activate",(GtkSignalFunc)hide_dither_method,GTK_OBJECT(ditherBox));
        gtk_signal_connect (GTK_OBJECT (optimized),"activate",(GtkSignalFunc)show_dither_method,GTK_OBJECT(ditherBox));

	om=omb.getOptionMenu();
	gtk_box_pack_start (GTK_BOX(colorBox), om, TRUE, TRUE, 0);
	wrw.addWidget (om, "color_file_colormap");

	omb.addOption ("portable bitmap (.pbm)"        , "pbm");
	omb.addOption ("postscript (.ps)"              , "postscript");
	omb.addOption ("encapsulated postscript (.eps)", "encapsulated");
	omb.addOption ("X11 bitmap (.xbm)"             , "X11_bitmap");
	omb.addOption ("Tiff (.tiff)"                  , "tiff");
	omb.addOption ("portable graymap (.pgm)"       , "pgm");
	om=omb.getOptionMenu();
	gtk_box_pack_start (GTK_BOX(bwBox), om, TRUE, TRUE, 0);
	wrw.addWidget (om, "dither_file_format");

	omb.addOption ("75 dpi"  , "res_75dpi"  );
	omb.addOption ("100 dpi" , "res_100dpi" );
	omb.addOption ("150 dpi" , "res_150dpi" );
	omb.addOption ("300 dpi" , "res_300dpi" );
	omb.addOption ("600 dpi" , "res_600dpi" );
	omb.addOption ("1200 dpi", "res_1200dpi");

	gtk_box_pack_start (GTK_BOX(bwBox), om=omb.getOptionMenu(), TRUE, TRUE, 0);
	
	gtk_option_menu_set_history (GTK_OPTION_MENU(om), 3);
 	gtk_menu_set_active (GTK_MENU(gtk_option_menu_get_menu(GTK_OPTION_MENU(om))), 3);

	wrw.addWidget (om, "resolution");

	ditherColors = gtk_check_button_new_with_label ("dither colors:");
	gtk_box_pack_start (GTK_BOX(ditherBox), ditherColors, FALSE, FALSE,0);
	ditherSteps = mygtk_new_spin (20.0, 0.01, 255.0, 0.01, 1.0, 0.1, 1);
	gtk_widget_set_sensitive (ditherSteps, FALSE);
	gtk_box_pack_start (GTK_BOX(ditherBox), ditherSteps, TRUE, TRUE, 0);
        gtk_signal_connect( GTK_OBJECT(ditherColors),"pressed",(GtkSignalFunc)toggled_dither_method,this );

	wrw.addWidget (ditherColors, "dither_colors");
	wrw.addWidget (ditherSteps, "dither_steps");

	gtk_box_pack_start    (GTK_BOX(GTK_FILE_SELECTION(fileselectiondialog)->main_vbox), xbox, FALSE, FALSE, 0);
	gtk_box_reorder_child (GTK_BOX(GTK_FILE_SELECTION(fileselectiondialog)->main_vbox), xbox, 0);
}

void SaveImageDialog::show(SaveImageDialog::imageType t)
{
	type = t;
	if (t == color) {
		gtk_window_set_title (GTK_WINDOW(fileselectiondialog), "Save color image as...");
		gtk_widget_set_sensitive (colorBox, TRUE);
		gtk_widget_set_sensitive (bwBox, FALSE);
		gtk_widget_set_sensitive (ditherBBox, TRUE);
	} else {
		gtk_window_set_title (GTK_WINDOW(fileselectiondialog), "Save dithered image as...");
		gtk_widget_set_sensitive (colorBox, FALSE);
		gtk_widget_set_sensitive (bwBox, TRUE);
		gtk_widget_set_sensitive (ditherBBox, FALSE);
	}
	gtk_widget_show_all (fileselectiondialog);
	gdk_window_raise(fileselectiondialog->window);
// 	gtk_menu_set_active (GTK_MENU(gtk_option_menu_get_menu(GTK_OPTION_MENU(om))), 2);

	// gtk_option_menu_set_history (GTK_OPTION_MENU(om), 3);
}

gint SaveImageDialog::handle_delete (GtkWidget *widget, GdkEvent *event, gpointer data)
{
	gtk_widget_hide (widget);
	return TRUE;
}
