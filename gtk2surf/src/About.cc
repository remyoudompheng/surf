/* 
 *  gtk2surf
 *      GTK2 Translation of gtksurf
 * 
 *  Author: Rémy Oudompheng <oudomphe@math.unice.fr>
 *  Copyright (C) 2009 Rémy Oudompheng
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

#include "About.h"

About::About(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade)
  : Gtk::Window(cobject),
    myGlade(refGlade),
    label(0)
{ 
  signal_delete_event().connect( sigc::mem_fun(*this, &About::_on_delete_event) );
  signal_button_press_event().connect( sigc::mem_fun(*this, &About::_on_button_press_event) );

  myGlade->get_widget("label_about", label);
  label->set_text(PACKAGE " " VERSION "\n"
		  "a GTK2 based graphical frontend to surf\n"
		  "\n"
		  "Author: Rémy Oudompheng <oudomphe@math.unice.fr>\n"
		  "Copyright (C) 2009 Rémy Oudompheng\n"
		  "License: GPL v2 or later\n"
		  "\n"
		  "http://surf.sourceforge.net");
}

About::~About()
{
}

bool About::_on_delete_event(GdkEventAny* event)
{
  hide_all();
  return true;
}

bool About::_on_button_press_event(GdkEventButton* event)
{
  hide_all();
  return true;
}
