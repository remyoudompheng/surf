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

#include <FileWriter.h>
#include <AvailableImageFormats.h>
#include <TreePolynom.h>
#include <Script.h>
#include <polylexyacc.h>
#include <init_parser.h>
#include <DrawFunc.h>
#include <SurfaceCalc.h>
#include <DrawCurve.h>
#include <SymbolTable.h>
#include <addDefaultSymbols.h>
#include <resultant.h>
#include <bit_buffer.h>
#include <float_buffer.h>
#include <dither.h>
#include <MultiVariatePolynom.h>
#include <RootFinder3d.h>
#include <SymbolTable.h>
#include <Triangulator.h>
#include <debug.h>

#ifdef HAVE_READLINE
#  include <readline/readline.h>
#  include <readline/history.h>
#endif

#include <sys/stat.h>
#include <signal.h>

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

#include <assert.h>
#include <errno.h>

#include<iostream>
#include<strstream>
#include<string>

#define PROMPT "-> "

using namespace ScriptVar;

bool Script::quiet = false;
bool Script::stdin_is_a_tty = false;
bool Script::stdout_is_a_tty = false;
bool Script::stop_flag = false;
bool Script::kernel_mode= false;
RgbBuffer* Script::buffer = 0;
bit_buffer* Script::bitbuffer = 0;
float_buffer* Script::zbuffer = 0;
float_buffer* Script::zbuffer3d = 0;
#ifdef HAVE_LIBGTS
Triangulator* Script::tritor = 0;
#endif
SymbolTable* Script::defaultValues = 0;

extern void symtab_delete_element(symtab*); // from lexfunc.cc
extern double Y_AXIS_LR_ROTATE;

namespace {

void replaceCommand(const char* name, void (*func)())
{
	symtab* st = symtab_find_name(name);
	if(st) {
		DMESS("replacing command " << name);
		symtab_delete_element(st);
	} else {
		DMESS("adding new command " << name);
	}
	
	symtab_add_surface_name(name, SYM_COMMAND, 1, (void*)func);
}

}


SymbolTable& Script::getDefaultValues()
{
	if(defaultValues == 0) {
		defaultValues = new SymbolTable();
		addDefaultSymbols(*defaultValues);
	}
	return *defaultValues;
}

void Script::beforeScriptExecution()
{
	for(size_t i = 0; i < MAIN_SURFACE_AMOUNT_NUM; i++) {
		main_formula_pxyz_data[i].n = 0;
	}
	
	std::ostrstream str;
	str << getDefaultValues() << std::ends;
	
	internalExecuteScript(str.str());
	error_begin_char = 0;
	char_number = 0;
	symtab_delete_user_names();

	*zbuffer = -10.0;
}

void Script::internalExecuteScript(const char* str)
{
	if(str == 0) {
		return;
	}

	char* main_formula_data = const_cast<char*>(str);
	set_the_yyinput(main_formula_data, 1, 1);
	scan_labels(main_formula_data);

	int parse_result;
	do {
		if(stop_flag) {
			parse_result = 0;
			stop_flag = false;
			break;
		}
		
		yyrestart(stdin);
		goto_flag = false;
		parse_result = yyparse();
		
		if(goto_flag) {
			set_the_yyinput(&(main_formula_data[goto_label]),
					goto_label + 1, goto_line);
		}
	
	} while (goto_flag);
	
	if(parse_result != 0) { // an error occured
		if(!stdout_is_a_tty) {
			std::cout << "error\n"
				  << error_begin_char << ' '
				  << char_number << '\n';
		}
		std::cout << "ERROR";
		if(!stdin_is_a_tty) {
			std::cout << " in line " << line_number;
		}
		std::cout << ": " << yyerrorstring << '\n';
		std::cout.flush();
	}
}

char* Script::readFile(const char* name)
{
	struct stat buf;
	if(stat(name, &buf) != 0) {
		std::cerr << "Can't stat file '" << name << "'"
			  << " (" << strerror(errno) << ")\n";
		return 0;
	}
	
	size_t size = buf.st_size;
	FILE* f = fopen(name, "r");
	if(f == 0) {
		std::cerr << "Can't open file '" << name << "'"
			  << " (" << strerror(errno) << ")\n";
		return 0;
	}
	
	char* str = new char[size + 1];
	if(fread(str, 1, size, f) != size) {
		std::cerr << "Could not read from file '" << name << "'"
			  << " (" << strerror(errno) << ")\n";
		delete [] str;
		fclose(f);
		return 0;
	}

	fclose(f);
	
	str[size] = 0;
	if(strlen(str) < size) {
		delete [] str;
		std::cerr << "\"" << name << "\" contains binary data.\n";
		return 0;
	}
	
	return str;
}

