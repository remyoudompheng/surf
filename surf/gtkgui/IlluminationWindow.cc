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
#include <stdio.h>
#include <assert.h>

#include "def.h"
#include "IlluminationWindow.h"
#include "ColorSelectionButton.h"
#include "init_parser.h"
#include "TableBuilder.h"
#include "OptionMenuBuilder.h"
// #define DEBUG
#include "debug.h"

extern const char *LIGHT_SETTINGS_FIRST_PIC_STRING [MAIN_SURFACE_AMOUNT_NUM][LIGHT_SETTINGS_FIRST_NUM];
extern const char *LIGHT_SETTINGS_SECOND_PIC_STRING[MAIN_SURFACE_AMOUNT_NUM][LIGHT_SETTINGS_SECOND_NUM];
extern const char *LIGHT_VOLUME_PIC_STRING         [LIGHT_SOURCE_MAX_VALUE];
extern const char *LIGHT_POSITION_PIC_STRING       [LIGHT_SOURCE_MAX_VALUE][LIGHT_POSITION_NUM];

void IlluminationWindow::readSymbols (SymbolTable &st)
{
	int j;
	int i;
	for (j=0; j<2; j++) {
		for (i=0; i<MAIN_SURFACE_AMOUNT_NUM; i++) {
			ColorSelectionButton *csb = buttons[j][i];

			double r = st.lookupInt(getColorSliderPicString (i+1, j == 1, 0)) / 255.0;
			double g = st.lookupInt(getColorSliderPicString (i+1, j == 1, 1)) / 255.0;
			double b = st.lookupInt(getColorSliderPicString (i+1, j == 1, 2)) / 255.0;
			buttons[j][i]->setColor (r, g, b);
		}
	}	
	for (i=0; i<LIGHT_SOURCE_MAX_VALUE; i++) {
		ColorSelectionButton *csb = color_buttons[i];

		double r = st.lookupInt(getLightSliderPicString (i+1, 0)) / 255.0;
		double g = st.lookupInt(getLightSliderPicString (i+1, 1)) / 255.0;
		double b = st.lookupInt(getLightSliderPicString (i+1, 2)) / 255.0;
		color_buttons[i]->setColor (r, g, b);
	}	
}


void IlluminationWindow::writeSymbols (SymbolTable &st)
{
	int j;
	int i;
	for (j=0; j<2; j++) {
		for (i=0; i<MAIN_SURFACE_AMOUNT_NUM; i++) {
			ColorSelectionButton *csb = buttons[j][i];
			st.insertInt (getColorSliderPicString (i+1, j == 1, 0), (int) (csb->red    *0xff));
			st.insertInt (getColorSliderPicString (i+1, j == 1, 1), (int) (csb->green  *0xff));
			st.insertInt (getColorSliderPicString (i+1, j == 1, 2), (int) (csb->blue   *0xff));
		}
	}
	for (i=0; i<LIGHT_SOURCE_MAX_VALUE; i++) {
		ColorSelectionButton *csb = color_buttons[i];
		st.insertInt (getLightSliderPicString (i+1, 0), (int) (csb->red    *0xff));
		st.insertInt (getLightSliderPicString (i+1, 1), (int) (csb->green  *0xff));
		st.insertInt (getLightSliderPicString (i+1, 2), (int) (csb->blue   *0xff));
	}


	char value[256];
	strcpy (value, "0");
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(illumination.ambient))) {
		strcat (value, "+ambient_light");
	}

	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(illumination.diffuse))) {
		strcat (value, "+diffuse_light");
	}

	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(illumination.reflected))) {
		strcat (value, "+reflected_light");
	}

	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(illumination.transmitted))) {
		strcat (value, "+transmitted_light");
	}

	st.insert("illumination", value);

}


