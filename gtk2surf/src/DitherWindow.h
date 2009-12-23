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

#ifndef DITHER_WINDOW_H
#define DITHER_WINDOW_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtkmm.h>
#include <string>

class DitherWindow : public Gtk::Window 
{
 public: 
  DitherWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade);
  virtual ~DitherWindow();
  void clear();
  void read_data();
  
 protected:
  // widgets
  Glib::RefPtr<Gtk::Builder> myGlade;
  ScriptWindow* script_win;
  Gtk::Menu* popup_menu;
  Gtk::DrawingArea* drawingarea;

  // Data
  int width;
  int height;
  Glib::RefPtr<Gdk::Pixbuf> pixbuf;
  std::string filename;
  std::string filetype;
  void set_my_title();

  // Callbacks
  bool _on_expose_event(GdkEventExpose *e);
  bool _on_button_press_event(GdkEventButton* e);
  void _on_save_activate(void);
  void _on_save_as_activate(void);
  void _on_close_activate(void);
};

#endif //!DITHER_WINDOW_H       

