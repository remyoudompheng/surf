/*
 *   gtksurf
 *       - a GTK+ based graphical frontend to surf
 *
 *   Author: Johannes Beigel <jojo@users.sourceforge.net>
 *   Copyright (C) 2001 Johannes Beigel
 *   License: GPL version >= 2
 *
 */


#ifndef GLTYPES_H
#define GLTYPES_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <GL/gl.h>

struct GLvertex {
	GLfloat x;
	GLfloat y;
	GLfloat z;
	GLfloat nx;
	GLfloat ny;
	GLfloat nz;
};

struct GLface {
	unsigned int p1;
	unsigned int p2;
	unsigned int p3;
};

#endif //!GLTYPES_H
