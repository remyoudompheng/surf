/*
 *   gtksurf
 *       - a GTK+ based graphical frontend to surf
 *
 *   Author: Johannes Beigel <jojo@users.sourceforge.net>
 *   Copyright (C) 2001 Johannes Beigel
 *   License: GPL version >= 2
 *
 */

#include <GLArea.h>
#include <NavigationWindow.h>
#include <ScriptWindow.h>
#include <Misc.h>

#include<iostream>
#include<cstdlib>
#include<cmath>
#include<strstream>

GLArea::GLArea(Glade& _glade, Kernel& _kernel, NavigationWindow* navwin,
	       ScriptWindow* sw)
	: glade(_glade), kernel(_kernel), navigationwin(navwin), scriptwin(sw),
	  pi(std::acos(-1.0)),
	  dragging(false),
	  rotMat(4), deltaRotMat(4), rotating(false),
	  showCross(false), wireframe(false), centralPerspective(true),
	  origx(0.0), origy(0.0), origz(0.0),
	  scalex(1.0), scaley(1.0), scalez(1.0),
	  shown(false),
	  display_list(0)
{
	kernel.set_glarea(this);
	
	if(!gdk_gl_query()) {
		glade.show_message("ERROR: OpenGL not supported!\n");
		abort();
	}

	rotMat.setToIdentity();
	
	glarea = gtk_gl_area_new_vargs(0,
				       GDK_GL_RGBA,
				       GDK_GL_DOUBLEBUFFER,
				       GDK_GL_DEPTH_SIZE, 8,
				       GDK_GL_NONE);

	gtk_widget_set_events(glarea,
			      GDK_EXPOSURE_MASK |
			      GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK |
			      GDK_BUTTON1_MOTION_MASK |
			      GDK_POINTER_MOTION_HINT_MASK);

	gtk_signal_connect(GTK_OBJECT(glarea), "expose_event",
			   GTK_SIGNAL_FUNC(_expose_event), (gpointer)this);
	gtk_signal_connect(GTK_OBJECT(glarea), "configure_event",
			   GTK_SIGNAL_FUNC(_configure_event), (gpointer)this);
	gtk_signal_connect(GTK_OBJECT(glarea), "realize",
			   GTK_SIGNAL_FUNC(_realize), (gpointer)this);
	gtk_signal_connect(GTK_OBJECT(glarea), "button_press_event",
			   GTK_SIGNAL_FUNC(_button_press_event), (gpointer)this);
	gtk_signal_connect(GTK_OBJECT(glarea), "button_release_event",
			   GTK_SIGNAL_FUNC(_button_release_event),
			   (gpointer)this);

	glade.sig_connect("togglecross", "activate", _on_togglecross_activate, this);
	glade.sig_connect("togglewireframe", "activate", _on_togglewireframe_activate, this);
	glade.sig_connect("central_perspective", "activate", _on_perspective_activate, this);
	glade.sig_connect("parallel_perspective", "activate", _on_perspective_activate, this);
	
	popupmenu = glade.get_widget("menu_glarea");
	
	gtk_widget_set_usize(glarea, 256, 256);
	gtk_widget_show(glarea);
}

GLArea::~GLArea()
{
	if(display_list) {
		glDeleteLists(display_list, 1);
	}
}

