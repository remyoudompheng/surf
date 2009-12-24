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

#ifndef IMAGE_WINDOW_H
#define IMAGE_WINDOW_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtkmm.h>
#include "ScriptWindow.h"
#include <string>

class ImageWindow : public Gtk::Window 
{
 public: 
  ImageWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade);
  virtual ~ImageWindow();

  enum ImageMode {
    SURFACE,
    CURVE
  };

  void set_mode(ImageMode _mode) {
    mode = _mode;
  }
  
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
  ImageMode mode;
  std::string filename;
  std::string filetype;
  void set_my_title();

  // Callbacks
  bool _on_expose_event(GdkEventExpose *e);
  bool _on_key_press_event(GdkEventKey* e);
  bool _on_button_press_event(GdkEventButton* e);
  void _on_save_activate(void);
  void _on_save_as_activate(void);
  void _on_dither_activate(void);
  void _on_close_activate(void);
};

#endif //!IMAGE_WINDOW_H       

