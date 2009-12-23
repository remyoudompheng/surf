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
//#include <GLArea.h>

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

#include <cstring>

int Kernel::kernel_pid;
int Kernel::kernel_input_fd;
FILE* Kernel::kernel_input;
int Kernel::kernel_output_fd;
FILE* Kernel::kernel_output;

ScriptWindow* Kernel::scriptwin;
ImageWindow* Kernel::imagewin;
DitherWindow* Kernel::ditherwin;
//GLArea* Kernel::glarea;

std::string Kernel::defaults;

std::list<std::string> Kernel::color_image_formats;
std::list<std::string> Kernel::dither_image_formats;
//std::list<std::string> Kernel::three_d_image_formats;

Glib::RefPtr<Glib::IOSource> Kernel::handler_id;


namespace {
void sigchild_handler(int sig)
{
	std::cerr << "BUG ALERT: Kernel has just crashed!!\n"
		  << "You better save your script and exit quickly..\n";
}
}

void Kernel::init(const std::string& kernel_path)
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
	kernel_input = fdopen(kernel_input_fd, "w");
	kernel_output_fd = from_kernel[0];
	kernel_output = fdopen(kernel_output_fd, "r");
	
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

		execl(kernel_path.c_str(), kernel_path.c_str(), "-k", 0);
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

	check_version(receive_string());
	receive_line();


	// send("set_kernel_mode;");

	send("print_defaults;\n");
	defaults.assign("");
	std::string line = receive_line();
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

/*	send("print_three_d_image_formats;");
	line = receive_line();
	while(line.length() > 0) {
		three_d_image_formats.push_back(line); // name
		three_d_image_formats.push_back(receive_line()); // id
		line = receive_line();
	}
*/

	connect_handler();
}

void Kernel::deinit()
{
        signal(SIGCHLD, SIG_IGN);
	fclose(kernel_input);
	fclose(kernel_output);
}

void Kernel::check_version(const std::string& version)
{
	if(version == "version") { // output of surf 1.0.x
		Misc::print_error("You need a newer surf kernel: surf v1.1.0 or later is required!\n");
	}
}

void Kernel::reset()
{
	send(defaults);
	send("reset;\n");
}

std::string Kernel::get_string(const std::string& name)
{
	disconnect_handler();
	send("variable = \"" + name + "\";\n"
	     "print_variable;\n");
	std::string line = receive_line();
#ifdef HAVE_SSTREAM
	std::istringstream is(line);
#else
	std::istrstream is(line);
#endif
	std::string head;
	is >> head;
	std::string s = "";
	if(head == name + ":") {
		is >> s;
	} else {
		Misc::print_error("Scrambled kernel output!?\n");
	}
	connect_handler();
	return s;
}

int Kernel::get_int(const std::string& name)
{
	disconnect_handler();
	send("variable = \"" + name + "\";\n"
	     "print_variable;\n");
	std::string line = receive_line();
#ifdef HAVE_SSTREAM
	std::istringstream is(line);
#else
	std::istrstream is(line);
#endif
	std::string head;
	is >> head;
	int i = 0;
	if(head == name + ":") {
		is >> i;
	} else {
		Misc::print_error("Scrambled kernel output!?\n");
	}
	connect_handler();
	return i;
}

double Kernel::get_double(const std::string& name)
{
	disconnect_handler();
	send("variable = \"" + name + "\";\n"
	     "print_variable;\n");
	std::string line = receive_line();
#ifdef HAVE_SSTREAM
	std::istringstream is(line);
#else
	std::istrstream is(line);
#endif
	std::string head;
	is >> head;
	double d = 0;
	if(head == name + ":") {
		is >> d;
	} else {
		Misc::print_error("Scrambled kernel output!?\n");
	}
	connect_handler();
	return d;
}

void Kernel::stop()
{
	kill(kernel_pid, SIGHUP);
}

void Kernel::process_output()
{
	disconnect_handler();
	
	std::string s = receive_line();
#ifdef DEBUG
	std::cout << s << "\n";
#endif
	if(s.length() == 0) {
		connect_handler();
		return;
	}

	if(s.substr(0, 7) == "status ") {
		scriptwin->set_status(s.substr(7) + "...");
	} else if(s.substr(0, 9) == "progress ") {
		s = s.substr(9);
		if(s == "done") {
			scriptwin->progress_mode(false);
			scriptwin->set_status("");
		} else if(s == "aborted") {
			scriptwin->progress_mode(false);
			scriptwin->set_status("Aborted.");
		} else {
#ifdef HAVE_SSTREAM
			std::istringstream iss(s);
#else
			std::istrstream iss(s.c_str());
#endif
			int percent;
			iss >> percent;
			scriptwin->set_progress(percent/100.0);
		}
	} else if(s == "P6") { // PPM image
		imagewin->read_data();
	} else if(s == "P4") { // PBM image
		ditherwin->read_data();
//	} else if(s == "{") { // OOF data follows
//		glarea->read_data();
	} else if(s == "clear_screen") {
		imagewin->clear();
	} else if(s == "not_implemented") {
		scriptwin->set_status("Feature not implemented in kernel!");
		gdk_beep();
	} else if(s == "error") {
		std::string errorregion = receive_line();
#ifdef HAVE_SSTREAM
		std::istringstream is(errorregion);
#else
		std::istrstream is(errorregion.c_str());
#endif
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
