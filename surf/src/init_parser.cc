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


#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <init_parser.h>
#include <ScriptVar.h>
#include <monomarith.h>
#include <AvailableImageFormats.h>

#include<string>


using namespace ScriptVar;

double Y_AXIS_LR_ROTATE;
double sym_cut_distance;

// ----------------------------------------------------------------------------
//  Create command names for scanner-parser
// ----------------------------------------------------------------------------

void addCommand(const char* name, void (*func)())
{
	symtab_add_surface_name(name, SYM_COMMAND, true, (void*)func);
}

void addConstant (const char *name, const int *ptr)
{
	symtab_add_surface_name(name, SYM_INTEGER, true, (void*)ptr);
}

void addConstant (const std::string& name, const int *ptr)
{
	symtab_add_surface_name(name.c_str(), SYM_INTEGER, true, (void*)ptr);
}

void addConstant (const char *name, const double *ptr)
{
	symtab_add_surface_name(name, SYM_DOUBLE, true, (void *)ptr);
}

void addNumber (const char *name, int *ptr)
{
	symtab_add_surface_name(name, SYM_INTEGER, false, ptr);
}

void addNumber(const std::string& name, int* ptr)
{
	symtab_add_surface_name(name.c_str(), SYM_INTEGER, false, ptr);
}

void addNumber (const char *name, double *ptr)
{
	symtab_add_surface_name(name, SYM_DOUBLE, false, ptr);
}

void addNumber (const std::string& name, double *ptr)
{
	symtab_add_surface_name(name.c_str(), SYM_DOUBLE, false, ptr);
}

void addString(const char* name, char** ptr)
{
	symtab_add_surface_name(name, SYM_STRING, false, ptr);
}

void addPoly(const char* name, polyxyz* ptr)
{
	symtab_add_surface_name(name, SYM_POLYXYZ, false, ptr);
}

void addPoly(const std::string& name, polyxyz* ptr)
{
	symtab_add_surface_name(name.c_str(), SYM_POLYXYZ, false, ptr);
}


// ----------------------------------------------------------------------------
//  Create variable names for scanner-parser
// ----------------------------------------------------------------------------

