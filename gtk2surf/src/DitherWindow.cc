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
  if (!bitmap) {
    return;
  }
  bitmap.reset();
  bitmap = Gdk::Bitmap::create(drawingarea->get_window(),
			       0, width, height);
  show();
}

namespace {
  guint8 reverse_bits(guint8 b)
  {
    return    (b >> 7) & 0x01
      | (b >> 5) & 0x02
      | (b >> 3) & 0x04
      | (b >> 1) & 0x08
      | (b << 1) & 0x10
      | (b << 3) & 0x20
      | (b << 5) & 0x40
      | (b << 7) & 0x80;
  }
}

void DitherWindow::read_data() 
{
  if (bitmap) bitmap.reset();

  std::string whstring = Kernel::receive_line();
#ifdef HAVE_SSTREAM
  std::istringstream is(whstring);
#else
  std::istrstream is(whstring.c_str());
#endif
  is >> width >> height;

  size_t bwidth = width/8 + (width%8 ? 1 : 0); // width in bytes
  size_t length = bwidth*height;
  guint8* data = new guint8[length];
  for(size_t i = 0; i < length; i++) {
    data[i] = reverse_bits(Kernel::receive_byte());
  }
  
  bitmap = Gdk::Bitmap::create(drawingarea->get_window(),
		  reinterpret_cast<gchar*>(data),
		  width, height);
  delete data;
  
  drawingarea->set_size_request(width, height);
  set_size_request(width, height);

  show_all();
  raise();
}

// GTK callbacks
// =============

bool DitherWindow::_on_expose_event(GdkEventExpose *e) 
{
  if (!bitmap) return true;
  drawingarea->get_window()->draw_drawable(drawingarea->get_style()->get_fg_gc(Gtk::STATE_NORMAL),
					   bitmap, e->area.x, e->area.y,
					   e->area.x, e->area.y,
					   width, height);
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
  bitmap.reset();
}