void Script::executeScriptFromStdin()
{
	beforeScriptExecution();
	
	stdin_is_a_tty = isatty(STDIN_FILENO);
	stdout_is_a_tty = isatty(STDOUT_FILENO);
	
	if(!quiet) {
		if(stdin_is_a_tty && stdout_is_a_tty) {
			std::cout <<
"                      _____\n"
"  ________ __________/ ____\\\n"
" /  ___/  |  \\_  __ \\   __\\\n"
" \\___ \\|  |  /|  | \\/|  |\n"
"/____  >____/ |__|   |__| v" VERSION "\n"
"     \\/\n\n";

		} else {
			std::cout << PACKAGE " " VERSION "\n";
		}
		std::cout.flush();
	}

	if(stdin_is_a_tty) {

#ifdef HAVE_READLINE
		rl_bind_key('\t', reinterpret_cast<Function*>(rl_insert));
		rl_bind_key('^', reinterpret_cast<Function*>(rl_insert));
		char* l;
		while((l = readline(PROMPT)) != 0) {
			if(*l) {
				add_history(l);
				internalExecuteScript(l);
			}
			free(l);
		}
#else
		std::cout << PROMPT;
		std::string line;
		while(std::getline(std::cin, line)) {
			if(line.length() > 0) {
				internalExecuteScript(line.c_str());
			}
			std::cout << PROMPT;
		}
#endif
		std::cout << "\nQuitting.\n";

	} else {
		std::string script = "";
		std::string line;
		while(std::getline(std::cin, line)) {
			if(line == "execute;") { // execute immediately
				internalExecuteScript(script.c_str());
				script.assign(""); // gcc 2.95 doesn't have std::string::clear()
			} else {
				script += line + '\n';
			}
		}
		if(script.length() > 0) {
			internalExecuteScript(script.c_str());
		}
	}
}

void Script::executeScriptFromFile(const char *name)
{
	stdin_is_a_tty = false;
	stdout_is_a_tty = isatty(STDOUT_FILENO);

	const char* str = readFile(name);
	beforeScriptExecution();
	internalExecuteScript(str);
	delete [] str;
}

void Script::init(bool _quiet)
{
	quiet = _quiet;
	init_main_variables();
	addNewCommands();
	buffer = new RgbBuffer(main_width_data, main_height_data);
	bitbuffer = new bit_buffer();
	bitbuffer->setSize(main_width_data, main_height_data);
	zbuffer = new float_buffer(main_width_data, main_height_data);
	*zbuffer = -10.0; // FIXME
#ifdef HAVE_LIBGTS
	tritor = new Triangulator;
#endif
	stop_flag = false;
}

void Script::deinit()
{
	delete buffer;
	delete bitbuffer;
	delete zbuffer;
	delete defaultValues;
	symtab_delete_total();
}

void Script::addNewCommands()
{
	replaceCommand("set_kernel_mode", setKernelMode);
	replaceCommand("set_size", setSize);
	replaceCommand("draw_curve", drawCurve);
	replaceCommand("dither_curve", ditherCurve);
	replaceCommand("cut_with_plane", cutWithPlane);
	replaceCommand("cut_with_surface", cutWithSurface);
	replaceCommand("draw_surface", drawSurface);
	replaceCommand("dither_surface", ditherSurface);
	replaceCommand("save_color_image", saveColorImage);
	replaceCommand("save_dithered_image", saveDitheredImage);
	replaceCommand("save_three_d_image", save3DImage);
	replaceCommand("clear_screen", clearScreen);
	replaceCommand("clear_pixmap", clearScreen);
	replaceCommand("resultant", computeResultant);
	replaceCommand("reset", reset);
        replaceCommand("triangulate_surface", triangulateSurface);
	replaceCommand("print_defaults", printDefaults);
	replaceCommand("print_color_image_formats", printColorImageFormats);
	replaceCommand("print_dither_image_formats", printDitherImageFormats);
	replaceCommand("print_three_d_image_formats", print3DImageFormats);
	replaceCommand("print_position", printPosition);
}

//
// --- Commands
//

void Script::setKernelMode()
{
	kernel_mode = true;
}

void Script::setSize()
{
	checkVariables();
	if(buffer->getWidth() != main_width_data ||
	   buffer->getHeight() != main_height_data) {
		buffer->Realloc(main_width_data, main_height_data);
	}
}


