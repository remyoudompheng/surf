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
	// read OOGL/OFF file from kernel:

	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	
	GLfloat color[4] = { 0.8, 0.8, 0.8, 1.0 };
	GLfloat insidecolor[4] = { 0.8, 0.8, 0.8, 1.0 };

	struct {
		GLfloat pos[4];
		GLfloat col[4];
	} cur_light;
	cur_light.col[3] = 1.0;
	int num_lights = 0;

	enum {
		neutral,
		appearance,
		material,
		lighting,
		light
	} state = neutral;
	
	bool end_of_file = false;
	while(!end_of_file) {
		std::string token = kernel.receive_string();
		if(token == "{") {
			continue;
		}

		switch(state) {
		case neutral:
			if(token == "appearance") {
				state = appearance;
			} else if(token == "NOFF") {
				read_noff();
				end_of_file = true;
			}
			break;
		case appearance:
			if(token == "material") {
				state = material;
			} else if(token == "lighting") {
				state = lighting;
			} else if(token == "}") {
				state = neutral;
			}
			break;
		case material:
			if(token == "ka") {
				// ambient reflection coefficient
				kernel.receive_float();
			} else if(token == "ambient") {
				GLfloat ambient[4];
				for(int i = 0; i != 3; i++) {
					ambient[i] = kernel.receive_float();
				}
				ambient[3] = 1.0;
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
			} else if(token == "kd") {
				// diffuse-reflection coefficient
				kernel.receive_float();
		        } else if(token == "diffuse") {
				for(int i = 0; i != 3; i++) {
					color[i] = kernel.receive_float();
				}
				color[3] = 1.0;
			} else if(token == "backdiffuse") {
				for(int i = 0; i != 3; i++) {
					insidecolor[i] = kernel.receive_float();
				}
			} else if(token == "ks") {
				// specular reflection coeff
				kernel.receive_float();
			} else if(token == "specular") {
				GLfloat specular[4];
				for(int i = 0; i != 3; i++) {
					specular[i] = kernel.receive_float();
				}
				specular[3] = 1.0;
				glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
				glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50.0);
			} else if(token == "alpha") {
				GLfloat alpha = kernel.receive_float();
				color[3] = insidecolor[3] = alpha;
				if(alpha != 1.0) {
					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE);
					glDepthMask(GL_FALSE);
				}
			} else if(token == "}") {
				glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
				glMaterialfv(GL_BACK, GL_DIFFUSE, insidecolor);
				state = appearance;
			}
			break;
		case lighting:
			if(token == "ambient") {
				GLfloat ambient[4];
				for(int i = 0; i != 3; i++) {
					ambient[i] = kernel.receive_float();
				}
				ambient[3] = 1.0;
				glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
			} else if(token == "light") {
				state = light;
			} else if(token == "}") {
				state = appearance;
			}
			break;
		case light:
			if(token == "color") {
				for(int i = 0; i != 3; i++) {
					cur_light.col[i] = kernel.receive_float();
				}
			} else if(token == "position") {
				for(int i = 0; i != 4; i++) {
					cur_light.pos[i] = kernel.receive_float();
				}
			} else if(token == "location") {
				// eat up location:
				kernel.receive_string();
			} else if(token == "}") {
				GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
				glLightfv(GL_LIGHT0 + num_lights, GL_DIFFUSE, cur_light.col);
				glLightfv(GL_LIGHT0 + num_lights, GL_SPECULAR, white);
				glLightfv(GL_LIGHT0 + num_lights, GL_POSITION, cur_light.pos);
				glEnable(GL_LIGHT0 + num_lights);
				num_lights++;
				state = lighting;
			}
			break;
		}
			
	}

	display();
}

void GLArea::read_noff()
{
	int num_vertices = kernel.receive_int();
	int num_faces = kernel.receive_int();
	kernel.receive_line(); // eat up edges count and '\n'

	std::cerr << "nv: " << num_vertices << ", nf: " << num_faces << "\n";

 	// read vertices:
	GLvertex* vertices = new GLvertex[num_vertices];
	for(int i = 0; i != num_vertices; i++) {
		vertices[i].x = kernel.receive_float();
		vertices[i].y = kernel.receive_float();
		vertices[i].z = kernel.receive_float();
		vertices[i].nx = kernel.receive_float();
		vertices[i].ny = kernel.receive_float();
		vertices[i].nz = kernel.receive_float();
	}

	// read faces:
	GLface* faces = new GLface[num_faces];
	for(int i = 0; i != num_faces; i++) {
		kernel.receive_int(); // should we check this?
		faces[i].p1 = kernel.receive_int();
		faces[i].p2 = kernel.receive_int();
		faces[i].p3 = kernel.receive_int();
	}

	while(kernel.receive_line() != "}") {
		// do nothing
	}

	// build OpenGL display list:
	if(display_list != 0) {
		glDeleteLists(display_list, 1);
	}
	display_list = glGenLists(1);

	glNewList(display_list, GL_COMPILE);
	glBegin(GL_TRIANGLES);

	for(int i = 0; i != num_faces; i++) {
		GLface& f = faces[i];
		GLvertex& p1 = vertices[f.p1];
		GLvertex& p2 = vertices[f.p2];
		GLvertex& p3 = vertices[f.p3];
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
}


// OpenGL drawing routines:
// ------------------------------------------------------------------------------

void GLArea::init(GLsizei width, GLsizei height)
{
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

	Kernel::Sequence seq[3];
	kernel.get_sequence(seq[0], seq[1], seq[2]);
	for(int i = 2; i != -1; i--) {
		switch(seq[i]) {
		case Kernel::translate:
			glTranslatef(origx, origy, origz);
			break;
		case Kernel::rotate:
			if(rotating) { // dragging in progress
				glMultMatrixf(deltaRotMat.getArray());
			}
			glMultMatrixf(rotMat.getArray());
			break;
		case Kernel::scale:
			glScalef(1/scalex, 1/scaley, 1/scalez);
			break;
		}
	}

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
	GLvector yaxis(0.0, -1.0, 0.0);
	GLvector zaxis(0.0, 0.0, 1.0);
	
	GLmatrix yrot(4);
	yrot.setToRotation(y*deg, yaxis);
	GLmatrix xrot(4);
	xrot.setToRotation(x*deg, xaxis);
	GLmatrix zrot(4);
	zrot.setToRotation(z*deg, zaxis);
	
	rotMat = yrot*xrot*zrot;

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

