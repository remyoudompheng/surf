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

namespace {
void sigchild_handler(int sig)
{
	std::cerr << "BUG ALERT: Kernel has just crashed!!\n"
		  << "You better save your script and exit quickly..\n";
}
}

Kernel::Kernel(const std::string& kernel_path)
{
	int to_kernel[2];
	int from_kernel[2];
	if(pipe(to_kernel) != 0 ||
	   pipe(from_kernel) != 0) {
		Misc::syscall_failed("pipe()");
	}

	
	// catch SIGCHILDs:
	struct sigaction sa;
	std::memset(static_cast<void*>(&sa), 0, sizeof(sa));
	sa.sa_handler = sigchild_handler;
	sigaction(SIGCHLD, &sa, 0);

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
		execl(kernel_path.c_str(), kernel_path.c_str(), 0);
		std::cerr << "\nERROR: Did you install the surf kernel properly?\n\n";
		std::string s = "execl(\"";
		s += kernel_path.c_str();
		s += "\", ...)";
		Misc::syscall_failed(s);
	}

	close(to_kernel[0]);
	close(from_kernel[1]);

	if(receive_string() != "surf") {
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

	for(int i = 0; i != 4; i++) {
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
		} else if(head == "sequence:") {
			for(int i = 0; i != 3; i++) {
				std::string s;
				is >> s;
				if(s == "rotate") {
					sequence[i] = rotate;
				} else if(s == "scale") {
					sequence[i] = scale;
				} else if(s == "translate") {
					sequence[i] = translate;
				} else {
					Misc::print_warning("Unkown sequence command!");
				}
			}
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
	
	if(s.length() == 0) {
		connect_handler();
		return;
	}

	if(s.compare(0, 7, "status ") == 0) {
		scriptwin->set_status(s.substr(7));
		scriptwin->progress_mode(true);
	} else if(s.compare(0, 9, "progress ") == 0) {
		s = s.substr(9);
		if(s == "done") {
			scriptwin->progress_mode(false);
			scriptwin->set_status("");
		} else if(s == "aborted") {
			scriptwin->progress_mode(false);
			scriptwin->set_status("Aborted.");
		} else {
			std::istrstream iss(s.c_str());
			int percent;
			iss >> percent;
			scriptwin->set_progress(percent/100.0);
		}
	} else if(s == "save_color_image") {
		imagewin->read_data();
	} else if(s == "save_dithered_image") {
		ditherwin->read_data();
	} else if(s == "save_three_d_image") {
		glarea->read_data();
	} else if(s == "clear_screen") {
		imagewin->clear();
	} else if(s == "not_implemented") {
		scriptwin->set_status("Feature not implemented in kernel!");
	} else if(s == "error") {
		std::string errorregion = receive_line();
		std::istrstream is(errorregion.c_str());
		int from, to;
		is >> from >> to;
		scriptwin->select_region(from, to);
		scriptwin->set_status(receive_line());
		gdk_beep();
	} else if(s.length() > 0) {
		std::string w = "Unrecognized line from kernel: ";
		Misc::print_warning(w + s);
	}

	connect_handler();
}
