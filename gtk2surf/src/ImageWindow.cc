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

#include "ScriptWindow.h"
#include "ImageWindow.h"
#include "Kernel.h"

#include<fstream>

#ifdef HAVE_SSTREAM
#  include<sstream>
#else
#  include<strstream>
#endif

ImageWindow::ImageWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade)
  : Gtk::Window(cobject),
    myGlade(refGlade)
{
  // Parent widget
  myGlade->get_widget_derived("window_script", script_win);
  // Child widgets
  myGlade->get_widget("menu_image", popup_menu);
  popup_menu->show_all();
  myGlade->get_widget("drawingarea", drawingarea);
  // Self
  drawingarea->signal_expose_event().connect( sigc::mem_fun(*this, &ImageWindow::_on_expose_event) );
  signal_key_press_event().connect( sigc::mem_fun(*this, &ImageWindow::_on_key_press_event) );
  signal_button_press_event().connect( sigc::mem_fun(*this, &ImageWindow::_on_button_press_event) );
  add_events(Gdk::BUTTON_PRESS_MASK);

  // Menu item callbacks
  Gtk::MenuItem* mi_save = 0;
  Gtk::MenuItem* mi_save_as = 0;
  Gtk::MenuItem* mi_dither = 0;
  Gtk::MenuItem* mi_close = 0;

  myGlade->get_widget("save_image", mi_save);
  myGlade->get_widget("save_image_as", mi_save_as);
  myGlade->get_widget("dither_image", mi_dither);
  myGlade->get_widget("close_image", mi_close);
  
  mi_save->signal_activate().connect( sigc::mem_fun(*this, &ImageWindow::_on_save_activate) );
  mi_save_as->signal_activate().connect( sigc::mem_fun(*this, &ImageWindow::_on_save_as_activate) );
  mi_dither->signal_activate().connect( sigc::mem_fun(*this, &ImageWindow::_on_dither_activate) );
  mi_close->signal_activate().connect( sigc::mem_fun(*this, &ImageWindow::_on_close_activate) );

  set_my_title();
}

ImageWindow::~ImageWindow()
{
}

void ImageWindow::set_my_title()
{
  std::string title = PACKAGE;
  title += " - Color Image: ";

  if(filename.length() == 0) {
    title += "Untitled";
  } else {
    title += filename;
  }

  set_title(Glib::ustring(title));
}

void ImageWindow::clear()
{
  if (!pixbuf) {
    return;
  }
  pixbuf.reset();
  pixbuf = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, false,
			       8, width, height);
  show();
}

void ImageWindow::read_data()
{
  if (pixbuf) pixbuf.reset();

  std::string whstring = Kernel::receive_line();
#ifdef HAVE_SSTREAM
  std::istringstream is(whstring);
#else
  std::istrstream is(whstring.c_str());
#endif
#ifdef DEBUG
  std::cout << whstring + "\n";
#endif
  is >> width >> height;

  Kernel::receive_line(); // eat up "255\n" line

  int rowstride = 3*width;
  int length = rowstride*height;
  char* pixdata = new char[length];
  for(int y = 0; y<height; y++) {
    Kernel::receive_bytes(pixdata + y*rowstride, rowstride);
    Gtk::Main::iteration(false);
  }

  pixbuf = Gdk::Pixbuf::create_from_data(reinterpret_cast<guint8*>(pixdata),
					 Gdk::COLORSPACE_RGB, false, 8,
					 width, height, rowstride);

  drawingarea->set_size_request(width, height);
  set_default_size(width, height);
  show_all(); raise();
}


// GTK callbacks
// =============

bool ImageWindow::_on_expose_event(GdkEventExpose *e) 
{
  if (!pixbuf) return false;
#if GTK_VERSION_GE(2,14)
  drawingarea->get_window()->draw_pixbuf(pixbuf, e->area.x, e->area.y,
					 e->area.x, e->area.y,
					 -1, -1,
					 Gdk::RGB_DITHER_NONE, 0, 0);
#else
  drawingarea->get_window()->draw_pixbuf(drawingarea->get_style()->get_fg_gc(Gtk::STATE_NORMAL),
					 pixbuf, e->area.x, e->area.y,
					 e->area.x, e->area.y,
					 -1, -1,
					 Gdk::RGB_DITHER_NONE, 0, 0);
#endif
  return true;
}

bool ImageWindow::_on_key_press_event(GdkEventKey* e)
{
  if (e->state == GDK_CONTROL_MASK) {
    switch(e->keyval) {
    case GDK_s:
      _on_save_activate();
      break;
    case GDK_d:
      _on_dither_activate();
      break;
    case GDK_w:
      _on_close_activate();
      break;
    }
  }
}

bool ImageWindow::_on_button_press_event(GdkEventButton* e)
{
  if ((e->type == GDK_BUTTON_PRESS) 
      && (e->button == 3))
    {
      popup_menu->popup(e->button, e->time);
      return true;
    }
  else 
    { 
      return false;
    }
}

void ImageWindow::_on_save_activate(void)
{
  if(filename.length() == 0) {
    _on_save_as_activate();
    return;
  }

  std::string s = "filename = \"";
  s += filename + "\";\n";
  if(filetype.length() > 0) {
    s += "color_file_format = ";
    s += filetype + ";\n";
  }
  s += "save_color_image;\n";
  Kernel::send(s);
}

void ImageWindow::_on_save_as_activate(void)
{
  // TODO: implement filetype choice
  script_win->set_status("Save color image as...");
  Gtk::FileChooserDialog dialog("Save color image as...",
				Gtk::FILE_CHOOSER_ACTION_SAVE);
  dialog.set_transient_for(*this);

  //Add response buttons the the dialog:
  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);

  int result = dialog.run();

  if (result == Gtk::RESPONSE_OK) {
    filetype = "png";
    filename = dialog.get_filename();
    set_my_title();
    _on_save_activate();
  }
}

void ImageWindow::_on_dither_activate(void)
{
  std::string script = "surface_run_commands = 1;\n";
  if(mode == SURFACE) {
    script += "dither_surface;\n";
  } else {
    script += "dither_curve;\n";
  }
  script += "filename = \"-\";\n"
    "dither_file_format = pbm;\n"
    "save_dithered_image;\n";
  Kernel::send(script);
}

void ImageWindow::_on_close_activate(void)
{ 
  hide();
  pixbuf.reset();
}
