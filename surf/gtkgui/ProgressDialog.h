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



#ifndef PROGRESS_DIALOG_H
#define PROGRESS_DIALOG_H

#include <gtk/gtk.h>
#define MAX_LEN 256

class Thread;

class ProgressDialog
{
public:
	enum ProgressType {
		Dialog,
		StatusBar
	};
	
	ProgressDialog();
 	~ProgressDialog();

	GtkWidget* getProgressBox() const { return hbox; }
	
	void setDone(double _done) { done = _done; }
	void setDoing(const char* _doing) { doing = _doing; }

	void setThread(Thread* aThread) { thread = aThread; show(); }
	void updateDisplay();	

private:
	static void tryToStop (GtkWidget *, gpointer);
	void show();
	void hide();

	ProgressType type;

	GtkWidget* dialog;
	GtkWidget* hbox;
	GtkWidget* label;
	GtkWidget* statusbar;
	GtkWidget* progress;
	GtkWidget* button;

	Thread *thread;
	const char *doing;
	double done;
	gint timeouttag;
	bool shown;

	// 
	// --- Callbacks
	//
	static gint timeout(gpointer data);
	static gint handle_delete(GtkWidget* widget, GdkEvent* event, gpointer data);
};

#endif