void GLArea::read_data()
{
	scriptwin->progress_mode(true);
	scriptwin->set_progress(0);

	// read surface & inside colors:
	GLfloat color[4];
	color[0] = kernel.receive_float();
	color[1] = kernel.receive_float();
	color[2] = kernel.receive_float();
	kernel.receive_line();
	color[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
	color[0] = kernel.receive_float();
	color[1] = kernel.receive_float();
	color[2] = kernel.receive_float();
	kernel.receive_line();
	glMaterialfv(GL_BACK, GL_AMBIENT_AND_DIFFUSE, color);

	// read light sources:
	int num_lights = kernel.receive_int();
	kernel.receive_line();
	GLfloat pos[4];
	pos[3] = 1.0;
	GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
	for(int i = 0; i != num_lights; i++) {
		color[0] = kernel.receive_float();
		color[1] = kernel.receive_float();
		color[2] = kernel.receive_float();
		pos[0] = kernel.receive_float();
		pos[1] = kernel.receive_float();
		pos[2] = kernel.receive_float();
		kernel.receive_line();
		glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, color);
		glLightfv(GL_LIGHT0 + i, GL_SPECULAR, white);
		glLightfv(GL_LIGHT0 + i, GL_POSITION, pos);
		glEnable(GL_LIGHT0 + i);
	}

	// read number of vertices/faces:
	int nv = kernel.receive_int();
	int nf = kernel.receive_int();
	kernel.receive_line();

	std::cerr << "nv: " << nv << ", nf: " << nf << "\n";

	size_t total = nv + nf;

	if(nv == 0 || nf == 0) {
		Misc::print_warning("There were no vertices/no faces at all!\n");
		return;
	}

	// read vertices:
	GLvertex* vertices = new GLvertex[nv];
	for(int i = 0; i != nv; i++) {
		vertices[i].x = kernel.receive_float();
		vertices[i].y = kernel.receive_float();
		vertices[i].z = kernel.receive_float();
		vertices[i].nx = kernel.receive_float();
		vertices[i].ny = kernel.receive_float();
		vertices[i].nz = kernel.receive_float();
		kernel.receive_line(); // eat up '\n'

		scriptwin->set_progress(i/gfloat(total));
	}

	// read faces:
	GLface* faces = new GLface[nf];
	for(int i = 0; i != nf; i++) {
		faces[i].p1 = kernel.receive_int();
		faces[i].p2 = kernel.receive_int();
		faces[i].p3 = kernel.receive_int();
		kernel.receive_line(); // eat up '\n'

		scriptwin->set_progress((nv + i)/gfloat(total));
	}

	if(kernel.receive_line() != "end") {
		Misc::print_warning("Transmission from kernel didn't end properly\n");
	}
	
	scriptwin->set_status("");
	scriptwin->progress_mode(false);

	// build OpenGL display list:
	if(display_list != 0) {
		glDeleteLists(display_list, 1);
	}
	display_list = glGenLists(1);

	glNewList(display_list, GL_COMPILE);
	glBegin(GL_TRIANGLES);

	for(int i = 0; i != nf; i++) {
		GLface& f = faces[i];
		GLvertex& p1 = vertices[f.p1 - 1];
		GLvertex& p2 = vertices[f.p2 - 1];
		GLvertex& p3 = vertices[f.p3 - 1];
		glNormal3f(p1.nx, p1.ny, p1.nz);
		glVertex3f(p1.x, p1.y, p1.z);
		glNormal3f(p2.nx, p2.ny, p2.nz);
		glVertex3f(p2.x, p2.y, p2.z);
		glNormal3f(p3.nx, p3.ny, p3.nz);
		glVertex3f(p3.x, p3.y, p3.z);
	}
	glEnd();
	glEndList();

	delete [] vertices;
	delete [] faces;

	display();
}


// OpenGL drawing routines:
// ------------------------------------------------------------------------------

void GLArea::init(GLsizei width, GLsizei height)
{
	// define material:
	static GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
	static GLfloat ambient[] = { 0.2, 0.2, 0.2, 1.0 };
	static GLfloat shininess = 50.0;
	
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	reshape(width, height);
}

void GLArea::reshape(GLsizei _width = 0, GLsizei _height = 0)
{
	if(_width > 0 && _height > 0) {
		width = _width;
		height = _height;
	}
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if(centralPerspective) {
		glFrustum(-5.0, 5.0, -5.0, 5.0, 10.0, 30.0);
	} else {
		glOrtho(-10.0, 10.0, -10.0, 10.0, 10.0, 30.0);
	}
	glMatrixMode(GL_MODELVIEW);
}

