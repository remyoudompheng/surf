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

#include "ScriptWindow.h"

ScriptWindow::ScriptWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade)
  : Gtk::Window(cobject),
    myGlade(refGlade)
{
  Gtk::MenuItem* mi_new = 0;
  Gtk::MenuItem* mi_open = 0;
  Gtk::MenuItem* mi_save = 0;
  Gtk::MenuItem* mi_save_as = 0;

  myGlade->get_widget("new", mi_new);
  myGlade->get_widget("new", mi_open);
  myGlade->get_widget("new", mi_save);
  myGlade->get_widget("new", mi_save_as);
  
  mi_new->signal_activate().connect( sigc::mem_fun(*this, &ScriptWindow::_on_new_activate) );
  mi_open->signal_activate().connect( sigc::mem_fun(*this, &ScriptWindow::_on_open_activate) );
  mi_save->signal_activate().connect( sigc::mem_fun(*this, &ScriptWindow::_on_save_activate) );
  mi_save_as->signal_activate().connect( sigc::mem_fun(*this, &ScriptWindow::_on_save_as_activate) );
}

ScriptWindow::~ScriptWindow()
{
}

void ScriptWindow::_on_new_activate() {}
void ScriptWindow::_on_open_activate() {}
void ScriptWindow::_on_save_activate() {}
void ScriptWindow::_on_save_as_activate() {}
void ScriptWindow::_on_prefs_activate() {}
void ScriptWindow::_on_quit_activate() {}
void ScriptWindow::_on_cut_activate() {}
void ScriptWindow::_on_copy_activate() {}
void ScriptWindow::_on_paste_activate() {}
void ScriptWindow::_on_about_activate() {}