void IlluminationWindow::init_gradient()
{
	//
	// --- init gradient struct
	//

	
	gradient.button.setName ("gradient color");
	controlWidget (gradient.button.getWidget(), "gradient");
	gtk_signal_connect (GTK_OBJECT(gradient.button.getWidget()),
			    "clicked",
			    (GtkSignalFunc) button_clicked, this);

	OptionMenuBuilder omb;
	omb.addOption ("none", "0");
	omb.addOption ("inside", "1");
	omb.addOption ("outside", "2");
	omb.addOption ("both", "3");
	
	GtkWidget *om = omb.getOptionMenu();
	

	TableBuilder tb;
	tb.setSize(2);	
	tb.newLine ("display gradient:", om);
	controlWidget (om, "gradient");

	GtkWidget *hbox = gtk_hbox_new (FALSE,0);
	gtk_box_pack_start (GTK_BOX(hbox), gradient.button.getWidget(), FALSE, FALSE,0);
	tb.newLine ("color:", hbox /*gradient.button.getWidget()*/ );
	gradient.container = tb.getTable();
}

void IlluminationWindow::init_curve()
{
	// init color button
	curve.color.setName ("curve color");
	gtk_signal_connect (GTK_OBJECT(curve.color.getWidget()), "clicked", (GtkSignalFunc) button_clicked, this);


	curve.width = mygtk_new_spin (3.0, 0.1, 100.0, 0.1, 1.0, 0.1, 1);
	curve.gamma = mygtk_new_spin (3.0, 0.1, 100.0, 0.1, 1.0, 0.1, 1);
	
	GtkWidget *box = gtk_hbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX(box), curve.color.getWidget(), FALSE, FALSE, 0);

	TableBuilder tb;
	tb.setSize (3);
	tb.newLine ("color:", box);
	tb.newLine ("width:", curve.width);
	tb.newLine ("gamma:", curve.gamma);
	

	GtkWidget *frame = gtk_frame_new ("curve");
	gtk_container_border_width (GTK_CONTAINER(frame), 5);
	
	gtk_container_add (GTK_CONTAINER(frame), tb.getTable());

	curve.container = frame;

	controlWidget (curve.width, "curve_width");
	controlWidget (curve.gamma, "curve_gamma");
	controlWidget (curve.color.getWidget(), "curve");
}

