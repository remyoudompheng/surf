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





// ===========================================================================
// File:     init_parser.cc
// Author:   Stephan Endrass
// Address:  endrass@mi.uni-erlangen.de
// Date:     3.2.95
// ============================================================================

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "init_parser.h"

#include "def.h"
#include "monomarith.h"
#include "polyarith.h"
#include "hornerarith.h"
#include "polylexyacc.h"
#include "gui_config.h"
#include "symtab.h"


using namespace ScriptVar;

double Y_AXIS_LR_ROTATE;
double sym_cut_distance;

const char    *POSITION_SEQUENCE_PIC_STRING[POSITION_SEQUENCE_NUM] =
{
    "first",
    "second",
    "third"
};

const char *LIGHT_VOLUME_PIC_STRING[LIGHT_SOURCE_MAX_VALUE] =
{
    "light1_vol",
    "light2_vol",
    "light3_vol",
    "light4_vol",
    "light5_vol",
    "light6_vol",
    "light7_vol",
    "light8_vol",
    "light9_vol"
};


const char  *LIGHT_POSITION_PIC_STRING
                           [LIGHT_SOURCE_MAX_VALUE][LIGHT_POSITION_NUM] =
{
    { "light1_x", "light1_y", "light1_z" },
    { "light2_x", "light2_y", "light2_z" },
    { "light3_x", "light3_y", "light3_z" },
    { "light4_x", "light4_y", "light4_z" },
    { "light5_x", "light5_y", "light5_z" },
    { "light6_x", "light6_y", "light6_z" },
    { "light7_x", "light7_y", "light7_z" },
    { "light8_x", "light8_y", "light8_z" },
    { "light9_x", "light9_y", "light9_z" }
};


const char *LIGHT_SETTINGS_SECOND_PIC_STRING[MAIN_SURFACE_AMOUNT_NUM] [LIGHT_SETTINGS_SECOND_NUM] =
{
    { "smoothness",  "transparence",  "thickness"  },
    { "smoothness2", "transparence2", "thickness2" },
    { "smoothness3", "transparence3", "thickness3" },
    { "smoothness4", "transparence4", "thickness4" },
    { "smoothness5", "transparence5", "thickness5" },
    { "smoothness6", "transparence6", "thickness6" },
    { "smoothness7", "transparence7", "thickness7" },
    { "smoothness8", "transparence8", "thickness8" },
    { "smoothness9", "transparence9", "thickness9" }
};

const char *LIGHT_SETTINGS_FIRST_PIC_STRING[MAIN_SURFACE_AMOUNT_NUM][LIGHT_SETTINGS_FIRST_NUM] =
{
    { "ambient",  "diffuse",  "reflected",  "transmitted"  },
    { "ambient2", "diffuse2", "reflected2", "transmitted2" },
    { "ambient3", "diffuse3", "reflected3", "transmitted3" },
    { "ambient4", "diffuse4", "reflected4", "transmitted4" },
    { "ambient5", "diffuse5", "reflected5", "transmitted5" },
    { "ambient6", "diffuse6", "reflected6", "transmitted6" },
    { "ambient7", "diffuse7", "reflected7", "transmitted7" },
    { "ambient8", "diffuse8", "reflected8", "transmitted8" },
    { "ambient9", "diffuse9", "reflected9", "transmitted9" }
};

const char *COLOR_GRADIENT_END_PIC_STRING[COLOR_GRADIENT_END_NUM] =
{
    "gradient_red",
    "gradient_green",
    "gradient_blue"
};


const char *COLOR_BACKGROUND_PIC_STRING[COLOR_BACKGROUND_NUM] =
{ 
    "background_red",
    "background_green",
    "background_blue"
};


