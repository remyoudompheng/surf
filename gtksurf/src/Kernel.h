/*
 *   gtksurf
 *       - a GTK+ based graphical frontend to surf
 *
 *   Author: Johannes Beigel <jojo@users.sourceforge.net>
 *   Copyright (C) 2001 Johannes Beigel
 *   License: GPL version >= 2
 *
 */

#ifndef KERNEL_H
#define KERNEL_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include<string>

#ifdef HAVE_STRINGSTREAM
#  include<sstream>
#else
#  include<strstream>
#endif

#include<list>

#include <gtk/gtk.h>

class ScriptWindow;
class ImageWindow;
class DitherWindow;
//class GLArea;

class Kernel {
public:
	static void init(const std::string& kernel_path);
	static void deinit();

	static void set_scriptwin(ScriptWindow* sw) {
		scriptwin = sw;
	}
	static void set_imagewin(ImageWindow* iw) {
		imagewin = iw;
	}
	static void set_ditherwin(DitherWindow* dw) {
		ditherwin = dw;
	}
//	static void set_glarea(GLArea* gl) {
//		glarea = gl;
//	}

	static void connect_handler() {
		handler_id = gtk_input_add_full(kernel_output_fd, GDK_INPUT_READ,
						_process_output, 0, 0, 0);
	}
	static void disconnect_handler() {
		gtk_input_remove(handler_id);
	}

	static void send(const char* txt) {
		fputs(txt, kernel_input);
		fputs("\nexecute;\n", kernel_input);
		fflush(kernel_input);
	}
	static void send(const std::string& txt) {
		send(txt.c_str());
	}
	static std::string receive_line() {
		char buf[1024];
		std::string s;
		if(fgets(buf, sizeof(buf), kernel_output) != 0) {
			size_t len = strlen(buf);
			if(buf[len - 1] == '\n') {
				buf[len - 1] = 0;
			}
			s.assign(buf);
		}
		return s;
	}
	static std::string receive_string() {
		skip_space();
		std::string s;
		int c;
		while((c = getc(kernel_output)) != EOF) {
			if(isspace(c)) {
				ungetc(c, kernel_output);
				break;
			}
			s.push_back(char(c));
		}
		return s;
	}
	static int receive_int() {
		std::string s = receive_string();
#ifdef HAVE_STRINGSTREAM
		std::istringstream iss(s);
#else
		std::istrstream iss(s.c_str());
#endif
		int i;
		iss >> i;
		return i;
	}
	static float receive_float() {
		std::string s = receive_string();
#ifdef HAVE_STRINGSTREAM
		std::istringstream iss(s);
#else
		std::istrstream iss(s.c_str());
#endif
		float f;
		iss >> f;
		return f;
	}
	static void receive_bytes(char* buf, size_t len) {
		fread(buf, len, 1, kernel_output);
	}
	static char receive_byte() {
		return char(getc(kernel_output));
	}

	static void reset();

	static void stop();

	static std::list<std::string>& get_color_image_formats() {
		return color_image_formats;
	}
	static std::list<std::string>& get_dither_image_formats() {
		return dither_image_formats;
	}
//	static std::list<std::string>& get_three_d_image_formats() {
//		return three_d_image_formats;
//	}

	static void get_orig(double& x, double& y, double& z);
	static void get_rotation(double& x, double& y, double& z);
	static void get_scale(double& x, double& y, double& z);
	static void get_clip(int& clip, double& radius);
	enum Sequence {
		rotate,
		translate,
		scale
	};
	static void get_sequence(Sequence sequence[3]);
	static void get_curve_width(double& w);
	static void get_curve_gamma(double& g);

private:
	static int kernel_pid;
	static int kernel_input_fd;
	static FILE* kernel_input;
	static int kernel_output_fd;
	static FILE* kernel_output;

	static ScriptWindow* scriptwin;
	static ImageWindow* imagewin;
	static DitherWindow* ditherwin;
//	static GLArea* glarea;

	static std::string defaults;

	static std::list<std::string> color_image_formats;
	static std::list<std::string> dither_image_formats;
	static std::list<std::string> three_d_image_formats;

	static void skip_space() {
		int c;
		while((c = getc(kernel_output)) != EOF) {
			if(!isspace(c)) {
				ungetc(c, kernel_output);
				break;
			}
		}
	}

	static void check_version(const std::string& v);

	static guint handler_id;
	static void process_output();
	static void _process_output(gpointer d, gint, GdkInputCondition) {
		process_output();
	}
};

#endif //!KERNEL_H
