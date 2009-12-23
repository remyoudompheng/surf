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
#include "About.h"

#include<fstream>
#include<iostream>

static char modified_txt[] = 
  "The current script has been modified.\n"
  "Do you want to save it?";

ScriptWindow::ScriptWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade)
  : Gtk::Window(cobject),
    myGlade(refGlade),
    text_script(0),
    sbar(0),
    pbar(0)
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
  Gtk::MenuItem* mi_clear = 0;
  Gtk::MenuItem* mi_select_all = 0;

  myGlade->get_widget("cut", mi_cut);
  myGlade->get_widget("copy", mi_copy);
  myGlade->get_widget("paste", mi_paste);
  myGlade->get_widget("clear", mi_clear);
  myGlade->get_widget("select_all", mi_select_all);
  
  mi_cut->signal_activate().connect( sigc::mem_fun(*this, &ScriptWindow::_on_cut_activate) );
  mi_copy->signal_activate().connect( sigc::mem_fun(*this, &ScriptWindow::_on_copy_activate) );
  mi_paste->signal_activate().connect( sigc::mem_fun(*this, &ScriptWindow::_on_paste_activate) );
  mi_clear->signal_activate().connect( sigc::mem_fun(*this, &ScriptWindow::_on_clear_activate) );
  mi_select_all->signal_activate().connect( sigc::mem_fun(*this, &ScriptWindow::_on_select_all_activate) );

  // Command Menu callbacks
  Gtk::MenuItem* mi_render_curve = 0;
  Gtk::MenuItem* mi_render_surface = 0;
  Gtk::MenuItem* mi_execute = 0;

  myGlade->get_widget("render_curve", mi_render_curve);
  myGlade->get_widget("render_surface", mi_render_surface);
  myGlade->get_widget("execute", mi_execute);

  mi_render_curve->signal_activate().connect( sigc::mem_fun(*this, &ScriptWindow::_on_render_curve_activate) );
  mi_render_surface->signal_activate().connect( sigc::mem_fun(*this, &ScriptWindow::_on_render_surface_activate) );
  mi_execute->signal_activate().connect( sigc::mem_fun(*this, &ScriptWindow::_on_execute_activate) );

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

  Gtk::ToolButton* but_render_curve = 0;
  Gtk::ToolButton* but_render_surface = 0;
  Gtk::ToolButton* but_execute = 0;
  myGlade->get_widget("button_render_curve", but_render_curve);
  myGlade->get_widget("button_render_surface", but_render_surface);
  myGlade->get_widget("button_execute", but_execute);
  but_render_curve->signal_clicked().connect( sigc::mem_fun(*this, &ScriptWindow::_on_render_curve_activate) );
  but_render_surface->signal_clicked().connect( sigc::mem_fun(*this, &ScriptWindow::_on_render_surface_activate) );
  but_execute->signal_clicked().connect( sigc::mem_fun(*this, &ScriptWindow::_on_execute_activate) );

  // Other widgets
  myGlade->get_widget("statusbar_script", sbar);
  myGlade->get_widget("progressbar_script", pbar);
  myGlade->get_widget("text_script", text_script);
  text_buffer = text_script->get_buffer();
  text_buffer->signal_changed().connect( sigc::mem_fun(*this, &ScriptWindow::_on_script_changed) );
  refClipboard = Gtk::Clipboard::get();

}

ScriptWindow::~ScriptWindow()
{
}

void ScriptWindow::set_my_title()
{
  std::string title = PACKAGE;
  title += " - ";
  if(filename.length() == 0) {
    title += "Untitled";
  } else {
    title += filename;
  }
  set_title(Glib::ustring(title));
}

void ScriptWindow::set_status(const std::string& txt)
{
  sbar->pop();
  sbar->push(Glib::ustring(txt));
  sbar->show();
}

void ScriptWindow::set_progress(double percentage)
{
  pbar->set_fraction(percentage);
  pbar->show();
}

