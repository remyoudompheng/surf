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

struct GLvertex {
	GLfloat x;
	GLfloat y;
	GLfloat z;
};

struct GLedge {
	size_t from;
	size_t to;
};

struct GLface {
	size_t a;
	size_t b;
	size_t c;
};

#endif //!GLTYPES_H