const char *LIGHT_SLIDER_PIC_STRING[LIGHT_SOURCE_MAX_VALUE][LIGHT_SLIDER_NUM] =
{
    { "light1_red", "light1_green", "light1_blue" },
    { "light2_red", "light2_green", "light2_blue" },
    { "light3_red", "light3_green", "light3_blue" },
    { "light4_red", "light4_green", "light4_blue" },
    { "light5_red", "light5_green", "light5_blue" },
    { "light6_red", "light6_green", "light6_blue" },
    { "light7_red", "light7_green", "light7_blue" },
    { "light8_red", "light8_green", "light8_blue" },
    { "light9_red", "light9_green", "light9_blue" }      
};

const char *CURVE_COLOR_SLIDER_PIC_STRING[CURVE_COLOR_SLIDER_NUM] =
{
    "curve_red"  ,
    "curve_green",
    "curve_blue"
};

static const char *MAIN_FORMULA_PXYZ_PIC_STRING[MAIN_SURFACE_AMOUNT_NUM] =
{
    "surface",
    "surface2",
    "surface3",
    "surface4",
    "surface5",
    "surface6",
    "surface7",
    "surface8",
    "surface9"
};	

static const char *MAIN_CLIP_PXYZ_PIC_STRING[MAIN_CLIP_AMOUNT_NUM] =
{
    "clip_surface" ,
    "clip_surface2",
    "clip_surface3",
    "clip_surface4",
    "clip_surface5",
    "clip_surface6",
    "clip_surface7",
    "clip_surface8",
    "clip_surface9",
    "clip_surface10",
    "clip_surface11",
    "clip_surface12"
};	

const char *getColorSliderPicString (int surfaceNr, bool inside, int color)
{
	static char str[64];
	const char *first = inside ? "inside" : "surface";

	const char *cstr = 0;
	switch (color) {
	case 0:
		cstr = "red";
		break;
	case 1:
		cstr = "green";
		break;
	case 2:
		cstr = "blue";
		break;
	default:
		assert(0);
		
	}
	
	if (surfaceNr>1) {
		sprintf (str, "%s%d_%s", first, surfaceNr, cstr);
	} else
		sprintf (str, "%s_%s", first, cstr);

	return str;
}

const char *getLightSliderPicString (int lightNr, int color )
{
	static char str[64];

	const char *cstr = 0;
	switch (color) {
	case 0:
		cstr = "red";
		break;
	case 1:
		cstr = "green";
		break;
	case 2:
		cstr = "blue";
		break;
	default:
		assert(0);
		
	}
	
	sprintf (str,"light%d_%s",lightNr,cstr);

	return str;
}

static const char *COLOR_SLIDER_PIC_STRING[MAIN_SURFACE_AMOUNT_NUM][SURFS_COLOR_SLIDER_NUM] =
{
    { "surface_red",  "surface_green",  "surface_blue",  "inside_red",  "inside_green",  "inside_blue"},
    { "surface2_red", "surface2_green", "surface2_blue", "inside2_red", "inside2_green", "inside2_blue"},
    { "surface3_red", "surface3_green", "surface3_blue", "inside3_red", "inside3_green", "inside3_blue"},
    { "surface4_red", "surface4_green", "surface4_blue", "inside4_red", "inside4_green", "inside4_blue"},
    { "surface5_red", "surface5_green", "surface5_blue", "inside5_red", "inside5_green", "inside5_blue"},
    { "surface6_red", "surface6_green", "surface6_blue", "inside6_red", "inside6_green", "inside6_blue"},
    { "surface7_red", "surface7_green", "surface7_blue", "inside7_red", "inside7_green", "inside7_blue"},
    { "surface8_red", "surface8_green", "surface8_blue", "inside8_red", "inside8_green", "inside8_blue"},
    { "surface9_red", "surface9_green", "surface9_blue", "inside9_red", "inside9_green", "inside9_blue"}
};

// ----------------------------------------------------------------------------
//  Create command names for scanner-parser
// ----------------------------------------------------------------------------

void addCommand (const char *name, void (*func) (void))
{
	symtab_add_surface_name(name, SYM_COMMAND, TRUE, func);
}

void addConstant (const char *name, const int *ptr)
{
	symtab_add_surface_name(name, SYM_INTEGER, TRUE, (void*)ptr);
}

