/*
 *   gtksurf
 *       - a GTK+ based graphical frontend to surf
 *
 *   Author: Johannes Beigel <jojo@users.sourceforge.net>
 *   Copyright (C) 2001 Johannes Beigel
 *   License: GPL version >= 2
 *
 */


#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <Kernel.h>
#include <Misc.h>
#include <ScriptWindow.h>
#include <ImageWindow.h>
#include <DitherWindow.h>
#include <GLArea.h>

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

#include<strstream>

Kernel::Kernel(const std::string& kernel_path)
{
	int to_kernel[2];
	int from_kernel[2];
	if(pipe(to_kernel) != 0 ||
	   pipe(from_kernel) != 0) {
		Misc::syscall_failed("pipe()");
	}

	kernel_input_fd = to_kernel[1];
	kernel_input = new std::ofstream(kernel_input_fd);
	kernel_output_fd = from_kernel[0];
	kernel_output = new std::ifstream(kernel_output_fd);
	
	kernel_pid = fork();
	if(kernel_pid == -1) {
		Misc::syscall_failed("fork()");
	}

	if(kernel_pid == 0) {

		// child code:

		close(to_kernel[1]);
		close(from_kernel[0]);
		
		if(dup2(to_kernel[0], STDIN_FILENO) == -1 ||
		   dup2(from_kernel[1], STDOUT_FILENO) == -1) {
			Misc::syscall_failed("dup2()");
		}
		execl(kernel_path.c_str(), kernel_path.c_str(), "-q", 0);
		std::cerr << "\nERROR: Did you install the surf kernel properly?\n\n";
		std::string s = "execl(\"";
		s += kernel_path.c_str();
		s += "\", ...)";
		Misc::syscall_failed(s);
	}

	close(to_kernel[0]);
	close(from_kernel[1]);

	send("kernel_mode_set;\n");

	if(receive_line() != "surf") {
		Misc::print_error("This isn't a surf kernel on the other side of the pipe, is it?");
	}
	std::string version = receive_line();

	send("print_defaults;\n");
	defaults.assign("");
	std::string line;
	line = receive_line();
	while(line.length() > 0) {
		defaults += line;
		defaults += "\n";
		line = receive_line();
	}

	send("print_color_image_formats;\n");
	line = receive_line();
	while(line.length() > 0) {
		color_image_formats.push_back(line); // push name
		color_image_formats.push_back(receive_line()); // push id
		line = receive_line();
	}

	send("print_dither_image_formats;\n");
	line = receive_line();
	while(line.length() > 0) {
		dither_image_formats.push_back(line); // name
		dither_image_formats.push_back(receive_line()); // id
		line = receive_line();
	}

	connect_handler();
}

Kernel::~Kernel()
{
	kernel_input->close();
	kernel_output->close();
}

void Kernel::reset()
{
	send(defaults);
	send("reset;\n");
}

void Kernel::update_position()
{
	disconnect_handler();

	send("print_position;\n");

	for(int i = 0; i != 3; i++) {
		std::string line;
		line = receive_line();
		std::istrstream is(line.c_str());
		std::string head;
		is >> head;
		if(head == "origin:") {
			is >> orig_x >> orig_y >> orig_z;
		} else if(head == "rotation:") {
			is >> rot_x >> rot_y >> rot_z;
		} else if(head == "scale:") {
			is >> scale_x >> scale_y >> scale_z;
		} else {
			Misc::print_warning("Scrambled kernel output!?");
		}
	}

	connect_handler();
}

void Kernel::stop()
{
	kill(kernel_pid, SIGHUP);
}

void Kernel::process_output()
{
	disconnect_handler();
	
	std::string s = receive_line();

	if(s == "draw_surface") {
		imagewin->set_mode(ImageWindow::SURFACE);
		scriptwin->set_status("Rendering surface...");
		imagewin->read_data();
	} else if(s == "draw_curve") {
		imagewin->set_mode(ImageWindow::CURVE);
		scriptwin->set_status("Rendering curve...");
		imagewin->read_data();
	} else if(s == "antialiased_image") {
		scriptwin->set_status("Antialiasing image...");
		imagewin->read_data();
	} else if(s == "dither_surface") {
		imagewin->set_mode(ImageWindow::SURFACE);
		ditherwin->read_data();
	} else if(s == "dither_curve") {
		imagewin->set_mode(ImageWindow::CURVE);
		ditherwin->read_data();
	} else if(s == "clear_screen") {
		imagewin->clear();
	} else if(s == "triangulate_surface") {
		scriptwin->set_status("Triangulating surface...");
		glarea->read_data();
	} else if(s == "not_implemented") {
		scriptwin->set_status("Feature not implemented in kernel!");
	} else if(s == "error") {
		std::string reason = receive_line();
		std::string txt = "ERROR in script: " + reason;
		scriptwin->set_status(txt);
		
		std::string errorregion = receive_line();
		std::istrstream is(errorregion.c_str());
		int from, to;
		is >> from >> to;
		scriptwin->select_region(from, to);
		
		gdk_beep();
	} else {
		std::string w = "Unrecognized line from kernel: ";
		if(s.length() == 0) {
			s.assign("<empty line>");
		}
		Misc::print_warning(w + s);
	}

	connect_handler();
}