void ScriptWindow::load_file(const std::string& fname)
{
  std::ifstream file(fname.c_str());
  if(!file) {
    set_status("Open failed: Couldn't open file!");
    Gdk::Display::get_default()->beep();
    return;
  }

  text_script->set_editable(false);
  text_buffer->erase(text_buffer->begin(), text_buffer->end());
  std::string buf;
  while (getline(file,buf)) {
    try {
      text_buffer->insert_at_cursor(Glib::locale_to_utf8(buf + '\n'));
    }
    catch (const Glib::ConvertError& ex)
      {
	std::cerr << "ConvertError: " << ex.what()  << std::endl;
      }
  }
  
  text_buffer->place_cursor(text_buffer->begin());
  text_script->set_editable(true);

  dirty = false;
  filename.assign(fname);
  set_my_title();
  set_status("Script loaded.");
}


// GTK callbacks
// =============

void ScriptWindow::_on_script_changed() 
{
  dirty = true;
}

void ScriptWindow::_on_new_activate() 
{
  if(dirty) {
    Gtk::MessageDialog dialog(*this, "Unsaved changes",
			      false, Gtk::MESSAGE_QUESTION,
			      Gtk::BUTTONS_YES_NO);
    dialog.set_secondary_text(modified_txt);
    int result = dialog.run();
    if (result == Gtk::RESPONSE_YES) {
      _on_save_activate();
    }
  }

  filename.erase();
  set_my_title();
  text_script->set_editable(false);
  text_buffer->erase(text_buffer->begin(), text_buffer->end());
  text_script->set_editable(true);

  dirty = false;
}

void ScriptWindow::_on_open_activate()
{
  if(dirty) {
    Gtk::MessageDialog dialog(*this, "Unsaved changes",
			      false, Gtk::MESSAGE_QUESTION,
			      Gtk::BUTTONS_YES_NO);
    dialog.set_secondary_text(modified_txt);
    int result = dialog.run();
    if (result == Gtk::RESPONSE_YES) {
      _on_save_activate();
    }
  }

  set_status("Open Script...");

  Gtk::FileChooserDialog dialog("Open script...",
				Gtk::FILE_CHOOSER_ACTION_OPEN);
  dialog.set_transient_for(*this);

  //Add response buttons the the dialog:
  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);

  int result = dialog.run();
  if (result == Gtk::RESPONSE_OK) load_file(dialog.get_filename());
}

void ScriptWindow::_on_save_activate() {
  if(filename.length() == 0) {
    _on_save_as_activate();
    return;
  }

  set_status("Save Script...");
  Glib::ustring scr = text_buffer->get_text();
  {
    std::ofstream file(filename.c_str());
    if(!file) {
      set_status("Saving failed: Couldn't open file!\n");
      Gdk::Display::get_default()->beep();
      return;
    }
    file << scr;
  }

  dirty = false;

  set_status("Script saved.");
}

void ScriptWindow::_on_save_as_activate() 
{
  set_status("Save script as...");

  Gtk::FileChooserDialog dialog("Save script as...",
				Gtk::FILE_CHOOSER_ACTION_SAVE);
  dialog.set_transient_for(*this);

  //Add response buttons the the dialog:
  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);

  int result = dialog.run();

  if (result == Gtk::RESPONSE_OK) {
    filename = dialog.get_filename();
    _on_save_activate();
  }
}

void ScriptWindow::_on_prefs_activate() {}

void ScriptWindow::_on_quit_activate() 
{
  if(dirty) {
    Gtk::MessageDialog dialog(*this, "Unsaved changes",
			      false, Gtk::MESSAGE_QUESTION,
			      Gtk::BUTTONS_YES_NO);
    dialog.set_secondary_text(modified_txt);
    int result = dialog.run();
    if (result == Gtk::RESPONSE_YES) {
      _on_save_activate();
    }
  }

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

void ScriptWindow::_on_clear_activate() {
  text_buffer->erase_selection();
}

void ScriptWindow::_on_select_all_activate() {
  text_buffer->select_range(text_buffer->begin(), text_buffer->end());
}

void ScriptWindow::_on_render_curve_activate() {}

void ScriptWindow::_on_render_surface_activate() {
  ImageWindow* image_win = 0;
  myGlade->get_widget_derived("window_image", image_win);
  image_win->show_all();
  image_win->raise();
}

void ScriptWindow::_on_execute_activate() {}

void ScriptWindow::_on_about_activate() {
  About* about_win = 0;
  myGlade->get_widget_derived("window_about", about_win);
  about_win->show_all();
}
