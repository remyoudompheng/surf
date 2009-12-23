#include <gtkmm.h>
#include <iostream>

// Widgets
#include "ScriptWindow.h"
#include "About.h"

using namespace std;

void
ok_button_clicked()
{
  cout << "Thanks for trying out my program." << endl;
  Gtk::Main::quit();
}

int
main (int argc, char *argv[])
{
  Gtk::Main kit(argc, argv);
  Glib::RefPtr<Gtk::Builder> refGlade = Gtk::Builder::create();

  // load the interface
  try {
    refGlade->add_from_file("gtk2surf.glade");
  }
  catch(const Glib::FileError& ex)
    {
      std::cerr << "FileError: " << ex.what() << std::endl;
      return 1;
    }
  catch(const Gtk::BuilderError& ex)
    {
      std::cerr << "BuilderError: " << ex.what() << std::endl;
      return 1;
    }

  ScriptWindow* script_win = 0;
  refGlade->get_widget_derived("window_script", script_win);
  if (script_win) {
    script_win->show_all();
    kit.run(*script_win);
  }
  
  delete script_win;

  return 0;
}