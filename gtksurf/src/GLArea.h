/*
 *   gtksurf
 *       - a GTK+ based graphical frontend to surf
 *
 *   Author: Johannes Beigel <jojo@users.sourceforge.net>
 *   Copyright (C) 2001 Johannes Beigel
 *   License: GPL version >= 2
 *
 */
/*
 * $Id$
 *
 * Copyright (C) 2001 Johannes Beigel <jojo@beigel.de>
 * License: GNU General Public License Version >= 2
 */

#ifndef GLAREA_H
#define GLAREA_H

#include <Glade.h>
#include <Vec3.h>
#include <Matrix.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <gtk/gtk.h>
#include <gtkgl/gtkglarea.h>

#undef MENUCB
#define MENUCB(name) \
        void on_##name##_activate();\
        static void _on_##name##_activate(GtkWidget*, GladeWindow* This) {\
                static_cast<GLArea*>(This)->on_##name##_activate();\
        }

class NavigationWindow;

class GLArea : public GladeWindow {
public:
	GLArea(Glade& glade, NavigationWindow* navwin);
	virtual ~GLArea() {}

	GtkWidget* getWidget() {
		return glarea;
	}

	void set_orig(gfloat x, gfloat y, gfloat z);
	void set_scale(gfloat x, gfloat y, gfloat z);
	void set_rot(gfloat x, gfloat y, gfloat z);

	void show() {
		shown = true;
	}
	void hide() {
		shown = false;
	}
	
private:
	void init(GLsizei width, GLsizei height);
	void reshape(GLsizei width, GLsizei height);
	void display();

	void drag_begin(gint x, gint y);
	void drag_move(gint x, gint y);
	void drag_end(gint x, gint y);
	void update_navwin(const GLmatrix& mat);
	GLvector mouseOnSphere(gint x, gint y);

	void trimSinCos(GLfloat& sc) {
		if(sc > 1.0) {
			sc = 1.0;
		} else if(sc < -1.0) {
			sc = -1.0;
		}
	}

	void trimAngle(GLfloat angle) {
		if(angle < -180.0) {
			angle = -180.0;
		} else if (angle > 180.0) {
			angle = 180.0;
		}
	}

	Glade& glade;
	NavigationWindow* navigationwin;

	GLfloat pi;
	
	GtkWidget* glarea;
	GtkWidget* popupmenu;

	guint motion_conn_id;
	bool dragging;
	GLmatrix rotMat;
	GLmatrix deltaRotMat;
	GLvector rotateFrom;
	bool rotating;

	bool showCross;
	bool wireframe;

	GLfloat origx, origy, origz;
	GLfloat scalex, scaley, scalez;

	bool shown;

	GLsizei width;
	GLsizei height;

	void plotCross();
	
	void swapBuffers() {
		gtk_gl_area_swapbuffers(GTK_GL_AREA(glarea));
	}

	// callbacks:

	gint realize();
	static gint _realize(GtkWidget* widget, GLArea* _this) {
		return _this->realize();
	}
	gint expose_event(GdkEventExpose* event);
	static gint _expose_event(GtkWidget* widget, GdkEventExpose* event, GLArea* _this) {
		return _this->expose_event(event);
	}
	gint configure_event(GdkEventConfigure* event);
	static gint _configure_event(GtkWidget* widget, GdkEventConfigure* event, GLArea* _this) {
		return _this->configure_event(event);
	}
	gint button_press_event(GdkEventButton* event);
	static gint _button_press_event(GtkWidget* widget, GdkEventButton* event, GLArea* _this) {
		return _this->button_press_event(event);
	}
	gint button_release_event(GdkEventButton* event);
	static gint _button_release_event(GtkWidget* widget, GdkEventButton* event, GLArea* _this) {
		return _this->button_release_event(event);
	}
	gint motion_notify_event(GdkEventMotion* event);
	static gint _motion_notify_event(GtkWidget* widget, GdkEventMotion* event, GLArea* _this) {
		return _this->motion_notify_event(event);
	}

	MENUCB(togglecross);
	MENUCB(togglewireframe);
};

#endif //!GLAREA_H
