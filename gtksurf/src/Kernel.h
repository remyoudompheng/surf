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
class GLArea;

class Kernel {
public:
	Kernel(const std::string& kernel_path);
	virtual ~Kernel();

	void set_scriptwin(ScriptWindow* sw) {
		scriptwin = sw;
	}
	void set_imagewin(ImageWindow* iw) {
		imagewin = iw;
	}
	void set_ditherwin(DitherWindow* dw) {
		ditherwin = dw;
	}
	void set_glarea(GLArea* gl) {
		glarea = gl;
	}

	void connect_handler() {
		handler_id = gtk_input_add_full(kernel_output_fd, GDK_INPUT_READ,
						_process_output, 0, this, 0);
	}
	void disconnect_handler() {
		gtk_input_remove(handler_id);
	}

	void send(const char* txt) {
		fputs(txt, kernel_input);
		fputs("\nexecute;\n", kernel_input);
		fflush(kernel_input);
	}
	void send(const std::string& txt) {
		send(txt.c_str());
	}
	std::string receive_line() {
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
	std::string receive_string() {
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
	int receive_int() {
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
	float receive_float() {
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
	void receive_bytes(char* buf, size_t len) {
		fread(buf, len, 1, kernel_output);
	}
	char receive_byte() {
		return char(getc(kernel_output));
	}

	void reset();

	void stop();

	std::list<std::string>& get_color_image_formats() {
		return color_image_formats;
	}
	std::list<std::string>& get_dither_image_formats() {
		return dither_image_formats;
	}
	std::list<std::string>& get_three_d_image_formats() {
		return three_d_image_formats;
	}

	void update_position();
	void get_orig(double& x, double& y, double& z) const {
		x = orig_x;
		y = orig_y;
		z = orig_z;
	}
	void get_rotation(double& x, double& y, double& z) const {
		x = rot_x;
		y = rot_y;
		z = rot_z;
	}
	void get_scale(double& x, double& y, double& z) const {
		x = scale_x;
		y = scale_y;
		z = scale_z;
	}
	enum Sequence {
		rotate,
		translate,
		scale
	};
	void get_sequence(Sequence& first, Sequence& second, Sequence& third) const {
		first = sequence[0];
		second = sequence[1];
		third = sequence[2];
	}

private:
	int kernel_pid;
	int kernel_input_fd;
	FILE* kernel_input;
	int kernel_output_fd;
	FILE* kernel_output;

	ScriptWindow* scriptwin;
	ImageWindow* imagewin;
	DitherWindow* ditherwin;
	GLArea* glarea;

	std::string defaults;

	std::list<std::string> color_image_formats;
	std::list<std::string> dither_image_formats;
	std::list<std::string> three_d_image_formats;

	double orig_x;
	double orig_y;
	double orig_z;
	double rot_x;
	double rot_y;
	double rot_z;
	double scale_x;
	double scale_y;
	double scale_z;
	Sequence sequence[3];
	
	void skip_space() {
		int c;
		while((c = getc(kernel_output)) != EOF) {
			if(!isspace(c)) {
				ungetc(c, kernel_output);
				break;
			}
		}
	}

	void check_version(const std::string& v);

	guint handler_id;
	void process_output();
	static void _process_output(gpointer This, gint, GdkInputCondition) {
		static_cast<Kernel*>(This)->process_output();
	}
};

#endif //!KERNEL_H