void Script::drawCurve()
{
	setSize();
	draw_func_draw();

	if(kernel_mode) {
		ImageFormats::imgFmt_PPM.saveColorImage("-", *buffer);
	}
}

void Script::drawSurface()
{
	setSize();

	Y_AXIS_LR_ROTATE = 0.0;
	SurfaceCalc sc;

	buffer->clearTags();

	*zbuffer = -10.0;
	sc.surface_calculate(*buffer);

	if(kernel_mode) {
		ImageFormats::imgFmt_PPM.saveColorImage("-", *buffer);
	}

	if(display_numeric.stereo_eye) {
		// -----------------
		//  Draw a 3D image
		// -----------------
		RgbBuffer* intensity = buffer;
		*Script::getZBuffer3d() = -10.0;

		Y_AXIS_LR_ROTATE= 2*atan(display_numeric.stereo_eye/
					 (2*position_numeric.spectator_z) );

		intensity->StereoLeft();

		int back = int(0.299*(float(color_background_data[RED]))
			       + 0.587*(float(color_background_data[GREEN]))
			       + 0.114*(float(color_background_data[BLUE])));
		float distf = display_numeric.stereo_z*display_numeric.stereo_eye/
			      position_numeric.spectator_z;
		int dist = int(distf*(float(min(main_width_data,main_height_data)))/20.0);

		SurfaceCalc sc;
		sc.surface_calculate(*buffer);

		intensity->StereoRight(display_numeric.stereo_red, display_numeric.stereo_green,
                                       display_numeric.stereo_blue, dist, back);

		if(kernel_mode) {
			ImageFormats::imgFmt_PPM.saveColorImage("-", *buffer);
		}
	}
}

void Script::saveColorImage()
{
	checkVariables();

	if(surface_filename_data == 0) {
		std::cerr << "No filename given.\n";
		return;
	}

	ImageFormats::saveColorImage(surface_filename_data, *buffer);
}


void Script::clearScreen()
{
	checkVariables();

	if(kernel_mode) {
		std::cout << "clear_screen\n";
		std::cout.flush();
	}
	
	RgbBuffer* intensity = buffer;
	float_buffer* zbuffer = getZBuffer();

	*intensity = byte(-print_background_data);

	intensity->NullInit_three();
	intensity->clearTags();
    
	*zbuffer = float(clip_numeric.clip_back);
}

void Script::saveDitheredImage()
{
	checkVariables();

	if(surface_filename_data == 0) {
		std::cerr << "No filename given.\n";
		return;
	}

	ImageFormats::saveDitheredImage(surface_filename_data, *bitbuffer);
}

void Script::save3DImage()
{
#ifdef HAVE_LIBGTS
	Triangulator* tritor = getTriangulator();
	
	if(surface_filename_data == 0) {
		std::cerr << "No filename given.\n";
		return;
	}

	ImageFormats::save3DImage(surface_filename_data, *tritor);
#endif
}

void Script::ditherSurface()
{
	checkVariables();

	float_buffer fbuffer(main_width_data, main_height_data);    
	
	bitbuffer->setSize(main_width_data, main_height_data);

	// sk: copy gray_values from rgb_buffer to buffer(float_buffer) 
	copy_rgb_to_float(*buffer, fbuffer, print_background_data);
	
	if(print_enhance_data == 0) {
		fbuffer.EnhanceEdges(print_alpha_data);
	}

	if(print_tone_data == 0) {
		fbuffer.AdjustToneScale();
	}

	if(print_gamma_data != 1.0 && print_gamma_correction_data == 1) {
		fbuffer.CorrectGamma(1.0/print_gamma_data);
	}

	dither_pixel_radius_adjust(fbuffer, float(print_p_radius_data)/100.0);

	if(print_dither_data == print_dither_floyd_steinberg_data) {
		dither_floyd_steinberg(fbuffer, *bitbuffer, print_random_weights_data, print_weight_data, print_serpentine_raster_data);
	} else if(print_dither_data == print_dither_jarvis_judis_ninke_data) {
		dither_jarvis_judis_ninke(fbuffer, *bitbuffer, print_random_weights_data, print_weight_data, print_serpentine_raster_data);
	} else if (print_dither_data == print_dither_stucki_data) {
		dither_stucki (fbuffer, *bitbuffer, print_random_weights_data, print_weight_data, print_serpentine_raster_data);
	} else if (print_dither_data == print_dither_ordered_dither_data) {
  		dither_clustered (fbuffer, *bitbuffer, print_pattern_size_data);
	} else if (print_dither_data == print_dither_dispersed_dither_data) {
  		dither_dispersed (fbuffer, *bitbuffer, print_pattern_size_data);
	} else if (print_dither_data == print_dither_dot_diffusion_data) {
  		dither_dot_diffusion (fbuffer, *bitbuffer, print_barons_data);
	} else if (print_dither_data == print_dither_smooth_dot_diffusion_data) {
  		dither_smooth_dot_diffusion (fbuffer, *bitbuffer, print_barons_data);
	} else {
  		std::cerr << "dithering_method out of range. No dithering done.\n";
	}

	if(kernel_mode) {
		ImageFormats::imgFmt_PBM.saveDitheredImage("-", *bitbuffer);
	}
}


