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

#include <iostream.h>

#include "ProgressDialog.h"
#include "Thread.h"

void ProgressDialog::tryToStop (GtkWidget *widget, gpointer data)
{
	ProgressDialog *pd = (ProgressDialog *)data;
	if (pd->thread) {
		pd->thread->stop();
	}
	
}


ProgressDialog::ProgressDialog()
{
	thread=0;
	doing=0;
	done=-1.0;
	shown = false;
	dialog = gtk_dialog_new ();
	gtk_container_border_width(GTK_CONTAINER(GTK_DIALOG (dialog)->vbox), 10);

	label = gtk_label_new("");

	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox),
			    label, TRUE, TRUE, 0);
	
	progress = gtk_progress_bar_new();
	gtk_progress_set_show_text (GTK_PROGRESS(progress), TRUE);
	gtk_progress_set_activity_mode (GTK_PROGRESS(progress), TRUE);
	
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox),
			    progress, FALSE, FALSE, 5);
	

	GtkWidget *button = gtk_button_new_with_label ("Stop");
	gtk_signal_connect (GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(tryToStop), this);

	gtk_box_pack_start (GTK_BOX(GTK_DIALOG(dialog)->action_area), button, TRUE, TRUE, 0);

	gtk_signal_connect (GTK_OBJECT(dialog), "delete_event", GTK_SIGNAL_FUNC(handle_delete), this);
	timeouttag = gtk_timeout_add (100, timeout, this);
}

ProgressDialog::~ProgressDialog()
{
	gtk_widget_hide(dialog);
	gtk_widget_destroy(dialog);
}

void ProgressDialog::updateDisplay()
{
	const char *newDoing;
	double newDone;
	if (thread) {
		if (thread->isReady()) {
			gtk_timeout_remove (timeouttag);
			delete this;
			return;
		}
		thread->getProgress (newDoing, newDone);
		if (newDoing != doing) {
			doing = newDoing;
			gtk_label_set_text(GTK_LABEL(label), doing);
			show();
		}
		if (newDone != done) {
			if (done < 0 && newDone >= 0) {
				gtk_progress_set_activity_mode (GTK_PROGRESS(progress), FALSE);
			} else if (done >= 0.0 && newDone < 0) {
				gtk_progress_set_activity_mode (GTK_PROGRESS(progress), TRUE);
			}
			done = newDone;
			if (done >= 0) {
				gtk_progress_set_percentage (GTK_PROGRESS(progress), done);
			}
		} else if (done < 0) {
			gfloat newval;
			GtkAdjustment *adj;
			adj = GTK_PROGRESS(progress)->adjustment;
			newval = adj->value+1;
			if (newval > adj->upper)
				newval=adj->lower;
			gtk_progress_set_value(GTK_PROGRESS(progress),newval);
			
		}
	}
}

void ProgressDialog::show()
{
	if (!shown) {
		gtk_widget_show_all (dialog);
		shown = true;
	}
	
}


void ProgressDialog::setDone (double _done)
{
	done = _done;
}

void ProgressDialog::setDoing (const char *_doing)
{
	doing = _doing;
}




//
// --- Callbacks
//

gint ProgressDialog::timeout (gpointer data)
{
	((ProgressDialog *)data)->updateDisplay();
	return TRUE;
}

gint ProgressDialog::handle_delete (GtkWidget *widget, GdkEvent *event, gpointer data)
{
	return TRUE;
}
