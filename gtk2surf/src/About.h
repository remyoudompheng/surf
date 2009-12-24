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

#ifndef ABOUT_H
#define ABOUT_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtkmm.h>

using namespace std;

class About : public Gtk::Window 
{
 public: 
  About(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade);
  virtual ~About();
  
 protected:
  // Signal Handlers
  bool _on_button_press_event(GdkEventButton* event);
  bool _on_delete_event(GdkEventAny* event); 

  Glib::RefPtr<Gtk::Builder> myGlade;
  Gtk::Label* label;
};

#endif //!ABOUT_H                                                                                             

