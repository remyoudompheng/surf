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

guint Kernel::handler_id;


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

	check_version(receive_string());
	receive_line();


	send("set_kernel_mode;");

	send("print_defaults;");
	defaults.assign("");
	std::string line = receive_line();
	while(line.length() > 0) {
		defaults += line;
		defaults += "\n";
		line = receive_line();
	}

	send("print_color_image_formats;");
	line = receive_line();
	while(line.length() > 0) {
		color_image_formats.push_back(line); // push name
		color_image_formats.push_back(receive_line()); // push id
		line = receive_line();
	}

	send("print_dither_image_formats;");
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

void Kernel::get_background(int& r, int& g, int& b)
{
	disconnect_handler();
	send("print_background;\n");
	std::string line;
	line = receive_line();
#ifdef HAVE_STRINGSTREAM
	std::istringstream is(line);
#else
	std::istrstream is(line.c_str());
#endif
	std::string head;
	is >> head;
	if(head == "background:") {
		is >> r >> g >> b;
	} else {
		Misc::print_warning("Scrambled kernel output!?");
	}
	connect_handler();
}

void Kernel::get_curve_color(int& r, int& g, int& b)
{
	disconnect_handler();
	send("print_curve_color;\n");
	std::string line;
	line = receive_line();
#ifdef HAVE_STRINGSTREAM
	std::istringstream is(line);
#else
	std::istrstream is(line.c_str());
#endif
	std::string head;
	is >> head;
	if(head == "curve_color:") {
		is >> r >> g >> b;
	} else {
		Misc::print_warning("Scrambled kernel output!?");
	}
	connect_handler();
}

void Kernel::get_orig(double& x, double& y, double& z)
{
	disconnect_handler();
	send("print_origin;\n");
	std::string line;
	line = receive_line();
#ifdef HAVE_STRINGSTREAM
	std::istringstream is(line);
#else
	std::istrstream is(line.c_str());
#endif
	std::string head;
	is >> head;
	if(head == "origin:") {
		is >> x >> y >> z;
	} else {
		Misc::print_warning("Scrambled kernel output!?");
	}
	connect_handler();
}

void Kernel::get_rotation(double& x, double& y, double& z)
{
	disconnect_handler();
	send("print_rotation;\n");
	std::string line;
	line = receive_line();
#ifdef HAVE_STRINGSTREAM
	std::istringstream is(line);
#else
	std::istrstream is(line.c_str());
#endif
	std::string head;
	is >> head;
	if(head == "rotation:") {
		is >> x >> y >> z;
	} else {
		Misc::print_warning("Scrambled kernel output!?");
	}
	connect_handler();
}

void Kernel::get_scale(double& x, double& y, double& z)
{
	disconnect_handler();
	send("print_scale;\n");
	std::string line;
	line = receive_line();
#ifdef HAVE_STRINGSTREAM
	std::istringstream is(line);
#else
	std::istrstream is(line.c_str());
#endif
	std::string head;
	is >> head;
	if(head == "scale:") {
		is >> x >> y >> z;
	} else {
		Misc::print_warning("Scrambled kernel output!?");
	}
	connect_handler();
}

void Kernel::get_clip(int& clip, double& radius)
{
	disconnect_handler();
	send("print_clip;\n");
	std::string line;
	line = receive_line();
#ifdef HAVE_STRINGSTREAM
	std::istringstream is(line);
#else
	std::istrstream is(line.c_str());
#endif
	std::string head;
	is >> head;
	if(head == "clip:") {
		is >> clip >> radius;
	} else {
		Misc::print_warning("Scrambled kernel output!?");
	}
	connect_handler();
}

void Kernel::get_sequence(Sequence sequence[3])
{
	disconnect_handler();
	send("print_sequence;\n");
	std::string line;
	line = receive_line();
#ifdef HAVE_STRINGSTREAM
	std::istringstream is(line);
#else
	std::istrstream is(line.c_str());
#endif
	std::string head;
	is >> head;
	if(head == "sequence:") {
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
	connect_handler();
}

void Kernel::get_curve_width(double& w)
{
	disconnect_handler();
	send("print_curve_width;\n");
	std::string line;
	line = receive_line();
#ifdef HAVE_STRINGSTREAM
	std::istringstream is(line);
#else
	std::istrstream is(line.c_str());
#endif
	std::string head;
	is >> head;
	if(head == "curve_width:") {
		is >> w;
	} else {
		Misc::print_warning("Scrambled kernel output!?");
	}
	connect_handler();
}

void Kernel::get_curve_gamma(double& g)
{
	disconnect_handler();
	send("print_curve_gamma;\n");
	std::string line;
	line = receive_line();
#ifdef HAVE_STRINGSTREAM
	std::istringstream is(line);
#else
	std::istrstream is(line.c_str());
#endif
	std::string head;
	is >> head;
	if(head == "curve_gamma:") {
		is >> g;
	} else {
		Misc::print_warning("Scrambled kernel output!?");
	}
	connect_handler();
}

void Kernel::get_curve_color(double& r, double& g, double &b)
{
	disconnect_handler();
	send("print_curve_color;\n");
	std::string line;
	line = receive_line();
#ifdef HAVE_STRINGSTREAM
	std::istringstream is(line);
#else
	std::istrstream is(line.c_str());
#endif
	std::string head;
	is >> head;
	if(head == "curve_color:") {
		is >> r >> g >> b;
	} else {
		Misc::print_warning("Scrambled kernel output!?");
	}
	connect_handler();
}

void Kernel::get_background_color(double& r, double& g, double &b)
{
	disconnect_handler();
	send("print_background_color;\n");
	std::string line;
	line = receive_line();
#ifdef HAVE_STRINGSTREAM
	std::istringstream is(line);
#else
	std::istrstream is(line.c_str());
#endif
	std::string head;
	is >> head;
	if(head == "background_color:") {
		is >> r >> g >> b;
	} else {
		Misc::print_warning("Scrambled kernel output!?");
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
#ifdef HAVE_STRINGSTREAM
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
#ifdef HAVE_STRINGSTREAM
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
