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
#include "DitherWindow.h"
#include "Kernel.h"

#include<fstream>
#include<cstring>

DitherWindow::DitherWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade)
  : Gtk::Window(cobject),
    myGlade(refGlade)
{
  // Parent widget
  myGlade->get_widget_derived("window_script", script_win);
  // Child widgets
  myGlade->get_widget("menu_dither", popup_menu);
  popup_menu->show_all();
  myGlade->get_widget("drawingarea_dither", drawingarea);
  // Self
  drawingarea->signal_expose_event().connect( sigc::mem_fun(*this, &DitherWindow::_on_expose_event) );
  signal_button_press_event().connect( sigc::mem_fun(*this, &DitherWindow::_on_button_press_event) );

  // Menu item callbacks
  Gtk::MenuItem* mi_save = 0;
  Gtk::MenuItem* mi_save_as = 0;
  Gtk::MenuItem* mi_close = 0;

  myGlade->get_widget("save_dither", mi_save);
  myGlade->get_widget("save_dither_as", mi_save_as);
  myGlade->get_widget("close_dither", mi_close);
  
  mi_save->signal_activate().connect( sigc::mem_fun(*this, &DitherWindow::_on_save_activate) );
  mi_save_as->signal_activate().connect( sigc::mem_fun(*this, &DitherWindow::_on_save_as_activate) );
  mi_close->signal_activate().connect( sigc::mem_fun(*this, &DitherWindow::_on_close_activate) );

  set_my_title();
}

DitherWindow::~DitherWindow()
{
}

void DitherWindow::set_my_title()
{
  std::string title = PACKAGE;
  title += " - Dither Image: ";

  if(filename.length() == 0) {
    title += "Untitled";
  } else {
    title += filename;
  }

  set_title(Glib::ustring(title));
}

void DitherWindow::clear()
{
  if (!pixbuf) {
    return;
  }
  pixbuf.reset();
  pixbuf = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, false,
			       8, width, height);
  show();
}

void DitherWindow::read_data()
{
  if (pixbuf) pixbuf.reset();

  std::string whstring = Kernel::receive_line();
#ifdef HAVE_SSTREAM
  std::istringstream is(whstring);
#else
  std::istrstream is(whstring.c_str());
#endif
  is >> width >> height;

  int rowstride = 3*width;
  char* pixdata = new char[rowstride*height];
  size_t bwidth = width/8 + (width%8 ? 1 : 0);
  char* buf = new char[bwidth];
  char* origin; char value;
  for(int y = 0; y<height; y++) {
    Kernel::receive_bytes(buf, bwidth);
    for (int z = 0; z<width; z++) {
      origin = pixdata + y*rowstride + z*3;
      value = (buf[z >> 3] >> (z%8)) & 0x01;
      origin[0] = origin[1] = origin[2] = 255*value;
    }
    Gtk::Main::iteration(false);
  }
  
  pixbuf = Gdk::Pixbuf::create_from_data(reinterpret_cast<guint8*>(pixdata),
					 Gdk::COLORSPACE_RGB, false, 8,
					 width, height, rowstride);
  delete pixdata;
  
  drawingarea->set_size_request(width, height);
  set_size_request(width, height);

  show_all();
  raise();
}

// GTK callbacks
// =============

bool DitherWindow::_on_expose_event(GdkEventExpose *e) 
{
  if (!pixbuf) return true;
  drawingarea->get_window()->draw_pixbuf(drawingarea->get_style()->get_fg_gc(Gtk::STATE_NORMAL),
					 pixbuf, e->area.x, e->area.y,
					 e->area.x, e->area.y,
					 width, height, Gdk::RGB_DITHER_NONE,
					 0, 0);
  return true;
}

bool DitherWindow::_on_button_press_event(GdkEventButton* e)
{
  if ((e->type == GDK_BUTTON_PRESS) 
      && (e->button == 3))
    {
      popup_menu->popup(e->button, e->time);
      script_win->set_status("Popup!");
      return true;
    }
  else 
    { 
      return false;
    }
}

void DitherWindow::_on_save_activate(void)
{
  if(filename.length() == 0) {
    _on_save_as_activate();
    return;
  }

  std::string s = "filename = \"";
  s += filename + "\";\n";
  if(filetype.length() > 0) {
    s += "dither_file_format = ";
    s += filetype + ";\n";
  }
  s += "save_dithered_image;\n";
  Kernel::send(s);
}

void DitherWindow::_on_save_as_activate(void)
{
  // TODO: implement filetype choice
  script_win->set_status("Save dither image as...");
  Gtk::FileChooserDialog dialog("Save dither image as...",
				Gtk::FILE_CHOOSER_ACTION_SAVE);
  dialog.set_transient_for(*this);

  //Add response buttons the the dialog:
  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);

  int result = dialog.run();

  if (result == Gtk::RESPONSE_OK) {
    filetype = "pbm";
    filename = dialog.get_filename();
    set_my_title();
    _on_save_activate();
  }
}

void DitherWindow::_on_close_activate(void)
{ 
  hide();
  pixbuf.reset();
}
