#ifndef OPTIONS_H
#define OPTIONS_H

class Options {
public:
	Options(int argc, char* argv[]);
	virtual ~Options() {};

	int get_argc() const { return argc; }
	char** get_argv() const { return argv; }
	int getFirstFileIdx() const { return fileopts; }
	bool get_no_gui() const { return no_gui; }
	bool get_exec() const { return execute; }
	bool get_progress_dialog() const { return progress_dialog; }
	bool get_auto_resize() const { return auto_resize; }

private:
	int argc;
	char** argv;
	int fileopts;
	bool no_gui;
	bool execute;
	bool progress_dialog;
	bool auto_resize;
};

extern Options* options;

#endif // !OPTIONS_H
