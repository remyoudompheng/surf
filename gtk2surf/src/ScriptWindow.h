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

#ifndef SCRIPT_WINDOW_H
#define SCRIPT_WINDOW_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtkmm.h>

using namespace std;

class ScriptWindow : public Gtk::Window 
{
 public: 
  ScriptWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade);
  virtual ~ScriptWindow();
  
 protected:
  Glib::RefPtr<Gtk::Builder> myGlade;
  Gtk::TextView* text_script;
  Glib::RefPtr<Gtk::TextBuffer> text_buffer;
  Glib::RefPtr<Gtk::Clipboard> refClipboard;

  void _on_new_activate();
  void _on_open_activate();
  void _on_save_activate();
  void _on_save_as_activate();
  void _on_prefs_activate();
  void _on_quit_activate();
  void _on_cut_activate();
  void _on_copy_activate();
  void _on_paste_activate();
  void _on_about_activate();
};

#endif //!SCRIPT_WINDOW_H       

