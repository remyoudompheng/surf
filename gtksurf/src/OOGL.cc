/*
 *   gtksurf
 *       - a GTK+ based graphical frontend to surf
 *
 *   Author: Johannes Beigel <jojo@users.sourceforge.net>
 *   Copyright (C) 2001 Johannes Beigel
 *   License: GPL version >= 2
 *
 */

#include <OOGL.h>
#include <GLtypes.h>

namespace {
void read_header(Kernel& kernel);
GLuint read_noff(Kernel& kernel);
}

namespace OOGL {

GLuint read(Kernel& kernel)
{
        read_header(kernel);
	return read_noff(kernel);
}

}

namespace {

void read_header(Kernel& kernel)
{
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
				glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 25.0);
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
				glLightfv(GLenum(GL_LIGHT0 + num_lights), GL_DIFFUSE, cur_light.col);
				glLightfv(GLenum(GL_LIGHT0 + num_lights), GL_SPECULAR, white);
				glLightfv(GLenum(GL_LIGHT0 + num_lights), GL_POSITION, cur_light.pos);
				glEnable(GLenum(GL_LIGHT0 + num_lights));
				num_lights++;
				state = lighting;
			}
			break;
		}
	}
}

GLuint read_noff(Kernel& kernel)
{
	int num_vertices = kernel.receive_int();
	int num_faces = kernel.receive_int();
	kernel.receive_line(); // eat up edges count and '\n'

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

	GLuint display_list = glGenLists(1);

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

	return display_list;
}

} // anon namespace
