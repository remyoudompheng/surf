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