IlluminationWindow::IlluminationWindow()
{
	illumination.ambient     = gtk_toggle_button_new_with_label ("ambient");
	illumination.diffuse     = gtk_toggle_button_new_with_label ("diffuse");
	illumination.reflected   = gtk_toggle_button_new_with_label ("reflected");
	illumination.transmitted = gtk_toggle_button_new_with_label ("transmitted");

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(illumination.ambient),   TRUE);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(illumination.diffuse),   TRUE);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(illumination.reflected), TRUE);


	GtkWidget *box = gtk_hbutton_box_new();
	gtk_button_box_set_layout (GTK_BUTTON_BOX(box), GTK_BUTTONBOX_START);
	gtk_button_box_set_spacing (GTK_BUTTON_BOX(box), 5);
	gtk_container_add (GTK_CONTAINER(box), illumination.ambient);
	gtk_container_add (GTK_CONTAINER(box), illumination.diffuse);
	gtk_container_add (GTK_CONTAINER(box), illumination.reflected);
	gtk_container_add (GTK_CONTAINER(box), illumination.transmitted);

	GtkWidget *ambient_box   = gtk_hbox_new( FALSE,5 );
        GtkWidget *ambient_label = gtk_label_new( "ambient:" );
	GtkObject *adj_ambient   = gtk_adjustment_new( 20.0, 0.0, 101.0, 1.0, 1.0, 1.0 );
	GtkWidget *ambient_value = gtk_hscale_new( GTK_ADJUSTMENT( adj_ambient ) );

        gtk_scale_set_digits   ( GTK_SCALE( ambient_value ),0 );
        gtk_scale_set_value_pos( GTK_SCALE( ambient_value ),GTK_POS_RIGHT );
	
	gtk_box_pack_start (GTK_BOX(ambient_box),ambient_label,FALSE,FALSE,0 );
	gtk_box_pack_start (GTK_BOX(ambient_box),ambient_value,TRUE,TRUE,0 );
	gtk_box_pack_start (GTK_BOX(ambient_box),gtk_label_new( "     " ),TRUE,TRUE,0 );

	//
	// --- build the colorselectiondialog
	// 
	colorseldialog = gtk_color_selection_dialog_new ("");
	VOIDCONNECT (GTK_COLOR_SELECTION_DIALOG(colorseldialog)->ok_button,
		     "clicked",
		     colorSelected);
	VOIDCONNECT (GTK_COLOR_SELECTION_DIALOG(colorseldialog)->cancel_button,
		     "clicked",
		     closeColorSelection);
	gtk_widget_hide (GTK_COLOR_SELECTION_DIALOG(colorseldialog)->help_button);

	gtk_signal_connect (GTK_OBJECT(colorseldialog), "delete_event", GTK_SIGNAL_FUNC(handle_delete), this); 

	// call the init_* methods
	// init_gradient();
	init_curve();

	// 
	// --- build the rest
	//

	container = gtk_vbox_new (FALSE, 5);

	backgroundColor.setName ("background color");
	gtk_signal_connect (GTK_OBJECT(backgroundColor.getWidget()), "clicked", (GtkSignalFunc) button_clicked, this);
	controlWidget( backgroundColor.getWidget(),"background" );
	GtkWidget *backgroundColorBox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start (GTK_BOX(backgroundColorBox), gtk_label_new("background color:"), FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(backgroundColorBox), backgroundColor.getWidget(), FALSE, FALSE, 5);
	gtk_box_pack_start (GTK_BOX(container), backgroundColorBox, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(container), box, FALSE, FALSE, 0);
        gtk_box_pack_start (GTK_BOX(container), ambient_box,FALSE,FALSE,0 );

        GtkWidget *surfaces_scroll = gtk_scrolled_window_new( NULL,NULL );
        GtkWidget *light_scroll    = gtk_scrolled_window_new( NULL,NULL );
        gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW(surfaces_scroll ),
                                        GTK_POLICY_AUTOMATIC, GTK_POLICY_NEVER );
        gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW(light_scroll ),
                                        GTK_POLICY_AUTOMATIC, GTK_POLICY_NEVER );
	GtkWidget *surfaces_box = gtk_hbox_new( FALSE,5 );
	GtkWidget *light_box    = gtk_hbox_new( FALSE,5 );
        gtk_scrolled_window_add_with_viewport( GTK_SCROLLED_WINDOW( surfaces_scroll ),surfaces_box );
        gtk_scrolled_window_add_with_viewport( GTK_SCROLLED_WINDOW( light_scroll ),   light_box    );
	gtk_box_pack_start (GTK_BOX(container), surfaces_scroll, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(container), light_scroll,    FALSE, FALSE, 0);

        for( int i=0; i<MAIN_SURFACE_AMOUNT_NUM; i++ ) {
	        char surfaces_string[15];
                char so_string[39],si_string[39];

                if( i>0 ) {
                        sprintf( surfaces_string,"surface %d",i+1 );
	        } else {
                        sprintf( surfaces_string,"surface" );
	        }
        
        	GtkWidget *surfaces_frame = gtk_frame_new ( surfaces_string );
                gtk_widget_set_usize( surfaces_frame,250,0 );
                TableBuilder tb;
                tb.setSize( 10 );
        
        	GtkWidget *outside_box = gtk_hbox_new( FALSE,5 );
        	GtkWidget *inside_box  = gtk_hbox_new( FALSE,5 );
                buttons[0][i] = new ColorSelectionButton( );
                buttons[1][i] = new ColorSelectionButton( );

                sprintf( so_string,"outside color of surface %d",i+1 );
                sprintf( si_string,"inside color of surface %d",i+1 );
                buttons[0][i]->setName( so_string );
                buttons[1][i]->setName( si_string );
        	gtk_signal_connect( GTK_OBJECT( buttons[0][i]->getWidget( ) ),"clicked",(GtkSignalFunc)button_clicked,this );
        	gtk_signal_connect( GTK_OBJECT( buttons[1][i]->getWidget ( ) ),"clicked",(GtkSignalFunc)button_clicked,this );
        	gtk_box_pack_start( GTK_BOX(outside_box),buttons[0][i]->getWidget( ),FALSE,FALSE,0 );
        	gtk_box_pack_start( GTK_BOX(inside_box), buttons[1][i]->getWidget ( ),FALSE,FALSE,0 );
                tb.newLine( "outside color:",outside_box );
                tb.newLine( "inside color:", inside_box  );
        
                tb.separator( );
        
        	GtkObject *adj_diffuse     = gtk_adjustment_new( 60.0, 0.0, 101.0, 1.0, 1.0, 1.0 );
        	GtkObject *adj_reflected   = gtk_adjustment_new( 70.0, 0.0, 101.0, 1.0, 1.0, 1.0 );
        	GtkObject *adj_transmitted = gtk_adjustment_new( 70.0, 0.0, 101.0, 1.0, 1.0, 1.0 );
        	diffuse[i]     = gtk_hscale_new( GTK_ADJUSTMENT( adj_diffuse     ) );
        	reflected[i]   = gtk_hscale_new( GTK_ADJUSTMENT( adj_reflected   ) );
        	transmitted[i] = gtk_hscale_new( GTK_ADJUSTMENT( adj_transmitted ) );
                gtk_scale_set_digits   ( GTK_SCALE( diffuse[i] )    ,0 );
                gtk_scale_set_digits   ( GTK_SCALE( reflected[i] )  ,0 );
                gtk_scale_set_digits   ( GTK_SCALE( transmitted[i] ),0 );
                gtk_scale_set_value_pos( GTK_SCALE( diffuse[i] )    ,GTK_POS_RIGHT );
                gtk_scale_set_value_pos( GTK_SCALE( reflected[i] )  ,GTK_POS_RIGHT );
                gtk_scale_set_value_pos( GTK_SCALE( transmitted[i] ),GTK_POS_RIGHT );        
                tb.newLine( "diffuse:",    diffuse[i]     );
                tb.newLine( "reflected:",  reflected[i]   );
                tb.newLine( "transmitted:",transmitted[i] );
                controlWidget( diffuse[i],    LIGHT_SETTINGS_FIRST_PIC_STRING[i][1] );        
                controlWidget( reflected[i],  LIGHT_SETTINGS_FIRST_PIC_STRING[i][2] );        
                controlWidget( transmitted[i],LIGHT_SETTINGS_FIRST_PIC_STRING[i][3] );

                tb.separator( );
        
        	GtkObject *adj_smoothness   = gtk_adjustment_new( 30.0, 1.0, 101.0, 1.0, 1.0, 1.0 );
        	GtkObject *adj_transparence = gtk_adjustment_new( 70.0, 0.0, 101.0, 1.0, 1.0, 1.0 );
        	GtkObject *adj_thickness    = gtk_adjustment_new( 10.0, 0.0, 101.0, 1.0, 1.0, 1.0 );
        	smoothness[i]   = gtk_hscale_new( GTK_ADJUSTMENT( adj_smoothness   ) );
        	transparence[i] = gtk_hscale_new( GTK_ADJUSTMENT( adj_transparence ) );
        	thickness[i]    = gtk_hscale_new( GTK_ADJUSTMENT( adj_thickness    ) );
                gtk_scale_set_digits   ( GTK_SCALE( smoothness[i] ),  0 );
                gtk_scale_set_digits   ( GTK_SCALE( transparence[i] ),0 );
                gtk_scale_set_digits   ( GTK_SCALE( thickness[i] ),   0 );
                gtk_scale_set_value_pos( GTK_SCALE( smoothness[i] ),  GTK_POS_RIGHT );
                gtk_scale_set_value_pos( GTK_SCALE( transparence[i] ),GTK_POS_RIGHT );
                gtk_scale_set_value_pos( GTK_SCALE( thickness[i] ),   GTK_POS_RIGHT );
                tb.newLine( "smoothness:",  smoothness[i]   );
                tb.newLine( "transparence:",transparence[i] );
                tb.newLine( "thickness:",   thickness[i]    );
                controlWidget( smoothness[i],  LIGHT_SETTINGS_SECOND_PIC_STRING[i][0] );        
                controlWidget( transparence[i],LIGHT_SETTINGS_SECOND_PIC_STRING[i][1] );        
                controlWidget( thickness[i],   LIGHT_SETTINGS_SECOND_PIC_STRING[i][2] );
        
	        gtk_container_add( GTK_CONTAINER( surfaces_frame ),tb.getTable( ) );
 	        gtk_box_pack_start( GTK_BOX( surfaces_box ),surfaces_frame,FALSE,FALSE,5 );
	}

        for( int i=0; i<LIGHT_SOURCE_MAX_VALUE; i++ ) {
	        char light_string[15];
                char color_string[39];

                sprintf( light_string,"light %d",i+1 );
        
        	GtkWidget *light_frame = gtk_frame_new ( light_string );
                gtk_widget_set_usize( light_frame,250,0 );
                TableBuilder tb;
                tb.setSize( 5 );
        
        	GtkWidget *color_box = gtk_hbox_new( FALSE,5 );
                color_buttons[i] = new ColorSelectionButton( );

                sprintf( color_string,"color of light %d",i+1 );
                color_buttons[i]->setName( color_string );
        	gtk_signal_connect( GTK_OBJECT( color_buttons[i]->getWidget( ) ),"clicked",(GtkSignalFunc)button_clicked,this );
        	gtk_box_pack_start( GTK_BOX(color_box),color_buttons[i]->getWidget( ),FALSE,FALSE,0 );
                tb.newLine( "color:",color_box );
        
		light_x[i] = mygtk_new_spin (100.0, -100000.0, 100000.0, 0.1, 1.0, 0.1, 1);
		light_y[i] = mygtk_new_spin (100.0, -100000.0, 100000.0, 0.1, 1.0, 0.1, 1);
		light_z[i] = mygtk_new_spin (100.0, -100000.0, 100000.0, 0.1, 1.0, 0.1, 1);
                tb.newLine( "x:",light_x[i] );
                tb.newLine( "y:",light_y[i] );
                tb.newLine( "z:",light_z[i] );
                controlWidget( light_x[i],LIGHT_POSITION_PIC_STRING[i][0] );
                controlWidget( light_y[i],LIGHT_POSITION_PIC_STRING[i][1] );
                controlWidget( light_z[i],LIGHT_POSITION_PIC_STRING[i][2] );

        	GtkObject *adj_volume = gtk_adjustment_new( 30.0, 0.0, 101.0, 1.0, 1.0, 1.0 );
        	volume[i] = gtk_hscale_new( GTK_ADJUSTMENT( adj_volume ) );
                gtk_scale_set_digits( GTK_SCALE( volume[i] ),0 );
                gtk_scale_set_value_pos( GTK_SCALE( volume[i] ),GTK_POS_RIGHT );
                tb.newLine( "volume:",volume[i]   );
                controlWidget( volume[i],LIGHT_VOLUME_PIC_STRING[i] );        
        
	        gtk_container_add( GTK_CONTAINER( light_frame ),tb.getTable( ) );
 	        gtk_box_pack_start( GTK_BOX( light_box ),light_frame,FALSE,FALSE,5 );
	}

 	gtk_box_pack_start (GTK_BOX(container), curve.container,	FALSE, FALSE, 0);
}

IlluminationWindow::~IlluminationWindow()
{

	for (int j=0; j<2; j++) {
		for (int i=0; i<MAIN_SURFACE_AMOUNT_NUM; i++) {
			delete buttons [j][i];
		}
	}
}


//
// --- GTK CALLBACKS
//

gint IlluminationWindow::handle_delete (GtkWidget *widget, GdkEvent *event, gpointer data)
{
	gtk_widget_hide (widget);
	return TRUE;
}

void IlluminationWindow::button_clicked (GtkWidget *widget, gpointer data)
{
	IlluminationWindow *iw = (IlluminationWindow *) data;

	ColorSelectionButton *cw = (ColorSelectionButton *)gtk_object_get_data (GTK_OBJECT(widget), "button");
	assert(cw);
	cw->selectOn (GTK_COLOR_SELECTION_DIALOG(iw->colorseldialog));
}

void IlluminationWindow::colorSelected ()
{
	gtk_widget_hide (colorseldialog);
}

void IlluminationWindow::closeColorSelection()
{
	gtk_widget_hide (colorseldialog);
}
