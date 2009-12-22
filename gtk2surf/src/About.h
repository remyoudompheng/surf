/* 
 *  gtk2surf
 *      GTK2 Translation of gtksurf
 * 
 *  Author: Rémy Oudompheng <oudomphe@math.unice.fr>
 *  Copyright (C) 2009 Rémy Oudompheng
 * 
 *  License: GPL version >= 2
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

