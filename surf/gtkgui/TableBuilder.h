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
 * Johannes Gutenberg-Universitaet Mainz, Fachbereich 17 Mathematik,
 * hereby disclaims all copyright interest in the program `surf'
 * (which makes passes at compilers) written by Stephan Endrass,
 * Hans Huelf, Ruediger Oertel, Kai Schneider, Ralf Schmitt and
 * Johannes Beigel
 *
 */

#ifndef TABLEBUILDER_H
#define TABLEBUILDER_H

#include <assert.h>

#include "mygtk.h"

class TableBuilder
{
public:
	TableBuilder ()
		{
			table=0;
		}

	void setSize (int _height)
		{
			assert(table==0);
			height = _height;
			table = gtk_table_new (height, 2, FALSE);
			currentHeight=0;
		}
	
	void newLine (GtkWidget *left, GtkWidget *right, double alignment=0.5)
		{
			assert(currentHeight < height);
			gtk_table_attach (GTK_TABLE(table), left, 0, 1, currentHeight, currentHeight+1,
					  (GtkAttachOptions)(GTK_FILL | GTK_SHRINK), 
					  (GtkAttachOptions)(GTK_FILL | GTK_SHRINK), 
					  TRUE, TRUE);
			if (GTK_IS_MISC(left))
				gtk_misc_set_alignment (GTK_MISC(left),1,alignment);
			gtk_table_attach (GTK_TABLE(table), right, 1, 2, currentHeight, currentHeight+1,
					  (GtkAttachOptions)(GTK_FILL | GTK_SHRINK | GTK_EXPAND), (GtkAttachOptions)0,
					  TRUE, TRUE);


			currentHeight++;
		
		}

	void newLine (const char *label, GtkWidget *w, double alignment=0.5)
		{
			assert(currentHeight < height);
			GtkWidget * l = gtk_label_new (label);
			newLine (l,w,alignment);
		}

	void separator()
		{
			assert(currentHeight < height);
			gtk_table_attach (GTK_TABLE(table), gtk_hseparator_new(), 0,2,currentHeight, currentHeight+1,
					  (GtkAttachOptions) (GTK_FILL | GTK_SHRINK | GTK_EXPAND),(GtkAttachOptions)0,
					  TRUE, TRUE);
			currentHeight++;
		}
			

	GtkWidget *getTable()
		{
			if (currentHeight != height) {
				cerr << "WARNING: currentHeight=" << currentHeight << " != " << height << endl;
			}
			assert(currentHeight==height);
			return table;
		}

protected:
	GtkWidget  *table;
	int height;
	int currentHeight;

};
#endif