void init_main_variables()
{
	addNumber ("cut_distance", &sym_cut_distance);

	addString("filename", &surface_filename_data);
   
	addNumber("width", &main_width_data);
	addNumber("height", &main_height_data);

	addConstant("yes", &global_yes_data);
	addConstant("no", &global_no_data);

	// curve & plane:
	addPoly("curve", &draw_func_function_data);
	addPoly("plane", &draw_func_plane_data);

	// surfaces:
	std::string surface = "surface";
	for(size_t i = 0; i < MAIN_SURFACE_AMOUNT_NUM; i++) {
		std::string suff = "";
		if(i > 0) {
			suff += '1' + i;
		}
		addPoly(surface + suff, &main_formula_pxyz_data[i]);
	}
	
	// clip surfaces:
	std::string clip_surface = "clip_surface";
	for(size_t i = 0; i < MAIN_CLIP_AMOUNT_NUM; i++) {
		std::string suff = "";
		if(i > 0) {
			suff += '1' + i;
		}
		addPoly(clip_surface + suff, &main_clip_pxyz_data[1]);
	}

	// surface colors:
	std::string inside = "inside";
	for(size_t i = 0; i < MAIN_SURFACE_AMOUNT_NUM; i++) {
		std::string suff;
		if(i > 0) {
			suff += '1' + i;
		}
		addNumber(surface + suff + "_red",   &color_slider[i].red);
		addNumber(surface + suff + "_green", &color_slider[i].green);
		addNumber(surface + suff + "_blue",  &color_slider[i].blue);
		addNumber(inside + suff + "_red",   &color_slider[i].inside_red);
		addNumber(inside + suff + "_green", &color_slider[i].inside_green);
		addNumber(inside + suff + "_blue",  &color_slider[i].inside_blue);
	}

	// light sources:
	std::string light = "light";
	for(size_t i = 0; i < LIGHT_SOURCE_MAX_VALUE; i++) {
		char suff = '1' + i;
		addNumber(light + suff + "_red",   &light_data[i].color[0]);
		addNumber(light + suff + "_green", &light_data[i].color[1]);
		addNumber(light + suff + "_blue",  &light_data[i].color[2]);
	} 
	
	addNumber("background_red",   &color_background_data[0]);
	addNumber("background_green", &color_background_data[1]);
	addNumber("background_blue",  &color_background_data[2]);
	
	addNumber("do_background", &color_do_background_data);
	addNumber("gradient", &color_gradient_data);
	
	addNumber("gradient_red",   &color_gradient_end_data[0]);
	addNumber("gradient_green", &color_gradient_end_data[1]);
	addNumber("gradient_blue",  &color_gradient_end_data[2]);

	addNumber("illumination", &light_illumination_data);
	addConstant("ambient_light", &light_illumination_ambient_data);
	addConstant("diffuse_light", &light_illumination_diffuse_data);
	addConstant("reflected_light", &light_illumination_reflected_data);
	addConstant("transmitted_light", &light_illumination_transmitted_data);
	
	std::string ambient = "ambient";
	std::string diffuse = "diffuse";
	std::string reflected = "reflected";
	std::string transmitted = "transmitted";
	std::string smoothness = "smoothness";
	std::string transparence = "transparence";
	std::string thickness = "thickness";
	for(size_t i = 0; i < MAIN_SURFACE_AMOUNT_NUM; i++) {
		std::string suff = "";
		if(i > 0) {
			suff += '1' + i;
		}
		addNumber(ambient + suff, &light_settings[i].ambient);
		addNumber(diffuse + suff, &light_settings[i].diffuse);
		addNumber(reflected + suff, &light_settings[i].reflected);
		addNumber(transmitted + suff, &light_settings[i].transmitted);
		addNumber(smoothness + suff, &light_settings[i].smoothness);
		addNumber(transparence + suff, &light_settings[i].transparence);
		addNumber(thickness + suff, &light_settings[i].thickness);
	} 

	// light positions:
	for(size_t i = 0; i < LIGHT_SOURCE_MAX_VALUE; i++ ) {
		char suff = '1' + i;
		addNumber(light + suff + "_x", &light_data[i].position[0]);
		addNumber(light + suff + "_y", &light_data[i].position[1]);
		addNumber(light + suff + "_z", &light_data[i].position[2]);
		addNumber(light + suff + "_vol", &light_data[i].volume);
	}

	//  print variables:
	addNumber("dithering_method", &print_dither_data);
	addConstant("floyd_steinberg_filter", &print_dither_floyd_steinberg_data);
	addConstant("jarvis_judis_ninke_filter", &print_dither_jarvis_judis_ninke_data);
	addConstant("stucki_filter", &print_dither_stucki_data);
	addConstant("clustered_dot_ordered_dither", &print_dither_ordered_dither_data);
	addConstant("dispersed_dot_ordered_dither", &print_dither_dispersed_dither_data);
	addConstant("dot_diffusion", &print_dither_dot_diffusion_data);
	addConstant("smooth_dot_diffusion", &print_dither_smooth_dot_diffusion_data);

	addNumber("serpentine_raster", &print_serpentine_raster_data);

	addNumber("random_weights", &print_random_weights_data);

 	addNumber("weight", &print_weight_data);
	addNumber("barons", &print_barons_data);
	addConstant("one_baron", &print_barons_one_baron_data);
	addConstant("two_baron", &print_barons_two_baron_data);

	addNumber("pattern_size", &print_pattern_size_data);
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

	using namespace ImageFormats;
	
	addNumber("color_file_format", &color_file_format_data);
	addNumber("dither_file_format", &dither_file_format_data);
	size_t num = ImageFormats::numAvailableFormats;
	for(size_t i = 0; i != num; i++) {
		using namespace ImageFormats;
		
		Format* fmt = availableFormats[i];
		image_formats_data[i] = i;
		addConstant(fmt->getID(), &image_formats_data[i]);
	}

	addNumber("color_file_colormap", &colormap_output_data);
	addConstant("netscape", &colormap_output_netscape_data);
	addConstant("optimized", &colormap_output_optimized_data);
	addConstant("truecolor", &colormap_output_true_color_data);


	//  position variables and constants:
	addNumber("origin_x", &position_numeric.orig_x);
	addNumber("origin_y", &position_numeric.orig_y);
	addNumber("origin_z", &position_numeric.orig_z);

	addNumber("spec_z", &position_numeric.spectator_z);
	
	addNumber("rot_x", &position_numeric.rot_x);
	addNumber("rot_y", &position_numeric.rot_y); 
	addNumber("rot_z", &position_numeric.rot_z);

	addNumber("scale_x", &position_numeric.scale_x);
	addNumber("scale_y", &position_numeric.scale_y);
	addNumber("scale_z", &position_numeric.scale_z);


	addNumber("perspective", &position_perspective_data);
	addConstant("parallel", &position_perspective_parallel_data);
	addConstant("central", &position_perspective_central_data);

	addNumber("first",  &(position_sequence_data[0]));
	addNumber("second", &(position_sequence_data[1]));
	addNumber("third",  &(position_sequence_data[2]));

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
	

	// curve variables and constants:
	addNumber("curve_width", &curve_width_data);
	addNumber("curve_gamma", &curve_gamma_data);

	addNumber("curve_red",   &curve_color_slider_data[0]);
	addNumber("curve_green", &curve_color_slider_data[1]);
	addNumber("curve_blue",  &curve_color_slider_data[2]);

	addNumber("surface_n", &curve_surface_nr_data);
	addNumber("surf_nr",   &curve_surface_nr_data);

	memset (sym_cutsurfaces, 0, sizeof(sym_cutsurfaces));
	
	for(size_t i = 0; i < sizeof(sym_cutsurfaces)/sizeof(sym_cutsurfaces[0]); i++) {
		char str[64];
		sprintf (str, "cutsurface%d", i+1);
		symtab_add_surface_name (str, SYM_POLYXYZ, false, &sym_cutsurfaces[i]);
	}

	// -----------------------------
	// GUI->kernel variables
	// -----------------------------
	
	addNumber("surface_run_commands", &surface_run_commands);


	// -----------------------------
	// image format options
	// -----------------------------

	addNumber("jpeg_quality", &jpeg_quality);
}