void Script::ditherCurve()
{
	int width = buffer->getWidth();
	int height = buffer->getHeight();

	float_buffer fbuffer(width, height);           

	// copy gray_values from rgb_buffer to float_buffer
	copy_rgb_to_float_curve(*buffer, fbuffer);
    
	dither_brute(fbuffer, *bitbuffer);

	if(kernel_mode) {
		ImageFormats::imgFmt_PBM.saveDitheredImage("-", *bitbuffer);
	}
}

void Script::checkVariables()
{
	if(numeric_epsilon_data <= 0) {
		std::cerr << "WARNING: epsilon = " << numeric_epsilon_data << " <= 0. Setting epsilon to 0.00001\n";
		numeric_epsilon_data = 0.00001;
	}

	if(main_width_data <= 0) {
		std::cerr << "WARNING: width = " << main_width_data << " <= 0. Setting width to 200\n";
		main_width_data = 200;
	}

	if (main_height_data <= 0) {
		std::cerr << "WARNING: height = " << main_height_data << " <= 0. Setting height to 200\n";
		main_height_data = 200;
	}
}


void Script::computeResultant()
{
	SurfaceCalc sc;
	Polyxyz p1(sc.sf_ds.getFormula (curve_surface_nr_data)->pxyz);

	CanvasDataStruct cds;
	cds.initWith_polyxyz (&sym_cutsurfaces[0]); 

	Polyxyz p2(cds.pxyz);

	resultant(p1, p2);
	return;
}

void Script::cutWithPlane()
{
	draw_func_cut();

	if(Script::isKernelMode()) {
		ImageFormats::imgFmt_PPM.saveColorImage("-", *buffer);
	}
	
}

void Script::cutWithSurface()
{
	WindowGeometry wingeo = WindowGeometry(main_width_data, main_height_data);

        Y_AXIS_LR_ROTATE = 0.0;
	checkVariables();
  	buffer->clearCurveTags();
 	Polyx::SetStatics(numeric_epsilon_data, numeric_iterations_data,
 			  numeric_root_finder_data, true);

	Script::getZBuffer()->Realloc(main_width_data, main_height_data);


	SurfaceCalc sc;
	Polyxyz p1 (sc.sf_ds.getFormula (curve_surface_nr_data)->pxyz);

	CanvasDataStruct cds;
	cds.initWith_polyxyz (&sym_cutsurfaces[0]); 
	Polyxyz p2(cds.pxyz);

	DrawCurve dc;
	dc.setCurveWidthAndGamma (curve_width_data, curve_gamma_data);
	NewClip *clip = NewClip::createSimpleClip (position_perspective_data, clip_data);
	clip->init();
	dc.setClip(clip);

	dc.setGeometry (wingeo);
	dc.setPolys (p1,p2);

	for(size_t i = 1; i < sizeof(sym_cutsurfaces)/sizeof(sym_cutsurfaces[0]); i++) {
		if(sym_cutsurfaces[i].n > 0) {
			CanvasDataStruct cds;
			cds.initWith_polyxyz(&sym_cutsurfaces[i]); 
			Polyxyz p3(cds.pxyz);
			dc.setAdditionalPoly(p3);
		}
	}

	dc.setBuffers(buffer, Script::getZBuffer());
	dc.drawCurve(0, 0, main_width_data, main_height_data, sym_cut_distance);
	sc.CalculateCurveOnSurface(0, 0, main_width_data, main_height_data, *buffer, *Script::getZBuffer());

	delete clip;

	if(kernel_mode) {
		ImageFormats::imgFmt_PPM.saveColorImage("-", *buffer);
	}

	if( display_numeric.stereo_eye ) {
		// -----------------
		//  Draw a 3D image
		// -----------------
  		buffer->clearCurveTags();
		Y_AXIS_LR_ROTATE= 2*atan(display_numeric.stereo_eye/
					 (2*position_numeric.spectator_z) );

        	SurfaceCalc sc;
	        Polyxyz p1(sc.sf_ds.getFormula (curve_surface_nr_data)->pxyz);

	        CanvasDataStruct cds;
	        cds.initWith_polyxyz(&sym_cutsurfaces[0]); 
	        Polyxyz p2(cds.pxyz);

	        DrawCurve dc;
	        dc.setCurveWidthAndGamma(curve_width_data, curve_gamma_data);
	        NewClip* clip = NewClip::createSimpleClip(position_perspective_data, clip_data);
	        clip->init();
	        dc.setClip (clip);

	        dc.setGeometry(wingeo);
	        dc.setPolys(p1, p2);

	        for(size_t i = 1; i < sizeof(sym_cutsurfaces)/sizeof(sym_cutsurfaces[0]); i++) {
		        if(sym_cutsurfaces[i].n > 0) {
			        CanvasDataStruct cds;
			        cds.initWith_polyxyz (&sym_cutsurfaces[i]); 
			        Polyxyz p3 (cds.pxyz);
			        dc.setAdditionalPoly (p3);
		        }
	        }
		
		Script::getZBuffer3d()->Realloc(main_width_data, main_height_data);
		

		dc.setBuffers(buffer, Script::getZBuffer3d());
	        dc.drawCurve(0, 0, main_width_data, main_height_data,sym_cut_distance);
	        sc.CalculateCurveOnSurface(0, 0, main_width_data,main_height_data, *buffer, *Script::getZBuffer3d());

	        delete clip;

		if(kernel_mode) {
			ImageFormats::imgFmt_PPM.saveColorImage("-", *buffer);
		}
	}
}

