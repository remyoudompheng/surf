/*
 *   surf - visualizing algebraic curves and algebraic surfaces
 *   Copyright (C) 1996-1997 Friedrich-Alexander-Universitaet
 *                           Erlangen-Nuernberg
 *                 1997-2001 Johannes Gutenberg-Universitaet Mainz
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

#include <compfn.h>
#include <Misc.h>

namespace {

//  Dispersed dot ordered dither patterns:

int dispersed_dither_4_x_4_data[4][4] = {
	{   1,  9,  3, 11 },
	{  13,  5, 15,  7 },
	{   4, 12,  2, 10 },
	{  16,  8, 14,  6 }
};

int dispersed_dither_8_x_8_data[8][8] = {
	{   1, 33,  9, 41,  3, 35, 11, 43 },
	{  49, 17, 57, 25, 51, 19, 59, 27 },
	{  13, 45,  5, 37, 15, 47,  7, 39 },
	{  61, 29, 53, 21, 63, 31, 55, 23 },
	{   4, 36, 12, 44,  2, 34, 10, 42 },
	{  52, 20, 60, 28, 50, 18, 58, 26 },
	{  16, 48,  8, 40, 14, 46,  6, 38 },
	{  64, 32, 56, 24, 62, 30, 54, 22 }
};

int dispersed_dither_16_x_16_data[16][16] = {
	{   1,129, 33,161,  9,137, 41,169,  3,131, 35,163, 11,139, 43,171 },
	{ 193, 65,225, 97,201, 73,233,105,195, 67,227, 99,203, 75,235,107 },
	{  49,177, 17,145, 57,185, 25,153, 51,179, 19,147, 59,187, 27,155 },
	{ 241,113,209, 81,249,121,217, 89,243,115,211, 83,251,123,219, 91 },
	{  13,141, 45,173,  5,133, 37,165, 15,143, 47,175,  7,135, 39,167 },
	{ 205, 77,237,109,197, 69,229,101,207, 79,239,111,199, 71,231,103 },
	{  61,189, 29,157, 53,181, 21,149, 63,191, 31,159, 55,183, 23,151 },
	{ 253,125,221, 93,245,117,213, 85,255,127,223, 95,247,119,215, 87 },
	{   4,132, 36,164, 12,140, 44,172,  2,130, 34,162, 10,138, 42,170 },
	{ 196, 68,228,100,204, 76,236,108,194, 66,226, 98,202, 74,234,106 },
	{  52,180, 20,148, 60,188, 28,156, 50,178, 18,146, 58,186, 26,154 },
	{ 244,116,212, 84,252,124,220, 92,242,114,210, 82,250,122,218, 90 },
	{  16,144, 48,176,  8,136, 40,168, 14,142, 46,174,  6,134, 38,166 },
	{ 208, 80,240,112,200, 72,232,104,206, 78,238,110,198, 70,230,102 },
	{  64,192, 32,160, 56,184, 24,152, 62,190, 30,158, 54,182, 22,150 },
	{ 256,128,224, 96,248,120,216, 88,254,126,222, 94,246,118,214, 86 }
};

float dispersed_dither_4_x_4_compare  [ 4][ 4];
float dispersed_dither_8_x_8_compare  [ 8][ 8];
float dispersed_dither_16_x_16_compare[16][16];

//  Clustered dot ordered dither pattern:

int clustered_dither_4_x_4_data[4][4] = {
	{   9, 11,  4,  2 },
	{  13, 15,  8,  6 },
	{   3,  1, 10, 12 },
	{   7, 5 , 14, 16 }
};

int clustered_dither_8_x_8_data[8][8] = {
	{  33, 41, 43, 35, 28, 12, 10, 26 },
	{  55, 57, 59, 45, 14,  4,  2, 24 },
	{  53, 61, 63, 47, 16,  6,  8, 22 },
	{  39, 51, 49, 37, 30, 18, 20, 32 },
	{  27, 11,  9, 25, 34, 42, 44, 36 },
	{  13,  3,  1, 23, 56, 58, 60, 46 },
	{  15,  5,  7, 21, 54, 62, 64, 48 },
	{  29, 17, 19, 31, 40, 52, 50, 38 }
};

int clustered_dither_16_x_16_data[16][16] = {
	{ 129,139,157,185,187,159,141,131,124,110, 88, 56, 54, 86,108,122 },
	{ 137,155,183,215,217,189,161,143,112, 90, 58, 30, 28, 52, 84,106 },
	{ 153,181,213,237,239,219,191,163, 92, 60, 32, 12, 10, 26, 50, 82 },
	{ 179,211,235,249,251,241,221,193, 62, 34, 14,  4,  2, 24, 48, 80 },
	{ 177,209,233,253,255,243,223,195, 64, 36, 16,  6,  8, 22, 46, 78 },
	{ 175,207,231,247,245,225,197,165, 94, 66, 38, 18, 20, 44, 76,104 },
	{ 151,173,205,229,227,199,167,145,114, 96, 68, 40, 42, 74,102,120 },
	{ 135,149,171,203,201,169,147,133,126,116, 98, 70, 72,100,118,128 },
	{ 123,109, 87, 55, 53, 85,107,121,130,140,158,186,188,160,142,132 },
	{ 111, 89, 57, 29, 27, 51, 83,105,138,156,184,216,218,190,162,144 },
	{  91, 59, 31, 11,  9, 25, 49, 81,154,182,214,238,240,220,192,164 },
	{  61, 33, 13,  3,  1, 23, 47, 79,180,212,236,250,252,242,222,194 },
	{  63, 35, 15,  5,  7, 21, 45, 77,178,210,234,254,256,244,224,196 },
	{  93, 65, 37, 17, 19, 43, 75,103,176,208,232,248,246,226,198,166 },
	{ 113, 95, 67, 39, 41, 73,101,119,152,174,206,230,228,200,168,146 },
	{ 125,115, 97, 69, 71, 99,117,127,136,150,172,204,202,170,148,134 }
};

float clustered_dither_4_x_4_compare  [ 4][ 4];
float clustered_dither_8_x_8_compare  [ 8][ 8];
float clustered_dither_16_x_16_compare[16][16];

//  Matrix for dot-diffusion:

//  Matrix with one baron:
int d_d_1_b_m[8][8] = {
	{ 25, 21, 13, 39, 47, 57, 53, 45 },
	{ 48, 32, 29, 43, 55, 63, 61, 56 },
	{ 40, 30, 35, 51, 59, 62, 60, 52 },
	{ 36, 14, 22, 26, 46, 54, 58, 44 },
	{ 16,  6, 10, 18, 38, 42, 50, 24 },
	{  8,  0,  2,  7, 15, 31, 34, 20 },
	{  4,  1,  3, 11, 23, 33, 28, 12 },
	{ 17,  9,  5, 19, 27, 49, 41, 37 }
};

//  Matrix with two barons:
int d_d_2_b_m[8][8] = {
	{ 34, 48, 40, 32, 29, 15, 23, 31 },
	{ 42, 58, 56, 53, 21,  5,  7, 10 },
	{ 50, 62, 61, 45, 13,  1,  2, 18 },
	{ 38, 46, 54, 37, 25, 17,  9, 26 },
	{ 28, 14, 22, 30, 35, 49, 41, 33 },
	{ 20,  4,  6, 11, 43, 59, 57, 52 },
	{ 12,  0,  3, 19, 51, 63, 60, 44 },
	{ 24, 16,  8, 27, 39, 47, 55, 36 }
};

int (*d_d_m[2])[8][8] = {
	&d_d_1_b_m,
	&d_d_2_b_m
};

} // end of anon namespace

// ----------------------------------------------------------------------------

//  Neighbour offsets x-coordinate:

int d_d_n_x[8] = {
    -1, 0, 1,
    -1,    1,
    -1, 0, 1
};

//  Neighbour offsets y-coordinate:

int d_d_n_y[8] = {
    -1,-1,-1,
     0,    0,
     1, 1, 1
};

//  Corresponding error weights:

int d_d_w[8] = {
     1, 2, 1,
     2,    2,
     1, 2, 1
};

//  Nearest neighbour offsets x-coordinate:

int d_d_n_n_x[9] = {
    -1, 0, 1,
    -1, 0, 1,
    -1, 0, 1
};

//  Nearest neighbour offsets y-coordinate:

int d_d_n_n_y[9] = {
    -1,-1,-1,
     0, 0, 0,
     1, 1, 1
};

//  Information about dot diffusion:

namespace {
dot_diffusion_struct d_d_1_b_s[64];
dot_diffusion_struct d_d_2_b_s[64];
}

typedef dot_diffusion_struct dot_diffusion_struct_arr[64];
typedef dot_diffusion_struct_arr* dot_diffusion_struct_arr_ptr;

dot_diffusion_struct_arr_ptr d_d_s[2] = {
	&d_d_1_b_s,
	&d_d_2_b_s
};

//  Information about smooth dot diffusion:

namespace {
smooth_dot_diffusion_struct s_d_d_1_b_s[64];
smooth_dot_diffusion_struct s_d_d_2_b_s[64];
}

typedef smooth_dot_diffusion_struct smooth_dot_diffusion_struct_arr[64];
typedef smooth_dot_diffusion_struct_arr* smooth_dot_diffusion_struct_arr_ptr;

smooth_dot_diffusion_struct_arr_ptr s_d_d_s[2] = {
	&s_d_d_1_b_s,
	&s_d_d_2_b_s,
};

//  Get the entry of a dot diffusion matrix:

int dot_diffusion_matrix(int i, int px, int py)
{
	px = (px < 0 ? 8 - (-px)%8 : px%8);
	py = (py < 0 ? 8 - (-py)%8 : py%8);

	return  (*d_d_m[i])[py][px];
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------

void nearest_neighbour_of_class(int i, int x, int y, int& rx,int& ry, int k)
{
	int x_d_min = 0;
	int y_d_min = 0;
	int dist_min = 1000;

	for(int j = 0; j < 9; j++ ) {
		int x_d  = (*d_d_s[i])[k].x + 8*d_d_n_n_x[j] - x;
		int y_d  = (*d_d_s[i])[k].y + 8*d_d_n_n_y[j] - y;
		int dist = x_d*x_d + y_d*y_d;

		if(dist < dist_min) {
			dist_min = dist;
			x_d_min = x_d;
			y_d_min = y_d;
		}
	}

	rx = x_d_min;
	ry = y_d_min;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------

namespace {

void dot_diffusion_init_struct()
{
	for(int i = 0; i < 2; i++) {
		for(int k = 0; k < 64; k++) {

			//  look for number  k  in matrix:

			bool found = false;
			int x = -1;
			int y = 0;
			while(x < 7 && !found) {
				x++;
				y = -1;
				
				while(y < 7 && !found) {
					y++;
					found = ((*d_d_m[i])[y][x] == k);
				}
			}
			
			if(!found) {
				Misc::print_error("Something weird happend while initializing dot diffusion! This is a bug.\n");
			}
			
			//  Store position of  k  in struct:
			
			(*d_d_s[i])[k].x = x;
			(*d_d_s[i])[k].y = y;
			
			//  Calculate error distribution:
			
			int num_err = 0;
			
			for(int j = 0; j < 8; j++ ) {
				if(dot_diffusion_matrix( i,x + d_d_n_x[j],
							 y + d_d_n_y[j] ) > k) {
					(*d_d_s[i])[k].err[j] = double(d_d_w[j]);
					num_err += d_d_w[j];
				} else {
					(*d_d_s[i])[k].err[j] = 0.0;
				}
			}

			if(num_err > 0) {
				for(int j = 0; j < 8; j++) {
					(*d_d_s[i])[k].err[j] /= num_err;
				}
			}
		}
	}
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------

void smooth_dot_diffusion_init_struct()
{
	// -----------------------------------------------
	//  Assume graceously that dot_diffusion_init has
	//  been called before
	// -----------------------------------------------
	
	for(int i = 0; i < 2; i++) {
		for(int k = 0; k <= 63; k++) {
			(*s_d_d_s[i])[k].x = (*d_d_s[i])[k].x;
			(*s_d_d_s[i])[k].y = (*d_d_s[i])[k].y;
			(*s_d_d_s[i])[k].n = 63 - k;
    
			if(k < 63) {
				(*s_d_d_s[i])[k].err_x = new int[(*s_d_d_s[i])[k].n];
				(*s_d_d_s[i])[k].err_y = new int[(*s_d_d_s[i])[k].n];
				(*s_d_d_s[i])[k].err = new double[(*s_d_d_s[i])[k].n];
				
				for(int j = 0, l = k + 1; j < (*s_d_d_s[i])[k].n; j++, l++) {
					nearest_neighbour_of_class(i,
								   (*s_d_d_s[i])[k].x,
								   (*s_d_d_s[i])[k].y,
								   (*s_d_d_s[i])[k].err_x[j],
								   (*s_d_d_s[i])[k].err_y[j],
								   l); 
					(*s_d_d_s[i])[k].err[j] = 1.0/(63 - k);
				}
			} else {
				(*s_d_d_s[i])[k].err_x = 0;
				(*s_d_d_s[i])[k].err_y = 0;
				(*s_d_d_s[i])[k].err = 0;
			}
		}
	}
}

} // anon namespace


void deinit_dot_diffusion()
{
	for(size_t i = 0; i < 2; i++ ) {
		for(size_t k = 0; k <= 63; k++ ) {
			(*s_d_d_s[i])[k].x = (*d_d_s[i])[k].x;
			(*s_d_d_s[i])[k].y = (*d_d_s[i])[k].y;
			(*s_d_d_s[i])[k].n = 63 - k;
    
			if( k < 63 ) {
				delete [] (*s_d_d_s[i])[k].err_x;
				delete [] (*s_d_d_s[i])[k].err_y;
				delete [] (*s_d_d_s[i])[k].err;
			}
		}
	}
}
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------

void    init_dot_diffusion( void )
{
    dot_diffusion_init_struct( );
    smooth_dot_diffusion_init_struct( );
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------

void    init_pattern( void )
{
    int     i;
    int     j;

    for( i = 0; i < 4; i++ )
    {
        for( j = 0; j < 4; j++ )
        {
            clustered_dither_4_x_4_compare[i][j] =
                    (float)( 2*clustered_dither_4_x_4_data[i][j] - 1)/32;
            dispersed_dither_4_x_4_compare[i][j] =
                    (float)( 2*dispersed_dither_4_x_4_data[i][j] - 1)/32;
        }
    }

    for( i = 0; i < 8; i++ )
    {
        for( j = 0; j < 8; j++ )
        {
            clustered_dither_8_x_8_compare[i][j] =
                    (float)( 2*clustered_dither_8_x_8_data[i][j] - 1)/128;
            dispersed_dither_8_x_8_compare[i][j] =
                    (float)( 2*dispersed_dither_8_x_8_data[i][j] - 1)/128;
        }
    }

    for( i = 0; i < 16; i++ )
    {
        for( j = 0; j < 16; j++ )
        {
            clustered_dither_16_x_16_compare[i][j] =
                    (float)( 2*clustered_dither_16_x_16_data[i][j] - 1)/512;
            dispersed_dither_16_x_16_compare[i][j] =
                    (float)( 2*dispersed_dither_16_x_16_data[i][j] - 1)/512;
        }
    }
}

// ----------------------------------------------------------------------------
// Compare functions
// ----------------------------------------------------------------------------

static  float   compare_clustered_dither_4_x_4( int x,int y )
{
    return  clustered_dither_4_x_4_compare[y%4][x%4];
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------

static  float   compare_clustered_dither_8_x_8( int x,int y )
{
    return  clustered_dither_8_x_8_compare[y%8][x%8];
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------

static  float   compare_clustered_dither_16_x_16( int x,int y )
{
    return  clustered_dither_16_x_16_compare[y%16][x%16];
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------

static  float   compare_dispersed_dither_4_x_4( int x,int y )
{
    return  dispersed_dither_4_x_4_compare[y%4][x%4];
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------

static  float   compare_dispersed_dither_8_x_8( int x,int y )
{
    return  dispersed_dither_8_x_8_compare[y%8][x%8];
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------

static  float   compare_dispersed_dither_16_x_16( int x,int y )
{
    return  dispersed_dither_16_x_16_compare[y%16][x%16];
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------

float   (*compare_function[2][3])( int,int ) =
{
	{
		compare_clustered_dither_4_x_4,
		compare_clustered_dither_8_x_8,
		compare_clustered_dither_16_x_16
	},
	{
		compare_dispersed_dither_4_x_4,
		compare_dispersed_dither_8_x_8,
		compare_dispersed_dither_16_x_16
	}
};





// ============================================================================
// End of compfn.cc
// ============================================================================
