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
#include "About.h"

ScriptWindow::ScriptWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade)
  : Gtk::Window(cobject),
    myGlade(refGlade),
    text_script(0)
{
  // File Menu item callbacks
  Gtk::MenuItem* mi_new = 0;
  Gtk::MenuItem* mi_open = 0;
  Gtk::MenuItem* mi_save = 0;
  Gtk::MenuItem* mi_save_as = 0;
  Gtk::MenuItem* mi_prefs = 0;
  Gtk::MenuItem* mi_quit = 0;

  myGlade->get_widget("new", mi_new);
  myGlade->get_widget("open", mi_open);
  myGlade->get_widget("save", mi_save);
  myGlade->get_widget("save_as", mi_save_as);
  myGlade->get_widget("preferences", mi_prefs);
  myGlade->get_widget("quit", mi_quit);
  
  mi_new->signal_activate().connect( sigc::mem_fun(*this, &ScriptWindow::_on_new_activate) );
  mi_open->signal_activate().connect( sigc::mem_fun(*this, &ScriptWindow::_on_open_activate) );
  mi_save->signal_activate().connect( sigc::mem_fun(*this, &ScriptWindow::_on_save_activate) );
  mi_save_as->signal_activate().connect( sigc::mem_fun(*this, &ScriptWindow::_on_save_as_activate) );
  mi_prefs->signal_activate().connect( sigc::mem_fun(*this, &ScriptWindow::_on_prefs_activate) );
  mi_quit->signal_activate().connect( sigc::mem_fun(*this, &ScriptWindow::_on_quit_activate) );

  // Edit Menu item callbacks
  Gtk::MenuItem* mi_cut = 0;
  Gtk::MenuItem* mi_copy = 0;
  Gtk::MenuItem* mi_paste = 0;

  myGlade->get_widget("cut", mi_cut);
  myGlade->get_widget("copy", mi_copy);
  myGlade->get_widget("paste", mi_paste);
  
  mi_cut->signal_activate().connect( sigc::mem_fun(*this, &ScriptWindow::_on_cut_activate) );
  mi_copy->signal_activate().connect( sigc::mem_fun(*this, &ScriptWindow::_on_copy_activate) );
  mi_paste->signal_activate().connect( sigc::mem_fun(*this, &ScriptWindow::_on_paste_activate) );

  // 
  myGlade->get_widget("text_script", text_script);
  text_buffer = text_script->get_buffer();
  refClipboard = Gtk::Clipboard::get();

  // Help Menu callbacks
  Gtk::MenuItem* mi_about = 0;
  myGlade->get_widget("about", mi_about);
  mi_about->signal_activate().connect( sigc::mem_fun(*this, &ScriptWindow::_on_about_activate) );

  // Toolbar callbacks
  Gtk::ToolButton* but_new = 0;
  Gtk::ToolButton* but_open = 0;
  Gtk::ToolButton* but_save = 0;

  myGlade->get_widget("button_new", but_new);
  myGlade->get_widget("button_open", but_open);
  myGlade->get_widget("button_save", but_save);

  but_new->signal_clicked().connect( sigc::mem_fun(*this, &ScriptWindow::_on_new_activate) );
  but_open->signal_clicked().connect( sigc::mem_fun(*this, &ScriptWindow::_on_open_activate) );
  but_save->signal_clicked().connect( sigc::mem_fun(*this, &ScriptWindow::_on_save_activate) );
}

ScriptWindow::~ScriptWindow()
{
}

void ScriptWindow::_on_new_activate() {}
void ScriptWindow::_on_open_activate() {}
void ScriptWindow::_on_save_activate() {}
void ScriptWindow::_on_save_as_activate() {}
void ScriptWindow::_on_prefs_activate() {}

void ScriptWindow::_on_quit_activate() 
{
  Gtk::Main::quit();
}

void ScriptWindow::_on_cut_activate() {
  text_buffer->cut_clipboard(refClipboard);
}

void ScriptWindow::_on_copy_activate() {
  text_buffer->copy_clipboard(refClipboard);
}

void ScriptWindow::_on_paste_activate() {
  text_buffer->paste_clipboard(refClipboard);
}

void ScriptWindow::_on_about_activate() {
  About* about_win = 0;
  myGlade->get_widget_derived("window_about", about_win);
  about_win->show_all();
}
