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



#include <stdio.h>
#include <stdlib.h>

#include "def.h"
#include "gui_config.h"



// -------------------------------------------
// Decides wether yacced commands are executed
// -------------------------------------------

int     surface_run_commands   = FALSE;
char    *surface_filename_data = 0;


// -------------------
// Surface polynomials
// -------------------

polyxyz main_formula_pxyz_data[MAIN_SURFACE_AMOUNT_NUM] = 
{
  {0,0,0},{0,0,0},{0,0,0},
  {0,0,0},{0,0,0},{0,0,0},
  {0,0,0},{0,0,0},{0,0,0}
};

// ------------------------
// Clip surface polynomials
// ------------------------

polyxyz main_clip_pxyz_data[MAIN_CLIP_AMOUNT_NUM] = 
{
  {0,0,0},{0,0,0},{0,0,0},
  {0,0,0},{0,0,0},{0,0,0},
  {0,0,0},{0,0,0},{0,0,0},
  {0,0,0},{0,0,0},{0,0,0}
};

// ----------------
// Width and height
// ----------------

int     main_width_data               = 200;
int     main_height_data              = 200;


// -------------
// Color sliders
// -------------

color_slider_t color_slider[MAIN_SURFACE_AMOUNT_NUM];


// ---------------- 
// Background color
// ---------------- 

int     color_background_data[COLOR_BACKGROUND_NUM];
int     color_do_background_data;


// --------
// Gradient
// --------

int     color_gradient_data;
int     color_gradient_end_data[COLOR_GRADIENT_END_NUM];


// -------------------
// Illumination choice
// -------------------

int     light_illumination_data;

int     light_illumination_ambient_data            = 1;
int     light_illumination_diffuse_data            = 2;
int     light_illumination_reflected_data          = 4;
int     light_illumination_transmitted_data        = 8;

// ----------------------------------------------------------------------------

// ---------------------
// Illumination settings
// ---------------------

// --------------
// First settings
// --------------



//sk :1'ten Befehle zur Oberflächen-Eigenschaftenangabe bis auf ...
//      ambient lassen sich alle E. für jede der 9 Flächen individiuell einstellen



int     LIGHT_SETTINGS_SECOND_MAX_VALUE[LIGHT_SETTINGS_SECOND_NUM] = 
{
	200, 99, 50,		
};

light_settings_t light_settings[MAIN_SURFACE_AMOUNT_NUM];

// ----------------------------------------------------------------------------

light_data_t light_data [LIGHT_SOURCE_MAX_VALUE];

// -----------------
// Dithering methods
// -----------------

int     print_dither_data;

int     print_dither_floyd_steinberg_data	= 0;
int     print_dither_jarvis_judis_ninke_data	= 1;
int     print_dither_stucki_data		= 2;
int     print_dither_ordered_dither_data	= 3;
int     print_dither_dispersed_dither_data	= 4;
int     print_dither_dot_diffusion_data		= 5;
int     print_dither_smooth_dot_diffusion_data	= 6;


// -----------------
// serpentine raster
// -----------------

int     print_serpentine_raster_data;

int     global_yes_data         = 0;
int     global_no_data          = 1;


// --------------
// random weights
// --------------

int     print_random_weights_data;
double  print_weight_data;


// ------
// barons
// ------

int     print_barons_data;
int     print_barons_one_baron_data         = 0;
int     print_barons_two_baron_data         = 1;


// ------------
// pattern size
// ------------

int     print_pattern_size_data;

int     print_pattern_size_4_x_4_data           = 0;
int     print_pattern_size_8_x_8_data           = 1;
int     print_pattern_size_16_x_16_data         = 2;


// ----------------------------------------------------------------------------

// -------------
// Enhance edges
// -------------


int     print_enhance_data;
double  print_alpha_data;


// ----------------------------------------------------------------------------

// ----------
// Background
// ----------

double  print_background_data;


// ----------------------------------------------------------------------------

// ---------------------
// Tone scale Adjustment
// ---------------------

int     print_tone_data;


// ----------------------------------------------------------------------------

// -----
// Gamma
// -----

double  print_gamma_data;
int     print_gamma_correction_data;


// ----------------------------------------------------------------------------

// ----------
// Resolution
// ----------

int     print_resolution_data;

int     print_resolution_array_data[6] =
{
	75, 100, 150, 300, 600, 1200
};