void addConstant (const char *name, const double *ptr)
{
	symtab_add_surface_name(name, SYM_DOUBLE, TRUE, (void *)ptr);
}

void addNumber (const char *name, int *ptr)
{
	symtab_add_surface_name(name, SYM_INTEGER, FALSE, ptr);
}

void addNumber (const char *name, double *ptr)
{
	symtab_add_surface_name(name, SYM_DOUBLE, FALSE, ptr);
}



void    init_surface_main_commands( void )
{
#if 0
	addCommand("draw_surface",		surface_3d);
	addCommand("cut_surface",		surface_cut);
	addCommand("dither_surface",		main_button_surface_print);
	addCommand("dither_curve",		main_button_curve_print);
	addCommand("save_dithered_image",	main_command_save_print);
	addCommand("save_color_image",		main_command_save_xwd);
	addCommand("create_movie",		main_command_create_movie);
	addCommand("quit",			main_button_quit);
	addCommand("set_size",			set_size);
	addCommand("set_width",			set_width);
	addCommand("set_height",		set_height);
	addCommand("set_color",			set_color);
	addCommand("enable_zbuffer",		activate_zbuffer);
	addCommand("disable_zbuffer",		disable_zbuffer);
	addCommand("clear_zbuffer",		reset_zbuffer);
	addCommand("clear_screen",		clear_screen);
	addCommand("optimize_colormap",		refresh_picture);
#endif
}

// ----------------------------------------------------------------------------
//  Create variable names for scanner-parser
// ----------------------------------------------------------------------------