void GLArea::display()
{
	if(!shown) {
		return;
	}
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    

	// position camera:
	
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -20.0);

	glEnable(GL_LIGHTING);

	glTranslatef(origx, origy, origz);

	if(rotating) { // dragging in progress
		glMultMatrixf(deltaRotMat.getArray());
	}
	glMultMatrixf(rotMat.getArray());

	glScalef(1/scalex, 1/scaley, 1/scalez);

	if(wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if(display_list != 0) {
		glCallList(display_list);
	}

	// draw coord system w/lightin turned off:

	if(showCross) {
		glDisable(GL_LIGHTING);
		glColor3f(1.0, 1.0, 0.5);
		glScalef(5.0, 5.0, 5.0);
		plotCross();
	}


	swapBuffers(); // swap front- & backbuffer
}

void GLArea::plotCross()
{
	static GLfloat crossVertices[] = {
		// x-axis + arrow
		-1.0, 0.0, 0.0,    1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,     0.9, 0.1, 0.0,
		1.0, 0.0, 0.0,     0.9, -0.1, 0.0,
		// y-axis + arrow
		0.0, -1.0, 0.0,    0.0, 1.0, 0.0,
		0.0, 1.0, 0.0,     0.1, 0.9, 0.0,
		0.0, 1.0, 0.0,     -0.1, 0.9, 0.0,
		// z-axis + arrow
		0.0, 0.0, -1.0,    0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,     0.1, 0.0, 0.9,
		0.0, 0.0, 1.0,     -0.1, 0.0, 0.9
	};
	glVertexPointer(3, GL_FLOAT, 0, crossVertices);
	glDrawArrays(GL_LINES, 0, sizeof(crossVertices)/sizeof(GLfloat)/3);
}

void GLArea::drag_begin(gint x, gint y)
{
	rotating = true;
	rotateFrom = mouseOnSphere(x, y);
}

void GLArea::drag_move(gint x, gint y)
{
	GLvector rotateTo = mouseOnSphere(x, y);
	GLfloat angle = std::acos(rotateFrom.dotProduct(rotateTo));
	GLvector axis = rotateFrom.crossProduct(rotateTo);
	deltaRotMat.setToRotation(-angle, axis);
	update_navwin(deltaRotMat*rotMat);
	display();
}

void GLArea::drag_end(gint x, gint y)
{
	rotMat = rotMat*deltaRotMat;
	rotating = false;
	update_navwin(rotMat);
}

void GLArea::update_navwin(const GLmatrix& rotd)
{
	// extract angles from rotation matrix:
	GLfloat sin_x, cos_x, sin_y, cos_y, sin_z, cos_z;
	GLfloat rad_x;

	sin_x = -rotd[2*4 + 1];
	rad_x = std::asin(sin_x);
	cos_x = std::cos(rad_x);
	cos_y = rotd[2*4 + 2] / cos_x;
	sin_y = rotd[2*4 + 0] / cos_x;
	sin_z = rotd[0*4 + 1] / cos_x;
	cos_z = rotd[1*4 + 1] / cos_x;

	trimSinCos(sin_y);
	trimSinCos(cos_y);
	trimSinCos(sin_z);
	trimSinCos(cos_z);
	
	gfloat rx = std::atan2(sin_x, cos_x);
	gfloat ry = std::atan2(sin_y, cos_y);
	gfloat rz = std::atan2(sin_z, cos_z);
	trimAngle(rx);
	trimAngle(ry);
	trimAngle(rz);

	GLfloat rad = 180.0/pi;
	navigationwin->set_rot(rx*rad, ry*rad, rz*rad);
}

