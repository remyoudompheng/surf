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

#ifndef SCRIPTVAR_H
#define SCRIPTVAR_H

#include <stdio.h>

#include <def.h>

#include <polyarith.h>
#include <degree.h>
#include <hornerarith.h>
#include <polylexyacc.h>
#include <polyyacc.h>

// sk :neuer enum für RED,GREEN,BLUE
typedef enum
{
	RED,
	GREEN,
	BLUE
    
} color_components_enum;

typedef enum
{
	SURFS,
	INSIDE,
	BALLS
} surfball_enum;


typedef enum 
{ 
	LIGHT_SMOOTHNESS, 
	LIGHT_TRANSPARENCE, 
	LIGHT_THICKNESS 		/* sk */

} light_settings_second_enum; 

typedef enum 
{ 
	LIGHT_POSITION_X, 
	LIGHT_POSITION_Y, 
	LIGHT_POSITION_Z 
 
} light_position_enum; 

typedef enum
{
	NUMERIC_ROOT_D_BISECTION,
	NUMERIC_ROOT_D_REGULA,
	NUMERIC_ROOT_D_PEGASUS,
	NUMERIC_ROOT_D_ANDERSON,
	NUMERIC_ROOT_D_NEWTON,
	NUMERIC_ROOT_BEZIER
} NUMERIC_ROOT_ENUM;    

// Don't declare the structs in namespace ScriptVar...
struct color_slider_t
{
	int red;
	int green;
	int blue;
	int inside_red;
	int inside_green;
	int inside_blue;
};

struct light_settings_t
{
	int ambient;
	int diffuse;
	int reflected;
	int transmitted;
	
	int smoothness;
	int transparence;
	int thickness;
};

struct light_data_t 
{
	int volume;
	int color[3];
	double position[3];
	
	double getColorValue (int c)
		{ return volume*color[c] / 25500.0;}
};

struct position_numeric_t
{
	double spectator_z;
	
	double rot_x;
	double rot_y;
	double rot_z;
	
	double scale_x;
	double scale_y;
	double scale_z;
	
	double orig_x;
	double orig_y;
	double orig_z;
};

struct display_numeric_t 
{
	double stereo_eye;
	double stereo_z;
	double stereo_red;
	double stereo_green;
	double stereo_blue;
};

struct clip_numeric_t
{
	double clip_front;
	double clip_back;
	double radius;
	double center_x;
	double center_y;
	double center_z;
};
	
// used in polyyacc.y...can't move this to namespace ScriptVar
extern int surface_run_commands; // Decides wether yacced commands are executed