void    init_surface_main_variables( void )
{

	addNumber ("cut_distance", &sym_cut_distance);


	int    i;
	int    j;

	// -------------------------
	//  Miscellaneous variables
	// --------------------------

	symtab_add_surface_name( "filename", SYM_STRING, FALSE,
				 (void*)&surface_filename_data);
   
	// ------------------------------------
	//  Main window variables (9 surfaces)
	// ------------------------------------
	for( i = 0; i < MAIN_SURFACE_AMOUNT_NUM; i++ ) {
		
		symtab_add_surface_name( MAIN_FORMULA_PXYZ_PIC_STRING[i],
					 SYM_POLYXYZ,
					 FALSE,
					 (void*)&main_formula_pxyz_data[i]);
	}
	
	// ----------------------------------------
	//  Main window variables (12 clip surfaces)
	// ----------------------------------------

	for( i = 0; i < MAIN_CLIP_AMOUNT_NUM; i++ ) {
		
		symtab_add_surface_name( MAIN_CLIP_PXYZ_PIC_STRING[i],
					 SYM_POLYXYZ,
					 FALSE,
					 (void*)&main_clip_pxyz_data[i]);
	}

	addNumber("width", &main_width_data);
	addNumber("height", &main_height_data);
	
	// ------------------------------
	//  Light variables (9 surfaces)
	// ------------------------------
	for( i = 0; i < MAIN_SURFACE_AMOUNT_NUM; i++ ) {
		addNumber (COLOR_SLIDER_PIC_STRING[i][0], &color_slider[i].red);
		addNumber (COLOR_SLIDER_PIC_STRING[i][1], &color_slider[i].green);
		addNumber (COLOR_SLIDER_PIC_STRING[i][2], &color_slider[i].blue);
		addNumber (COLOR_SLIDER_PIC_STRING[i][3], &color_slider[i].inside_red);
		addNumber (COLOR_SLIDER_PIC_STRING[i][4], &color_slider[i].inside_green);
		addNumber (COLOR_SLIDER_PIC_STRING[i][5], &color_slider[i].inside_blue);
	}

	for( i = 0; i < LIGHT_SOURCE_MAX_VALUE; i++ ) {
		for( j = 0; j < LIGHT_SLIDER_NUM; j++ ) {
			addNumber(LIGHT_SLIDER_PIC_STRING[i][j], 
				  &light_data[i].color[j]);
		}
	} 
	
	for( i = 0; i < COLOR_BACKGROUND_NUM; i++ ) {
		addNumber(COLOR_BACKGROUND_PIC_STRING[i], &color_background_data[i]);
	}
	
	addNumber("do_background", &color_do_background_data);
	addNumber("gradient", &color_gradient_data);
	


	for( i = 0; i < COLOR_GRADIENT_END_NUM; i++ ) {
		addNumber(COLOR_GRADIENT_END_PIC_STRING[i], 
			   &color_gradient_end_data[i]);
	}
	
	addNumber ("illumination", &light_illumination_data);
	addConstant ("ambient_light", &light_illumination_ambient_data);
	addConstant ("diffuse_light", &light_illumination_diffuse_data);
	addConstant ("reflected_light", &light_illumination_reflected_data);
	addConstant ("transmitted_light", &light_illumination_transmitted_data);
	
	for( i = 0; i < MAIN_SURFACE_AMOUNT_NUM; i++ ) {
		addNumber (LIGHT_SETTINGS_FIRST_PIC_STRING[i][0],
			   &light_settings[i].ambient);
		addNumber (LIGHT_SETTINGS_FIRST_PIC_STRING[i][1],
			   &light_settings[i].diffuse);
		addNumber (LIGHT_SETTINGS_FIRST_PIC_STRING[i][2],
			   &light_settings[i].reflected);
		addNumber (LIGHT_SETTINGS_FIRST_PIC_STRING[i][3],
			   &light_settings[i].transmitted);
		

		addNumber (LIGHT_SETTINGS_SECOND_PIC_STRING[i][0],
			   &light_settings[i].smoothness);
		addNumber (LIGHT_SETTINGS_SECOND_PIC_STRING[i][1],
			   &light_settings[i].transparence);
		addNumber (LIGHT_SETTINGS_SECOND_PIC_STRING[i][2],
			   &light_settings[i].thickness);
	} 
        
    
	for( i = 0; i < LIGHT_SOURCE_MAX_VALUE; i++ ) {
		for( j = 0; j < LIGHT_POSITION_NUM; j++ ) {
			addNumber( LIGHT_POSITION_PIC_STRING[i][j], 
				   &light_data[i].position[j]);
		}
		
		addNumber(LIGHT_VOLUME_PIC_STRING[i],&light_data[i].volume);
	}

	// -----------------
	//  Print variables
	// -----------------
	
	addConstant ("yes", &global_yes_data);
	addConstant ("no", &global_no_data);

	addNumber("dithering_method", &print_dither_data);
	addConstant("floyd_steinberg_filter",&print_dither_floyd_steinberg_data);
	addConstant("jarvis_judis_ninke_filter", &print_dither_jarvis_judis_ninke_data);
	addConstant("stucki_filter", &print_dither_stucki_data);
	addConstant("clustered_dot_ordered_dither", &print_dither_ordered_dither_data);
	addConstant("dispersed_dot_ordered_dither", &print_dither_dispersed_dither_data);
	addConstant("dot_diffusion", &print_dither_dot_diffusion_data);
	addConstant("smooth_dot_diffusion", &print_dither_smooth_dot_diffusion_data);

	addNumber ("serpentine_raster", &print_serpentine_raster_data);

	addNumber ("random_weights", &print_random_weights_data);

 	addNumber ("weight", &print_weight_data);
	addNumber ("barons", &print_barons_data);
	addConstant ("one_baron", &print_barons_one_baron_data);
	addConstant ("two_baron", &print_barons_two_baron_data);

	addNumber ("pattern_size", &print_pattern_size_data);
	addConstant("pattern_4x4", &print_pattern_size_4_x_4_data);
	addConstant("pattern_8x8", &print_pattern_size_8_x_8_data);
	addConstant("pattern_16x16", &print_pattern_size_16_x_16_data);

	addNumber("enhance_edges", &print_enhance_data);
	addNumber("alpha", &print_alpha_data);
	addNumber("background", &print_background_data);
	addNumber("tone_scale_adjustment", &print_tone_data);
	addNumber("gamma", &print_gamma_data);
	addNumber("gamma_correction", &print_gamma_correction_data);
	addNumber("resolution", &print_resolution_data);
	addConstant("res_75dpi", &print_resolution_75dpi_data);
	addConstant("res_100dpi", &print_resolution_100dpi_data);
	addConstant("res_150dpi", &print_resolution_150dpi_data);
	addConstant("res_300dpi", &print_resolution_300dpi_data);
	addConstant("res_600dpi", &print_resolution_600dpi_data);
	addConstant("res_1200dpi", &print_resolution_1200dpi_data);

	addNumber("pixel_size", &print_p_radius_data);
	addNumber("color_file_format", &color_output_data);
	addConstant("xwd", &color_output_xwd_data);
	addConstant("sun", &color_output_sun_data);
	addConstant("ppm", &color_output_ppm_data);
	addConstant("jpg", &color_output_jpeg_data);

	addNumber("color_file_colormap", &colormap_output_data);
	addConstant("Netscape", &colormap_output_netscape_data);
	addConstant("Optimized", &colormap_output_optimized_data);
	addConstant("TrueColor", &colormap_output_true_color_data);

	addNumber("dither_file_format", &print_output_data);
	addConstant("postscript", &print_output_ps_data);
	addConstant("encapsulated", &print_output_eps_data);
	addConstant("X11_bitmap",&print_output_bitmap_data);
	addConstant("tiff", &print_output_tiff_data);
	addConstant("pgm", &print_output_pgm_data);
	addConstant("pbm", &print_output_pbm_data);

	// ----------------------------------
	//  Position variables and constants
	// ----------------------------------

	addNumber ("origin_x", &position_numeric.orig_x);
	addNumber ("origin_y", &position_numeric.orig_y);
	addNumber ("origin_z", &position_numeric.orig_z);

	addNumber ("spec_z", &position_numeric.spectator_z);
	
	addNumber ("rot_x", &position_numeric.rot_x);
	addNumber ("rot_y", &position_numeric.rot_y); 
	addNumber ("rot_z", &position_numeric.rot_z);
	addNumber ("scale_x", &position_numeric.scale_x);
	addNumber ("scale_y", &position_numeric.scale_y);
	addNumber ("scale_z", &position_numeric.scale_z);


	addNumber("perspective", &position_perspective_data);
	addConstant("parallel", &position_perspective_parallel_data);
	addConstant("central", &position_perspective_central_data);

	for( i = 0; i < POSITION_SEQUENCE_NUM; i++ ) {
		addNumber(POSITION_SEQUENCE_PIC_STRING[i],&(position_sequence_data[i]));
	}

	addConstant("translate", &position_sequence_translate_data);
	addConstant("rotate", &position_sequence_rotate_data);
	addConstant("scale", &position_sequence_scale_data);

    // ---------------------------------  
    //  Display variables and constants
    // ---------------------------------

	addNumber("normalize_brightness", &display_normalize_data);
	addNumber("optimize_colors", &display_optimize_data);
	addNumber("dither_colors", &display_color_dither_data);

	addNumber("normalize_factor", &display_max_i_data);
	addNumber("dither_steps", &display_dither_value_data);
	addNumber("antialiasing", &display_ref_level_data);
	addNumber("antialiasing_threshold",&display_ref_treshold_data);
	addNumber("antialiasing_radius", &display_ref_radius_data);
	
	addNumber("depth_cueing", &display_depth_cue_data);
	addNumber("depth", &display_depth_value_data);



	addNumber ("stereo_eye",   &display_numeric.stereo_eye);
	addNumber ("stereo_z",     &display_numeric.stereo_z);			
	addNumber ("stereo_red",   &display_numeric.stereo_red);     		
	addNumber ("stereo_blue",  &display_numeric.stereo_blue);      		
	addNumber ("stereo_green", &display_numeric.stereo_green);      		

	// ---------------------------------
	//  Numeric variables and constants
	// ---------------------------------
	
	addNumber("root_finder", &numeric_root_finder_data);
	addConstant("d_chain_bisection", &numeric_root_d_bisection_data);
	addConstant("d_chain_regula_falsi", &numeric_root_d_regula_data);
	addConstant("d_chain_pegasus", &numeric_root_d_pegasus_data);
	addConstant("d_chain_anderson_bjoerck", &numeric_root_d_anderson_data);
	addConstant("d_chain_newton", &numeric_root_d_newton_data);

	addConstant("bezier_all_roots", &numeric_root_bezier_data);
	
	
	addNumber("epsilon", &numeric_epsilon_data);
	addNumber("iterations", &numeric_iterations_data);
	// ------------------------------
	//  Clip variables and constants
	// ------------------------------
	addNumber("clip", &clip_data);

	addConstant("sphere",      &clip_sphere_data);
	addConstant("tetrahedron", &clip_tetrahedron_data);
	addConstant("cube",        &clip_cube_data);
	addConstant("octahedron",  &clip_octahedron_data);
	addConstant("dodecahedron",&clip_dodecahedron_data);
	addConstant("icosahedron", &clip_icosahedron_data);
	addConstant("cylinder",    &clip_cylinder_data);
	addConstant("user",        &clip_user_data);
	addConstant("none",        &clip_none_data);


	addNumber ("clip_front", &clip_numeric.clip_front);
        addNumber ("clip_back",  &clip_numeric.clip_back);
	addNumber ("radius", &clip_numeric.radius);
	addNumber ("center_x", &clip_numeric.center_x); 
	addNumber ("center_y", &clip_numeric.center_y); 
	addNumber ("center_z", &clip_numeric.center_z); 


	// -----------------------------
	//  Cut variables and constants
	// -----------------------------

	addNumber("cut_input", &cut_input_data);
	addConstant("keyboard", &cut_input_keyboard_data);
	addConstant("pointer", &cut_input_mouse_data);
	
	addNumber("cut_modus", &cut_modus_data);
	addConstant("plane_xy", &cut_modus_one_point_data);
	addConstant("plane_three_points", &cut_modus_three_point_data);

	addNumber ("upper_left_x", &cut_numeric.upper_left_x);
	addNumber ("upper_left_y", &cut_numeric.upper_left_y);
	addNumber ("lower_right_x", &cut_numeric.lower_right_x);
	addNumber ("lower_right_y", &cut_numeric.lower_right_y);
	addNumber ("z_value", &cut_numeric.z_value);
	addNumber ("point_1_x", &cut_numeric.point_1_x);
	addNumber ("point_1_y", &cut_numeric.point_1_y);
	addNumber ("point_1_z", &cut_numeric.point_1_z);
	addNumber ("point_2_x", &cut_numeric.point_2_x);
	addNumber ("point_2_y", &cut_numeric.point_2_y);
	addNumber ("point_2_z", &cut_numeric.point_2_z);
	addNumber ("point_3_x", &cut_numeric.point_3_x);
	addNumber ("point_3_y", &cut_numeric.point_3_y);
	addNumber ("point_3_z", &cut_numeric.point_3_z);
	

	// -------------------------------
	//  Curve variables and constants
	// -------------------------------
	
	addNumber("curve_width", &curve_width_data);
	addNumber("curve_gamma", &curve_gamma_data);

	for( i = 0; i < CURVE_COLOR_SLIDER_NUM; i++ ) {
		addNumber(CURVE_COLOR_SLIDER_PIC_STRING[i],&curve_color_slider_data[i]);
	}

	addNumber ("surface_n", &curve_surface_nr_data);
	addNumber ("surf_nr",   &curve_surface_nr_data);



	memset (sym_cutsurfaces, 0, sizeof(sym_cutsurfaces));
	
	for (i=0; i<sizeof(sym_cutsurfaces)/sizeof(sym_cutsurfaces[0]); i++) {
		char str[64];
		sprintf (str, "cutsurface%d", i+1);
		symtab_add_surface_name (str, SYM_POLYXYZ, FALSE, &sym_cutsurfaces[i]);
	}
}