inline GLvector GLArea::mouseOnSphere(gint x, gint y)
{
	GLvector ballMouse;
	if(width >= height) {
		ballMouse.x = -(GLfloat(x)/width - 0.5)*2*(width/height);
		ballMouse.y = (GLfloat(y)/height - 0.5)*2;
	} else {
		ballMouse.x = -(GLfloat(x)/width - 0.5)*2;
		ballMouse.y = (GLfloat(y)/height - 0.5f)*2*(height/width);
	}
	
	GLfloat mag = ballMouse.x*ballMouse.x + ballMouse.y*ballMouse.y;
	if(mag > 1.0) {
		GLfloat scale = 1.0/std::sqrt(mag);
		ballMouse.x *= scale;
		ballMouse.y *= scale;
		ballMouse.z = 0.0;
	} else {
		ballMouse.z = std::sqrt(1 - mag);
	}
	
	return ballMouse;
}

// Called from NavigationWindow:
// ------------------------------------------------------------------------------

void GLArea::set_orig(gfloat x, gfloat y, gfloat z)
{
        origx = x;
	origy = y;
	origz = z;
	display();
}

void GLArea::set_scale(gfloat x, gfloat y, gfloat z)
{
	scalex = x;
	scaley = y;
	scalez = z;
	display();
}

void GLArea::set_rot(gfloat x, gfloat y, gfloat z)
{
	GLfloat deg = pi/180.0;

	GLvector xaxis(1.0, 0.0, 0.0);
	GLvector yaxis(0.0, 1.0, 0.0);
	GLvector zaxis(0.0, 0.0, -1.0);
	
	GLmatrix yrot(4);
	yrot.setToRotation(y*deg, yaxis);
	GLmatrix xrot(4);
	xrot.setToRotation(x*deg, xaxis);
	GLmatrix zrot(4);
	zrot.setToRotation(z*deg, zaxis);
	
	rotMat = zrot*xrot*yrot;

	display();
}

// Gtk event callbacks:
// ------------------------------------------------------------------------------

gint GLArea::realize()
{
	if(!gtk_gl_area_make_current(GTK_GL_AREA(glarea))) {
		return false;
	}
	width = glarea->allocation.width;
	height = glarea->allocation.height;
	init(width, height);
	return true;
}

gint GLArea::expose_event(GdkEventExpose* event)
{
	if(event->count > 0) {
		return true;
	}
	if(!gtk_gl_area_make_current(GTK_GL_AREA(glarea))) {
		return false;
	}
	display();
	return true;
}

gint GLArea::configure_event(GdkEventConfigure* event)
{
	if(!gtk_gl_area_make_current(GTK_GL_AREA(glarea))) {
		return false;
	}
	width = glarea->allocation.width;
	height = glarea->allocation.height;
	reshape(width, height);
	return true;
}

gint GLArea::button_press_event(GdkEventButton* event)
{
	if(event->button == 1 && !dragging) {
		dragging = true;
		motion_conn_id =
			gtk_signal_connect(GTK_OBJECT(glarea),
					   "motion_notify_event",
					   GTK_SIGNAL_FUNC(_motion_notify_event),
					   (gpointer)this);
		drag_begin((gint)event->x, (gint)event->y);
	} else if(event->button == 3) {
		gtk_menu_popup(GTK_MENU(popupmenu), 0, 0, 0, 0, 3, event->time);
	}

	return true;
}

gint GLArea::button_release_event(GdkEventButton* event)
{
	if(dragging) {
		dragging = false;
		drag_end((gint)event->x, (gint)event->y);
		gtk_signal_disconnect(GTK_OBJECT(glarea), motion_conn_id);
	}
	return true;
}

gint GLArea::motion_notify_event(GdkEventMotion* event)
{
	gint x, y;
	gdk_window_get_pointer(event->window, &x, &y, 0);
	drag_move(x, y);
	return true;
}

void GLArea::on_togglecross_activate()
{
	showCross = !showCross;
}

void GLArea::on_togglewireframe_activate()
{
	wireframe = !wireframe;
}

void GLArea::on_perspective_activate()
{
	GtkCheckMenuItem* item = GTK_CHECK_MENU_ITEM(glade.get_widget("central_perspective"));
	centralPerspective = item->active;
	reshape();
}

