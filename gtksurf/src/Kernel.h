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

#include <gtk/gtk.h>

#include<fstream>
#include<string>
#include<list>

class ScriptWindow;
class ImageWindow;

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

	void connect_handler() {
		handler_id = gtk_input_add_full(kernel_output_fd, GDK_INPUT_READ,
						_process_output, 0, this, 0);
	}
	void disconnect_handler() {
		gtk_input_remove(handler_id);
	}

	void send(const char* txt) {
		*kernel_input << txt << "execute;\n";
		kernel_input->flush();
	}
	void send(const std::string& txt) {
		*kernel_input << txt << "execute;\n";
		kernel_input->flush();
	}
	std::string receive_line() {
		std::string s;
		std::getline(*kernel_output, s);
		return s;
	}
	int receive_int() {
		int i;
		*kernel_output >> i;
		return i;
	}
	void receive_bytes(unsigned char* buf, size_t len) {
		kernel_output->read(buf, len);
	}
	unsigned char receive_byte() {
		unsigned char c;
		kernel_output->get(c);
		return c;
	}

	void reset();

	void stop();

	std::list<std::string>& get_color_image_formats() {
		return color_image_formats;
	}
	std::list<std::string>& get_dither_image_formats() {
		return dither_image_formats;
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

private:
	int kernel_pid;
	int kernel_input_fd;
	std::ofstream* kernel_input;
	int kernel_output_fd;
	std::ifstream* kernel_output;

	ScriptWindow* scriptwin;
	ImageWindow* imagewin;

	std::string defaults;

	std::list<std::string> color_image_formats;
	std::list<std::string> dither_image_formats;

	double orig_x;
	double orig_y;
	double orig_z;
	double rot_x;
	double rot_y;
	double rot_z;
	double scale_x;
	double scale_y;
	double scale_z;
	
	enum {
		NEUTRAL,
		COLOR_IMAGE_HEADER,
		COLOR_IMAGE_DATA,
		STOPPED
	} state;

	struct {
		int width;
		int height;
		size_t length;
		size_t rowstride;
		guchar* pixdata;
		int mod;
		int y;
	} image;

	guint handler_id;
	void process_output();
	static void _process_output(gpointer This, gint, GdkInputCondition) {
		static_cast<Kernel*>(This)->process_output();
	}
};

#endif //!KERNEL_H
