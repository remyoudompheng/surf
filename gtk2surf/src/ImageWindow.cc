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
  signal_button_press_event().connect( sigc::mem_fun(*this, &ImageWindow::_on_button_press_event) );

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
  is >> width >> height;

  Kernel::receive_line(); // eat up "255\n" line

  int rowstride = 3*width;
  int length = rowstride*height;
  char* pixdata = new char[length];
  for(int y = 0; y != height; y++) {
    Kernel::receive_bytes(pixdata + y*rowstride, rowstride);
    Gtk::Main::iteration(false);
  }
  
  pixbuf = Gdk::Pixbuf::create_from_data(reinterpret_cast<guint8*>(pixdata),
					 Gdk::COLORSPACE_RGB, false, 8,
					 width, height, rowstride);
  delete pixdata;
  
  drawingarea->set_size_request(width, height);
  set_size_request(width, height);

  show();
}


// GTK callbacks
// =============

bool ImageWindow::_on_expose_event(GdkEventExpose *e) 
{
  if (!pixbuf) return true;
  drawingarea->get_window()->draw_pixbuf(drawingarea->get_style()->get_fg_gc(Gtk::STATE_NORMAL),
					 pixbuf, e->area.x, e->area.y,
					 e->area.x, e->area.y,
					 width, height, Gdk::RGB_DITHER_NONE,
					 0, 0);
  return true;
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
}

void ImageWindow::_on_save_as_activate(void)
{
}

void ImageWindow::_on_dither_activate(void)
{
}

void ImageWindow::_on_close_activate(void)
{ 
  hide();
  pixbuf.reset();
}