int     print_resolution_75dpi_data	= 0;
int     print_resolution_100dpi_data	= 1;
int     print_resolution_150dpi_data	= 2;
int     print_resolution_300dpi_data	= 3;
int     print_resolution_600dpi_data	= 4;
int     print_resolution_1200dpi_data	= 5;


// ----------------------------------------------------------------------------

// ------------
// Pixel radius
// ------------

int     print_p_radius_data;


// ----------------------------------------------------------------------------

// ----------------------------------
// Spectator and rotation and scaling
// ----------------------------------

position_numeric_t position_numeric;


// -----------
// perspective
// -----------

int     position_perspective_data;

int     position_perspective_parallel_data          = 0;
int     position_perspective_central_data           = 1;


// --------------------------------------
// rotate about object axis or world axis
// --------------------------------------

int     position_object_axis_data          = 0;
int     position_world_axis_data           = 1;


// -------------------
// sequence of actions
// -------------------

int     position_sequence_data[POSITION_SEQUENCE_NUM];

int     position_sequence_translate_data        = 0;
int     position_sequence_rotate_data           = 1;
int     position_sequence_scale_data            = 2;

 
// ------------------------------------------------
// The following data defines the appearance of the
// display menu.
// ------------------------------------------------

// -----------------
// Display normalize
// -----------------

int     display_normalize_data;


// --------------
// Optimize Color
// --------------

int     display_optimize_data;


// --------------
// Display Dither
// --------------

int     display_color_dither_data;


// -------------------
// Display Dither value
// --------------------

double  display_dither_value_data;


// ---------------------
// Display max intensity
// ---------------------

double  display_max_i_data;


// ----------------
// Refinement level
// ----------------

int     display_ref_level_data;


// --------------------
// Refinement threshold
// --------------------

double  display_ref_treshold_data;


// -----------------
// Refinement radius
// -----------------

double  display_ref_radius_data;


// ----------------------------------------------------------------------------

// ------------
// Depth cueing
// ------------

int     display_depth_cue_data;
double  display_depth_value_data;

// ----------
// 3D-Display
// ----------

display_numeric_t display_numeric;


// ----------------------------------------------------------------------------

// ------------------
// Choice root finder
// ------------------

int     numeric_root_finder_data;

int      numeric_root_d_bisection_data       = NUMERIC_ROOT_D_BISECTION;
int      numeric_root_d_regula_data          = NUMERIC_ROOT_D_REGULA;
int      numeric_root_d_pegasus_data         = NUMERIC_ROOT_D_PEGASUS;
int      numeric_root_d_anderson_data        = NUMERIC_ROOT_D_ANDERSON;
int      numeric_root_d_newton_data          = NUMERIC_ROOT_D_NEWTON;
int      numeric_root_bezier_data            = NUMERIC_ROOT_BEZIER;


// ----------------------------------------------------------------------------

// ---------------------------
// Epsilon and max. iterations
// ---------------------------

double  numeric_epsilon_data;
int     numeric_iterations_data;


// ----------------------------------------------------------------------------
// The following data defines most of the appearance of the
// clip configuration menu.
// sk : Clip-Menü in clip_numeric_data im DepthCueing-Wert erweitert 
// ----------------------------------------------------------------------------


int     clip_data;


int     clip_sphere_data            = 0;
int     clip_tetrahedron_data       = 1;
int     clip_cube_data              = 2;
int     clip_octahedron_data        = 3;
int     clip_dodecahedron_data      = 4;
int     clip_icosahedron_data       = 5;
int     clip_cylinder_data          = 6;
int     clip_user_data              = 7;
int     clip_none_data              = 8;


//  double  clip_numeric_data[CLIP_NUMERIC_NUM];
clip_numeric_t clip_numeric;

// ----------------------------------------------------------------------------
// The following data defines most of the appearance of the
// cut configuration menu.
// ----------------------------------------------------------------------------


int     cut_input_data;

int     cut_input_keyboard_data     = 0;
int     cut_input_mouse_data        = 1;



int     cut_modus_data;


int     cut_modus_one_point_data        = 0;
int     cut_modus_three_point_data      = 1;


cut_numeric_t cut_numeric;






// ----------------------------------------------------------------------------
//  The following data defines most of the appearance of the
//  curves configuration menu.
// ----------------------------------------------------------------------------

double  curve_width_data;
double  curve_gamma_data;


// ---------------------
//  Curve color sliders 
// ---------------------

int     curve_color_slider_data[CURVE_COLOR_SLIDER_NUM];

 
// ----------------
//  surface number
// ----------------

int     curve_surface_nr_data;
