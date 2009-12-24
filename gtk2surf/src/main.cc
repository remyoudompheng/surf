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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif
#include <stdlib.h>

#ifdef HAVE_GETOPT_LONG
# define _GNU_SOURCE_H
# include <getopt.h>
#else
#define getopt_long(argc, argv, optstring, longopts, longindex) \
  getopt(argc, argv, optstring)
#endif

#include <iostream>
#include "Kernel.h"

// Widgets
#include "ScriptWindow.h"
#include "ImageWindow.h"
#include "DitherWindow.h"

// Resources
#define KERNEL_BINARY BINDIR "/surf"
#define GLADEFILE PKGDATADIR "/gtk2surf.glade"

using namespace std;

namespace {
char usage_text[] =
"gtksurf is a graphical frontend to surf, the script driven visualization\n"
"tool for real algebraic geometry.\n"
"\n"
"Usage: gtk2surf [OPTION]... [FILE]\n"
"\n"
#ifdef HAVE_GETOPT_LONG
"  -k, --kernel=PATHTOKERNEL   load PATHTOKERNEL as surf kernel instead of\n"
"                               the default " KERNEL_BINARY "\n"
"  -g, --glade-file=GLADEFILE  use GLADEFILE as Glade GUI description file\n"
"                               instead of " GLADEFILE "\n"
"  -h, --help                  display this help and exit\n"
"  -V, --version               output version information and exit\n"
#else
"  -k PATHTOKERNEL     load PATHTOKERNEL as surf kernel instead of the\n"
"                       default " KERNEL_BINARY "\n"
"  -g PATHTOGLADEFILE  use PATHTOGLADEFILE as GUI description file\n"
"                       instead of " GLADEFILE "\n"
"  -h                  display this help and exit\n"
"  -V                  output version information and exit\n"
#endif // HAVE_GETOPT_LONG           
"\n"
"Report bugs to the SourceForge bug tracking system at\n"
  "http://sourceforge.net/tracker/?group_id=3275\n";

char version_txt[] = 
PACKAGE " " VERSION "\n"
"\n"
"Copyright (C) 2009 Rémy Oudompheng\n"
"\n"
"This is free software; you can redistribute it and/or modify it under the\n"
"terms of the GNU General Public License; either version 2 of the License,\n"
"or any later version.\n"
"\n"
"Written by Rémy Oudompheng.\n";
}

// parse options:
#ifdef HAVE_GETOPT_LONG
option longopts[] = {
  { "kernel", required_argument, 0, 'k' },
  { "glade-file", required_argument, 0, 'g' },
  { "help", no_argument, 0, 'h' },
  { "version", no_argument, 0, 'V' },
  { 0, 0, 0, 0 }
};
#endif

int
main (int argc, char *argv[])
{
  // default values:
  std::string kernel_path = KERNEL_BINARY;
  std::string gladefile = GLADEFILE;

  int c;
  while((c = getopt_long(argc, argv, "k:g:hV", longopts, 0)) != -1) {
    switch(c) {
    case 'k':
      kernel_path.assign(optarg);
      break;
    case 'g':
      gladefile.assign(optarg);
      break;
    case 'h':
      std::cout << usage_text;
      exit(0);
      break;
    case 'V':
      std::cout << version_txt;
      exit(0);
      break;
    default:
      std::cerr << "\nUnknown option!\n\n";
      std::cerr << usage_text;
      exit(1);
    }
  }

  Kernel::init(kernel_path);
  Gtk::Main kit(argc, argv);
  Glib::RefPtr<Gtk::Builder> refGlade = Gtk::Builder::create();

  // load the interface
  try {
    refGlade->add_from_file(gladefile);
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
  ImageWindow* image_win = 0;
  DitherWindow* dither_win = 0;
  refGlade->get_widget_derived("window_script", script_win);
  refGlade->get_widget_derived("window_image", image_win);
  refGlade->get_widget_derived("window_dither", dither_win);
  Kernel::set_scriptwin(script_win);
  Kernel::set_imagewin(image_win);
  Kernel::set_ditherwin(dither_win);
  if (script_win) {
    if(optind < argc) script_win->load_file(argv[optind]);
    script_win->show_all();
    kit.run(*script_win);
  }
  
  delete script_win;
  Kernel::deinit();

  return 0;
}
