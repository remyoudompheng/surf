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

#ifndef SCRIPT_WINDOW_H
#define SCRIPT_WINDOW_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtkmm.h>
#include <string>

using namespace std;

class ImageWindow;

class ScriptWindow : public Gtk::Window 
{
 public: 
  ScriptWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade);
  virtual ~ScriptWindow();

  void set_my_title(void);
  void set_status(const std::string& txt);
  void set_progress(double percentage);
  void progress_mode(bool mode) const;
  void select_region(int from, int to);
  void load_file(const std::string& fname);
  
 protected:
  bool dirty; // File has changed ?
  string filename;
  size_t prelude_length;

  Glib::RefPtr<Gtk::Builder> myGlade;
  Gtk::TextView* text_script;
  Glib::RefPtr<Gtk::TextBuffer> text_buffer;
  Glib::RefPtr<Gtk::Clipboard> refClipboard;
  Gtk::Statusbar* sbar;
  Gtk::ProgressBar* pbar;

  ImageWindow* image_win;

  void _on_new_activate();
  void _on_open_activate();
  void _on_save_activate();
  void _on_save_as_activate();
  void _on_prefs_activate();
  void _on_quit_activate();

  void _on_cut_activate();
  void _on_copy_activate();
  void _on_paste_activate();
  void _on_clear_activate();
  void _on_select_all_activate();

  void _on_render_curve_activate();
  void _on_render_surface_activate();
  void _on_execute_activate();

  void _on_about_activate();

  void _on_script_changed();
};

#endif //!SCRIPT_WINDOW_H       