void Script::reset()
{
	for(size_t i = 0; i < MAIN_SURFACE_AMOUNT_NUM; i++) {
		main_formula_pxyz_data[i].n = 0;
	}
	char_number = 0;
	symtab_delete_user_names();
	*zbuffer = -10.0;
}

void Script::triangulateSurface()
{
#ifdef HAVE_LIBGTS
	tritor->triangulate();
	if(kernel_mode) {
		ImageFormats::imgFmt_OOGL.save3DImage("-", *tritor);
	}
#else
	std::cout << "not_implemented\n";
	std::cout.flush();
#endif // HAVE_LIBGTS
}

void Script::printDefaults()
{
	std::cout << getDefaultValues();
	std::cout << "\n";
	std::cout.flush();
}

void Script::printColorImageFormats()
{
	using namespace ImageFormats;
	
	for(size_t i = 0; i != numAvailableFormats; i++) {
		Format* fmt = availableFormats[i];
		if(fmt->isColorFormat()) {
			std::cout << fmt->getName() << '\n'
				  << fmt->getID() << '\n';
		}
	}
	std::cout << "\n";
	std::cout.flush();
}

void Script::printDitherImageFormats()
{
	using namespace ImageFormats;
	
	for(size_t i = 0; i != numAvailableFormats; i++) {
		Format* fmt = availableFormats[i];
		if(fmt->isDitherFormat()) {
			std::cout << fmt->getName() << '\n'
				  << fmt->getID() << '\n';
		}
	}
	std::cout << "\n";
	std::cout.flush();
}

void Script::print3DImageFormats()
{
	using namespace ImageFormats;
	
	for(size_t i = 0; i != numAvailableFormats; i++) {
		Format* fmt = availableFormats[i];
		if(fmt->is3DFormat()) {
			std::cout << fmt->getName() << '\n'
				  << fmt->getID() << '\n';
		}
	}
	std::cout << "\n";
	std::cout.flush();
}

void Script::printPosition()
{
	using namespace ScriptVar;
	
	std::cout << "origin:\t"
		  << position_numeric.orig_x << ' '
		  << position_numeric.orig_y << ' '
		  << position_numeric.orig_z << '\n'
		  << "rotation:\t"
		  << position_numeric.rot_x << ' '
		  << position_numeric.rot_y << ' '
		  << position_numeric.rot_z << '\n'
		  << "scale:\t"
		  << position_numeric.scale_x << ' '
		  << position_numeric.scale_y << ' '
		  << position_numeric.scale_z << '\n'
		  << "sequence:\t";
	for(int i = 0; i != 3; i++) {
		using namespace ScriptVar;
		int s = position_sequence_data[i];
		if(s == position_sequence_translate_data) {
			std::cout << "translate ";
		} else if(s == position_sequence_rotate_data) {
			std::cout << "rotate ";
		} else {
			std::cout << "scale";
		}
	}

	std::cout << '\n';
	std::cout.flush();
}
