/*
 *   surf - visualizing algebraic curves and algebraic surfaces
 *   Copyright (C) 1996-1997 Friedrich-Alexander-Universitaet
 *                           Erlangen-Nuernberg
 *                 1997-2000 Johannes Gutenberg-Universitaet Mainz
 *   Authors: Stephan Endrass, Hans Huelf, Ruediger Oertel,
 *            Kai Schneider, Ralf Schmitt, Johannes Beigel
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */


#include <math.h>

#include "Vector.h"
#include "gui_config.h"
#include "debug.h"
#include "cube.h"


double  cb,cf;

static  double  sz,szsq;
static  double  cx,cxsq,cy,cysq,cz,czsq;
static  double  r,rsq;
static  double  d1,d1sq;

static  double  A0,A1,A2;
static  double  B0,B1;
static  double  DISCR,DISCR1;
static  double  a0,a1,a2,a3,a4,a5,a6,a7,a8,a9;
static  double  b0,b1,b2,b3,b4,b5,b6,b7,b8;

static  double  discr,discr1,discr2;
static  double  z_y_min,z_y_max;

static  Vector  cut_normal;
static  Vector  cut_point;

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------

static  double  surface_cut_z_value( double,double )
{
    return  cut_numeric_data[CUT_Z_VALUE];
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------

int cut_normal_init (void)
{
	Vector  p1,p2,p3;

	p1.x = cut_numeric_data[CUT_P1_X];
	p1.y = cut_numeric_data[CUT_P1_Y];
	p1.z = cut_numeric_data[CUT_P1_Z];

	p2.x = cut_numeric_data[CUT_P2_X];
	p2.y = cut_numeric_data[CUT_P2_Y];
	p2.z = cut_numeric_data[CUT_P2_Z];

	p3.x = cut_numeric_data[CUT_P3_X];
	p3.y = cut_numeric_data[CUT_P3_Y];
	p3.z = cut_numeric_data[CUT_P3_Z];

	p2 -= p1; // VECTOR_SELF_SUBTRACT( p2,p1 );
	p3 -= p1; // VECTOR_SELF_SUBTRACT( p3,p1 );
	cut_normal = CrossProduct (p2,p3);
	// VECTOR_CROSS_PRODUCT( cut_normal,p2,p3 );
	cut_point = p1;

	Vector NULL_VECTOR (0,0,0);
	if( vector_equal( cut_normal, NULL_VECTOR ) ) {
		cut_normal = Vector(0.0, 0.0, 1.0); // vector_Z_AXIS;
		return  FALSE;
	} else {
		return  TRUE;
	}
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------

static  double  surface_cut_three_planes( double ux,double uy )
{
	return  ( cut_normal.x*( cut_point.x - ux ) +
		  cut_normal.y*( cut_point.y - uy ) )/cut_normal.z +
		cut_point.z;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------

static  double  (*surface_cut_front[2])( double, double ) =
{
	surface_cut_z_value,
	surface_cut_three_planes
};

// ----------------------------------------------------------------------------
//  Clipping routines: currently implemented
//      0 sphere
//      1 cylinder horizontal
//      2 cylinder vertical
//      3 cylinder in direction of z-axis
//      4 cube
//      5 none
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  Test if  user-y-coordinate is within sphere
//  center x-coord.: clip_numeric_data[CLIP_CENTER_X]
//  center y-coord.: clip_numeric_data[CLIP_CENTER_Y]
//  center z-coord.: clip_numeric_data[CLIP_CENTER_Z]
//  radius:          clip_numeric_data[CLIP_RADIUS]
// ----------------------------------------------------------------------------

// ---------------------------------------
//  The following data saves computations
// ---------------------------------------



void    surface_clip_init( void )
{
	sz = position_numeric_data[POSITION_SPECTATOR_Z];

	cb = clip_numeric.clib_back;
	cf = clip_numeric.clip_front;
	cx = clip_numeric_data[CLIP_CENTER_X];
	cy = clip_numeric_data[CLIP_CENTER_Y];
	cz = clip_numeric_data[CLIP_CENTER_Z];
	r  = clip_numeric_data[CLIP_RADIUS]; 
	d1 = sz - cz;

	szsq = sz*sz;
	cxsq = cx*cx;
	cysq = cy*cy;
	czsq = cz*cz;
	rsq  = r*r;
	d1sq = d1*d1;
}

// ----------------------------------------------------------------------------
//  Test: user-y-coordinate is within clipping sphere, projection = parallel
// ----------------------------------------------------------------------------

static  void    surface_clip_user_y_in_sphere_parallel_init( void )
{
}

static  int     surface_clip_user_y_in_sphere_parallel( double uy )
{
	return  fabs( uy - cy ) <= r;
}

// ----------------------------------------------------------------------------
//  Test: user-y-coordinate is within clipping sphere, projection = central
// ----------------------------------------------------------------------------

static  void    surface_clip_user_y_in_sphere_central_init( void )
{
	DISCR = d1sq + cysq - rsq;
	A0    = cy*sz*d1;
	A1    = sz*r;
	A2    = d1sq - rsq;

	if( DISCR >= 0.0 ) {
		DISCR1 = sqrt( DISCR );

		B0 = ( A0 - A1*DISCR1 )/A2;
		B1 = ( A0 + A1*DISCR1 )/A2;
	}

	a0 = cx*cy;
	a1 = cx*sz*d1;
	a2 = d1sq + cxsq + cysq - rsq;
	a3 = rsq - d1sq; 
	a4 = 2*cy*sz*d1;
	a5 = szsq*( rsq - cysq );
	a6 = d1sq + cysq - rsq;
	
	
	b0 = -( d1sq + cysq - rsq );
	b1 = 2*cx*cy;
	b2 = 2*sz*cx*d1;
	b3 = -( d1sq + cxsq - rsq );
	b4 = 2*sz*cy*d1;
	b5 = szsq*( rsq - cxsq - cysq );
}

static  int     surface_clip_user_y_in_sphere_central( double uy )
{
	if( DISCR < 0 ) {
		return  TRUE;
	}

	return  uy >= B0 && uy <= B1;
}

// ----------------------------------------------------------------------------
//  Test: user-x-coordinate in within clipping sphere, projection = parallel
// ----------------------------------------------------------------------------

static  void    surface_clip_user_xy_in_sphere_parallel_init( double uy )
{
	A0 = uy - cy;
	A0 *= A0;
}

static  int     surface_clip_user_xy_in_sphere_parallel( double ux,double,
                        double *zmin,double *zmax )
{
	double  g = ux - cx;
	double  h = g*g + A0;

	if( h <= rsq ) {
		h = sqrt( rsq - h );
		*zmin = cz - h;
		*zmax = cz + h;
		*zmin = max( *zmin,cb );
		*zmax = min( *zmax,cf );
		
		return  TRUE;
	} else {
		return  FALSE;
	}
}

// ----------------------------------------------------------------------------
//  Test: user-x-coordinate in within clipping sphere, projection = parallel
// ----------------------------------------------------------------------------

static void surface_clip_user_xy_in_sphere_central_init (double uy)
{
	b6 = uy*uy;
	b7 = b1*uy + b2;
	b8 = b3*b6 + b4*uy + b5;

	discr1 = a2*( a3*b6 + a4*uy + a5 );
	
	if( discr1 >= 0.0 ) {
		discr2 = sqrt( discr1 );
		a7 = a0*uy + a1;
		a8 = ( a7 - discr2 )/a6;
		a9 = ( a7 + discr2 )/a6;
	}
}

static  int     surface_clip_user_xy_in_sphere_central (double ux, double uy,
							double *zmin, double *zmax)
{
	if( discr1 >= 0.0 ) {
		if( ux < a8 || ux > a9 ) {
			return  FALSE;
		}
	} else {
		return FALSE;
	}

	double  uxsq = ux*ux;
	double  discr_b = b0*uxsq + ux*b7 + b8;
	discr_b = sqrt( discr_b );
	
	double  a = uxsq - ux*cx + b6 - uy*cy + sz*cz;
	double  b = sz/( uxsq + b6 + szsq );
	
	*zmin = ( a - discr_b )*b;
	*zmax = ( a + discr_b )*b;
	*zmin = max( *zmin,cb );
	*zmax = min( *zmax,cf );
	
	return  TRUE;
}

// ----------------------------------------------------------------------------
//  Test: user-x-coordinate in within clipping hoizontal cylinder,
//        projection = parallel
// ----------------------------------------------------------------------------

static  void    surface_clip_user_xy_in_cylhoriz_parallel_init( double uy )
{
	a0 = uy - cy;
	a0 *= a0;
}

static  int     surface_clip_user_xy_in_cylhoriz_parallel( double,double,
							   double *zmin,double *zmax )
{
	double  h = sqrt( rsq - a0 );
	*zmin = cz - h;
	*zmax = cz + h;
	*zmin = max( *zmin,cb );
	*zmax = min( *zmax,cf );

	return  TRUE;
}

// ----------------------------------------------------------------------------
//  Test: user-x-coordinate in within clipping hoizontal cylinder,
//        projection = central
// ----------------------------------------------------------------------------

static  void    surface_clip_user_y_in_cylhoriz_central_init( void )
{
	DISCR = d1sq + cysq - rsq;
	A0    = cy*sz*d1;
	A1    = sz*r;
	A2    = d1sq - rsq;

	if( DISCR >= 0.0 ) {
		DISCR1 = sqrt( DISCR );

		B0 = ( A0 - A1*DISCR1 )/A2;
		B1 = ( A0 + A1*DISCR1 )/A2;
	}

	a0 = sz;
	a1 = -sz*cy;
	a2 = szsq*cz;
	a3 = rsq - d1sq;
	a4 = 2*sz*cy*d1;
	a5 = szsq*( rsq - cysq );
}

static  int     surface_clip_user_y_in_cylhoriz_central( double uy )
{
	if( uy < B0 || uy > B1 ) {
		return  FALSE;
	} else {
		return  TRUE;
	}
}

static  void    surface_clip_user_xy_in_cylhoriz_central_init( double uy )
{
	double  uysq = uy*uy;

	discr = a3*uysq + a4*uy + a5;
	b0 = a0*uysq + a1*uy + a2;
	b1 = uysq + szsq;
	
	discr = sz*sqrt( discr );

	z_y_max = ( b0 + discr )/b1;
	z_y_min = ( b0 - discr )/b1;

	z_y_max = min( z_y_max,cf );
	z_y_min = max( z_y_min,cb );
}

static  int     surface_clip_user_xy_in_cylhoriz_central( double,double,
							  double *zmin,double *zmax )
{
	*zmax = z_y_max;
	*zmin = z_y_min;

	return  TRUE;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------

static  void    surface_clip_user_y_in_cylvert_parallel_init( void )
{
}

static  int     surface_clip_user_y_in_cylvert_parallel( double )
{
	return  TRUE;
}

static  void    surface_clip_user_y_in_cylvert_central_init( void )
{
	DISCR = d1sq + cxsq - rsq;
	A0    = cx*sz*d1;
	A1    = sz*r;
	A2    = d1sq - rsq;

	if( DISCR >= 0.0 ) {
		DISCR1 = sqrt( DISCR );

		B0 = ( A0 - A1*DISCR1 )/A2;
		B1 = ( A0 + A1*DISCR1 )/A2;
	}

	a0 = sz;
	a1 = -sz*cx;
	a2 = szsq*cz;
	a3 = rsq - d1sq;
	a4 = 2*sz*cx*d1;
	a5 = szsq*( rsq - cxsq );
}

static  int     surface_clip_user_y_in_cylvert_central( double )
{
	return  TRUE;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------

static  void    surface_clip_user_xy_in_cylvert_parallel_init( double )
{
}

static  int     surface_clip_user_xy_in_cylvert_parallel( double ux,double, 
                        double *zmin,double *zmax )
{
	double  h = ux - cx;
	h *= h;

	if( h <= rsq ) {
		h = sqrt( rsq - h );
		*zmin = cz - h;
		*zmax = cz + h;
		*zmin = max( *zmin,cb );
		*zmax = min( *zmax,cf );

		return  TRUE;
	} else {
		return  FALSE;
	}
}

static  void    surface_clip_user_xy_in_cylvert_central_init( double )
{
}

static  int     surface_clip_user_xy_in_cylvert_central( double ux,double,
                        double *zmin,double *zmax )
{
	if( ux < B0 || ux > B1 ) {
		return  FALSE;
	} else {
		double  uxsq = ux*ux;
		
		discr = a3*uxsq + a4*ux + a5;
		b0 = a0*uxsq + a1*ux + a2;
		b1 = uxsq + szsq;

		discr = sz*sqrt( discr );
		
		*zmax = ( b0 + discr )/b1;
		*zmin = ( b0 - discr )/b1;
		
		*zmax = min( *zmax,cf );
		*zmin = max( *zmin,cb );
		
		return  TRUE;
	}
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------

static  void    surface_clip_user_xy_in_cylz_parallel_init( double uy )
{
	a0 = uy - cy;
	a0 *= a0;
}

static  int     surface_clip_user_xy_in_cylz_parallel( double ux,double,
                        double *zmin,double *zmax )
{
	double  g = ux - cx;
	
	if( g*g + a0 <= rsq ) {
		*zmin = cb;
		*zmax = cf;

		return  TRUE;
	} else {   
		return  FALSE;
	}
}

static  void    surface_clip_user_y_in_cylz_central_init( void )
{
}

static  int     surface_clip_user_y_in_cylz_central( double )
{
	return  TRUE;
}

static  void    surface_clip_user_xy_in_cylz_central_init( double )
{
}

static  int     surface_clip_user_xy_in_cylz_central( double,double,
                        double *zmin,double *zmax )
{
	*zmax = cf;
	*zmin = cb;

	return  TRUE;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------

static  void    surface_clip_user_y_in_cube_parallel_init( void )
{
}

static  int     surface_clip_user_y_in_cube_parallel( double )
{
	return  TRUE;
}

static  void    surface_clip_user_xy_in_cube_parallel_init( double )
{
}


static  int     surface_clip_user_xy_in_cube_parallel( double ux,double uy,
						       double *zmin,double *zmax )
{
	double  z_value[6];
	int     z_flag[6];
	double  temp;
	int     i,j;
	
	for( i = 0; i < 6; i++ ) {
		if( clip_cube.equations[i][2] != 0.0 ) {
			z_value[i] = -( clip_cube.equations[i][0]*ux +
					clip_cube.equations[i][1]*uy +
					clip_cube.equations[i][3] )/
				clip_cube.equations[i][2];
			z_flag[i] = FALSE;
		} else {
			z_value[i] = clip_cube.equations[i][0]*ux +
				clip_cube.equations[i][1]*uy +
				clip_cube.equations[i][3];
			z_flag[i] = TRUE;
		}
	}

	*zmin = cb;
	*zmax = cf;

	for( i = 0; i < 6; i += 2 ) {
		j = i + 1;
		
		if( z_flag[i] == FALSE && z_flag[j] == FALSE ) {
			if( z_value[i] > z_value[j] ) {
				temp = z_value[i];
				z_value[i] = z_value[j];
				z_value[j] = temp;
			}

			*zmin = max( *zmin,z_value[i] );
			*zmax = min( *zmax,z_value[j] );
		} else {
			if( z_value[i]*z_value[j] > 0.0 ) {
				*zmin =  1.0;
				*zmax = -1.0;
			}
		}
	}

	return ( *zmin < *zmax );
}


static  void    surface_clip_user_y_in_cube_central_init( void )
{
}


static  int     surface_clip_user_y_in_cube_central( double )
{
	return  TRUE;
}


static  void    surface_clip_user_xy_in_cube_central_init( double )
{
}



static  int     surface_clip_user_xy_in_cube_central( double ux,double uy,
                 	double *zmin,double *zmax )
{
	double  z_value[6];
	int     z_flag[6];
	double  hlp,temp;
	int     i,j;

	for( i = 0; i < 6; i++ ) {
		hlp = clip_cube.equations[i][0]*ux +
			clip_cube.equations[i][1]*uy -
			clip_cube.equations[i][2]*
			position_numeric_data[POSITION_SPECTATOR_Z];
		
		if( hlp != 0.0 ) {
			z_value[i] = position_numeric_data[POSITION_SPECTATOR_Z]*
				( clip_cube.equations[i][0]*ux +
				  clip_cube.equations[i][1]*uy +
				  clip_cube.equations[i][3] )/hlp;
			z_flag[i] = FALSE;
		} else {
			z_value[i] = clip_cube.equations[i][0]*ux +
				clip_cube.equations[i][1]*uy +
				clip_cube.equations[i][3];
			z_flag[i] = TRUE;
		}
	}

	*zmin = cb;
	*zmax = cf;

	for( i = 0; i < 6; i += 2 ) {
		j = i + 1;

		if( z_flag[i] == FALSE || z_flag[j] == FALSE ) {
			if( z_value[i] > z_value[j] ) {
				temp = z_value[i];
				z_value[i] = z_value[j];
				z_value[j] = temp;
			}

			*zmin = max( *zmin,z_value[i] );
			*zmax = min( *zmax,z_value[j] );
		} else {
			if( z_value[i]*z_value[j] > 0.0 ) {
				*zmin =  1.0;
				*zmax = -1.0;
			}
		}
	}

	return ( *zmin < *zmax );
}


// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------

static void surface_clip_user_xy_in_none_parallel_init (double)
{
}

static int surface_clip_user_xy_in_none_parallel( double, double,
						  double *zmin, double *zmax )
{
	*zmin = cb;
	*zmax = cf;

	return  TRUE;
}

static  void surface_clip_user_y_in_none_central_init( void )
{
}

static  int surface_clip_user_y_in_none_central( double )
{
	return  TRUE;
}

static  void surface_clip_user_xy_in_none_central_init( double )
{
}

static int surface_clip_user_xy_in_none_central( double,double,
						 double *zmin,double *zmax )
{
	*zmin = cb;
	*zmax = cf;

	return  TRUE;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------

static void (*surface_clip_user_y_in_parallel_init[CLIP_NUM_CHOICE]) (void) =
{
	surface_clip_user_y_in_sphere_parallel_init,
	surface_clip_user_y_in_sphere_parallel_init,
	surface_clip_user_y_in_cylvert_parallel_init,
	surface_clip_user_y_in_sphere_parallel_init,
	surface_clip_user_y_in_cube_parallel_init,
	surface_clip_user_y_in_cylvert_parallel_init
};

static int (*surface_clip_user_y_in_parallel[CLIP_NUM_CHOICE]) (double) =
{
	surface_clip_user_y_in_sphere_parallel,
	surface_clip_user_y_in_sphere_parallel,
	surface_clip_user_y_in_cylvert_parallel,
	surface_clip_user_y_in_sphere_parallel,
	surface_clip_user_y_in_cube_parallel,
	surface_clip_user_y_in_cylvert_parallel
};

static  void (*surface_clip_user_y_in_central_init[CLIP_NUM_CHOICE]) (void) =
{
	surface_clip_user_y_in_sphere_central_init,
	surface_clip_user_y_in_cylhoriz_central_init,
	surface_clip_user_y_in_cylvert_central_init,
	surface_clip_user_y_in_cylz_central_init,
	surface_clip_user_y_in_cube_central_init,
	surface_clip_user_y_in_none_central_init
};

static int (*surface_clip_user_y_in_central[CLIP_NUM_CHOICE]) (double) =
{
	surface_clip_user_y_in_sphere_central,
	surface_clip_user_y_in_cylhoriz_central,
	surface_clip_user_y_in_cylvert_central,
	surface_clip_user_y_in_cylz_central,
	surface_clip_user_y_in_cube_central,
	surface_clip_user_y_in_none_central
};

void (**surface_clip_user_y_in_init[2]) (void) =
{
	surface_clip_user_y_in_parallel_init,
	surface_clip_user_y_in_central_init
};

int (**surface_clip_user_y_in[2]) (double) =
{
	surface_clip_user_y_in_parallel,
	surface_clip_user_y_in_central
};

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------

static void (*surface_clip_user_xy_in_parallel_init[CLIP_NUM_CHOICE]) (double) =
{
	surface_clip_user_xy_in_sphere_parallel_init,
	surface_clip_user_xy_in_cylhoriz_parallel_init,
	surface_clip_user_xy_in_cylvert_parallel_init,
	surface_clip_user_xy_in_cylz_parallel_init,
	surface_clip_user_xy_in_cube_parallel_init,
	surface_clip_user_xy_in_none_parallel_init
};

static int (*surface_clip_user_xy_in_parallel[CLIP_NUM_CHOICE]) (double, double, double*, double* ) =
{
	surface_clip_user_xy_in_sphere_parallel,
	surface_clip_user_xy_in_cylhoriz_parallel,
	surface_clip_user_xy_in_cylvert_parallel,
	surface_clip_user_xy_in_cylz_parallel,
	surface_clip_user_xy_in_cube_parallel,
	surface_clip_user_xy_in_none_parallel
};

static void (*surface_clip_user_xy_in_central_init[CLIP_NUM_CHOICE]) (double) =
{
	surface_clip_user_xy_in_sphere_central_init,
	surface_clip_user_xy_in_cylhoriz_central_init,
	surface_clip_user_xy_in_cylvert_central_init,
	surface_clip_user_xy_in_cylz_central_init,
	surface_clip_user_xy_in_cube_central_init,
	surface_clip_user_xy_in_none_central_init
};              

static int (*surface_clip_user_xy_in_central[CLIP_NUM_CHOICE]) (double, double, double*, double* ) =
{
	surface_clip_user_xy_in_sphere_central,
	surface_clip_user_xy_in_cylhoriz_central,
	surface_clip_user_xy_in_cylvert_central,
	surface_clip_user_xy_in_cylz_central,
	surface_clip_user_xy_in_cube_central,
	surface_clip_user_xy_in_none_central
};

void    (**surface_clip_user_xy_in_init[2])( double ) =
{
	surface_clip_user_xy_in_parallel_init,
	surface_clip_user_xy_in_central_init
};

int     (**surface_clip_user_xy_in[2]) (double, double, double*, double* ) =
{
	surface_clip_user_xy_in_parallel,
	surface_clip_user_xy_in_central
};

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------

int     surface_user_xy_in( double ux,double uy,double *zmin,double *zmax,
                            int cut,int cut_modus,int position_perspective,
                            int clip_modus )
{
	if( surface_clip_user_xy_in[position_perspective][clip_modus]
	    ( ux,uy,zmin,zmax ) ) {
		if( cut ) {
			double  z = surface_cut_front[cut_modus]( ux,uy );

			if( z > *zmin && z < *zmax ) {
				*zmax = z;
				return  TRUE;
			} else {
				return  FALSE;
			}
		} else {
			return  TRUE;
		}
	} else {
		return  FALSE;
	}
}