namespace ScriptVar 
{

extern  char    *surface_filename_data;


// ---------------
//  Formula input
// ---------------

extern polyxyz draw_func_function_data;
extern polyxyz draw_func_plane_data;

extern  polyxyz main_formula_pxyz_data[MAIN_SURFACE_AMOUNT_NUM];
extern  polyxyz main_clip_pxyz_data[MAIN_CLIP_AMOUNT_NUM];


// ------------------
//  Width and height
// ------------------

extern  int     main_width_data;
extern  int     main_height_data;


extern color_slider_t color_slider[MAIN_SURFACE_AMOUNT_NUM];


// ----------------
// Background color
// ----------------

extern int     color_background_data[COLOR_BACKGROUND_NUM];
extern int     color_do_background_data;


// --------
// Gradient
// --------

extern int     color_gradient_data;
extern int     color_gradient_end_data[COLOR_GRADIENT_END_NUM];


// -------------------
// Illumination choice
// -------------------

extern  int     light_illumination_data;

extern  int     light_illumination_ambient_data;
extern  int     light_illumination_diffuse_data;
extern  int     light_illumination_reflected_data;
extern  int     light_illumination_transmitted_data;


// ---------------------
// Illumination settings
// ---------------------

extern  int     LIGHT_SETTINGS_SECOND_MAX_VALUE [LIGHT_SETTINGS_SECOND_NUM];


extern light_settings_t light_settings[MAIN_SURFACE_AMOUNT_NUM];


extern light_data_t light_data [LIGHT_SOURCE_MAX_VALUE];



// -----------------
// Dithering methods
// -----------------

extern  int     print_dither_data;

extern  int     print_dither_floyd_steinberg_data;
extern  int     print_dither_jarvis_judis_ninke_data;
extern  int     print_dither_stucki_data;
extern  int     print_dither_ordered_dither_data;
extern  int     print_dither_dispersed_dither_data;
extern  int     print_dither_dot_diffusion_data;
extern  int     print_dither_smooth_dot_diffusion_data;


// -------------------
// Serpentine raster
// -------------------

extern  int     print_serpentine_raster_data;
extern  int     global_yes_data;
extern  int     global_no_data;


// --------------
// Random weights
// --------------

extern  int     print_random_weights_data;
extern  double  print_weight_data;


// --------
//  Barons
// --------

extern  int     print_barons_data;

extern  int     print_barons_one_baron_data;
extern  int     print_barons_two_baron_data;


// --------------
//  Pattern size
// --------------

extern  int     print_pattern_size_data;
extern  int     print_pattern_size_4_x_4_data;
extern  int     print_pattern_size_8_x_8_data;
extern  int     print_pattern_size_16_x_16_data;


// ---------------
//  Enhance edges
// ---------------

extern  int     print_enhance_data;
extern  double  print_alpha_data;


// ------------
//  Background
// ------------

extern  double  print_background_data;


// -----------------------
//  Tone scale adjustment
// -----------------------

extern  int     print_tone_data;


// -------
//  Gamma
// -------

extern  double  print_gamma_data;
extern  int     print_gamma_correction_data;


// ------------
//  Resolution
// ------------

extern  int     print_resolution_data;

extern  int     print_resolution_array_data[6];

extern  int     print_resolution_75dpi_data;
extern  int     print_resolution_100dpi_data;
extern  int     print_resolution_150dpi_data;
extern  int     print_resolution_300dpi_data;
extern  int     print_resolution_600dpi_data;
extern  int     print_resolution_1200dpi_data;


// --------------
//  Pixel radius
// --------------

extern  int     print_p_radius_data;


// ----------------------------------
// Spectator and rotation and scaling
// ----------------------------------


extern position_numeric_t position_numeric;
 
// -------------
//  Perspective
// -------------

extern  int     position_perspective_data;

extern  int     position_perspective_parallel_data;
extern  int     position_perspective_central_data;
 

// ----------------------------------------
//  Rotate about object axis or world axis
// ----------------------------------------

extern  int     position_object_world_data;
extern  int     position_object_axis_data;
extern  int     position_world_axis_data;


// -------------------
// sequence of actions
// -------------------

extern  int     position_sequence_data[POSITION_SEQUENCE_NUM];

extern  int     position_sequence_translate_data;
extern  int     position_sequence_rotate_data;
extern  int     position_sequence_scale_data;


// -----------------
// display normalize
// -----------------

extern  int     display_normalize_data;

// --------------
// Optimize Color
// --------------

extern  int     display_optimize_data;


// --------------
// display dither
// --------------

extern  int     display_color_dither_data;


// --------------------
// display dither value
// --------------------

extern  double  display_dither_value_data;


// ---------------------
// display max intensity
// ---------------------

extern  double  display_max_i_data;


// ------------------
//  Refinement level
// ------------------

extern  int     display_ref_level_data;


// ---------------------
//  Refinement treshold
// ---------------------

extern  double  display_ref_treshold_data;


// ---------------------
// sk :Refinement radius
// ---------------------

extern  double  display_ref_radius_data;


// --------------
//  depth cueing
// --------------

extern  int     display_depth_cue_data;
extern  double  display_depth_value_data;


// ----------
// 3D-Display
// ----------


extern display_numeric_t display_numeric;


// --------------------
//  Choice root finder
// --------------------

extern  int     numeric_root_finder_data;
extern  int      numeric_root_d_bisection_data;
extern  int      numeric_root_d_regula_data;
extern  int      numeric_root_d_pegasus_data;
extern  int      numeric_root_d_anderson_data;
extern  int      numeric_root_d_newton_data;
extern  int      numeric_root_bezier_data;


// -----------------------------
//  Epsilon and max. iterations
// -----------------------------

extern  double  numeric_epsilon_data;
extern  int     numeric_iterations_data;


extern  int     clip_data;

extern  int     clip_sphere_data;
extern  int     clip_tetrahedron_data;
extern  int     clip_cube_data;
extern  int     clip_octahedron_data;
extern  int     clip_dodecahedron_data;
extern  int     clip_icosahedron_data;
extern  int     clip_cylinder_data;
extern  int     clip_user_data;
extern  int     clip_none_data;

extern clip_numeric_t clip_numeric;


extern  int     cut_input_data;
extern  int     cut_input_keyboard_data;
extern  int     cut_input_mouse_data;
extern  int     cut_modus_data;
extern  int     cut_modus_one_point_data;
extern  int     cut_modus_three_point_data;

struct cut_numeric_t
{
	double upper_left_x, upper_left_y;
	double lower_right_x, lower_right_y;
	double z_value;
	double point_1_x, point_1_y, point_1_z;
	double point_2_x, point_2_y, point_2_z;
	double point_3_x, point_3_y, point_3_z;

};

extern cut_numeric_t cut_numeric;



// --------
//  Curves
// --------

extern  double  curve_width_data;
extern  double  curve_gamma_data;

// -----------
// Curve Color
// -----------

extern  int     curve_color_slider_data[CURVE_COLOR_SLIDER_NUM];   


// -------------
// surfacenumber
// -------------

extern int     curve_surface_nr_data;



// 
// --- "cutsurface"
//

extern polyxyz sym_cutsurfaces[9];

//
// --- "color_file_format" color_ouput_data
//

extern  int     color_file_format_data;

//
// --- "color_file_colormap" colormap_output_data
//

extern  int     colormap_output_data;

extern  const int     colormap_output_netscape_data;	// "Netscape"
extern  const int     colormap_output_optimized_data;	// "Optimized"
extern  const int     colormap_output_true_color_data;	// "TrueColor"


//
//  --- "dither_file_format"
//
extern  int     dither_file_format_data;

// --------------------
// Image Format options
// --------------------

extern int jpeg_quality; 
}

#endif  //!SCRIPTVAR_H
