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




#include <assert.h>
#include <stdio.h>
#include <errno.h>

#include <iostream.h>

#include <sys/stat.h>
#include <unistd.h>



#include "FileWriter.h"
#include "ImageFormats.h"
#include "TreePolynom.h"
#include "Misc.h"
#include "Script.h"
#include "polylexyacc.h"
#include "gui_config.h"
#include "init_parser.h"
#include "DrawFunc.h"
#include "TSDrawingArea.h"
#include "SurfaceCalc.h"
#include "DrawCurve.h"
#include "SymbolTable.h"
#include "addDefaultSymbols.h"

#include "bit_buffer.h"
#include "float_buffer.h"
#include "dither.h"

#include "Thread.h"

#include "MultiVariatePolynom.h"
#include "RootFinder3d.h"
#include "GuiThread.h"

#include "SymbolTable.h"
#include "symtab.h"

// #define DEBUG
#include "debug.h"


TSDrawingArea	*Script::display	= 0;
RgbBuffer	*Script::buffer		= 0;
bit_buffer	*Script::bitbuffer	= 0;
float_buffer	*Script::zbuffer	= 0;
float_buffer	*Script::zbuffer3d	= 0;
SymbolTable     *Script::defaultValues  = 0;
Preview          Script::preview;

Condition scriptRunning;


/* extern "C" */ void symtab_delete_element(symtab *);

static void replaceCommand (const char *name, void (*func) (void))
{
	symtab *st = symtab_find_name (name);
	if (st) {
		DMESS("replacing command " << name);
		symtab_delete_element(st);
	} else {
		DMESS("adding new command " << name);
	}
	
	symtab_add_surface_name (name, SYM_COMMAND, 1, func);
}


SymbolTable &Script::getDefaultValues()
{
	if (defaultValues == 0) {
		defaultValues = new SymbolTable();
		addDefaultSymbols(*defaultValues);
	}
	return *defaultValues;
}


void *Script::startThread (void *data)
{
	ExecuteScriptStruct *ess = (ExecuteScriptStruct *) data;

//  	int i;
//  	for (i=0; i<4; i++) {
//  		main_mosaic_choice_data[i] = ess->preview[i];
//  	}
	preview = ess->preview;

	beforeScriptExecution();
	if (ess->firstPart) {
		if (internalExecuteScript (ess->firstPart)!=0) {
			Misc::alert ("internal error.");
		}
	}
	
	assert(ess->secondPart);
	ess->parse_result = internalExecuteScript (ess->secondPart, ess->executeUserScriptCommands);

	ess->error_begin = error_begin_char;
	ess->error_end = char_number;

	ess->errorString = yyerrorstring;

	if (ess->parse_result == 0) {
		internalExecuteScript (ess->thirdPart);
	}

	scriptRunning.lock();
	scriptRunning.value = 0;
	scriptRunning.unlock();

	if (ess->doneCallback) {
		ess->doneCallback (ess);
	}
	return 0;
}

bool Script::startScriptExecution(ExecuteScriptStruct *ess)
{
	if (!scriptRunning.tryLock()) {
		Misc::alert("another script is running.");
	} else if (scriptRunning.value != 0) {
		scriptRunning.unlock();
		Misc::alert("another script is running.");
	} else {
		scriptRunning.value = 1;
		scriptRunning.unlock();
		setDisplay (ess->drawingArea);

		ess->thread = new Thread();
		
		ess->thread->start (startThread, ess);
		return true;
	}

	return false;
}

TSDrawingArea *Script::getDisplay()
{
	return display;
}

void Script::setDisplay (TSDrawingArea *_display)
{
	display = _display;
}

void Script::beforeScriptExecution()
{
	int i;
	for( i=0; i<MAIN_SURFACE_AMOUNT_NUM; i++ ) {
		main_formula_pxyz_data[i].n = 0;
	}
	
	ostrstream str;
	str << getDefaultValues() << ends;
	
	internalExecuteScript (str.str());
	// symtab_set_default();	
	error_begin_char = 0;
	char_number=0;
	symtab_delete_user_names( );

	*zbuffer = -10.0;
}

int Script::internalExecuteScript (const char *str, bool runCommands)
{
	if (str==0)
		return 0;
	char    *main_formula_data = 0;

	surface_run_commands = runCommands;
	
	int parse_result;
	main_formula_data = (char *) str;
	set_the_yyinput (main_formula_data, 1, 1 );
	scan_labels( main_formula_data );

	do {
		if (Thread::shouldStop()) {
			parse_result = 0;
			break;
		}
			
		yyrestart( stdin );
		goto_flag    = FALSE;
		parse_result = yyparse( );
		
		if( goto_flag ) {
			set_the_yyinput( &(main_formula_data[goto_label]), goto_label+1,goto_line );
		}
	} while ( goto_flag );
	
// 	if (parse_result) {
// 		Misc::alert("Syntax error in script");
// 		printf ("%20s\n", str+char_number);
// 	}
	return parse_result;
}


char *Script::readFile (const char *name)
{
	BEGIN("readFile");
	

	struct stat buf;
	if (stat(name, &buf) != 0) {
		ostrstream ostr;
		ostr << "Can´t stat file ´" << name << "´" << " (" << strerror(errno) << ")" << ends;
		Misc::alert (ostr);
		return 0;
	}

	unsigned int size = buf.st_size;
	FILE *f=fopen(name, "r");
	if (f==0) {
		ostrstream ostr;
		ostr << "Can´t open file ´" << name << "´" << " (" << strerror(errno) << ")";
		Misc::alert (ostr);
		return 0;
	}
	
	char *str = new char [size+1];
	if (fread(str, 1, size, f) != size) {
		ostrstream ostr;
		ostr << "Could not read from file ´" << name << "´" << " (" << strerror(errno) << ")";
		Misc::alert(ostr);
		delete str;
		fclose(f);
		return 0;
	}

	fclose(f);
	str[size]=0;
	if (strlen(str) < size) {
		delete str;
		ostrstream ostr;
		ostr << "\"" << name << "\" contains binary data.";
		Misc::alert(ostr);
		return 0;
	}
	return str;
}

void Script::executeScriptFromFile (const char *name)
{
	BEGIN("Script::executeScriptFromFile");
	Thread::setDoing ("executing script...");
	const char *str = readFile(name);
	display=0;
	beforeScriptExecution();
	internalExecuteScript(str);
	delete str;
}

void Script::init()
{
	BEGIN("Script::init");
//  	addDefaultSymbols (getDefaultValues());

	init_surface_main_commands();
	init_surface_main_variables();
	draw_func_init_parser();
	addNewCommands();
	buffer = new RgbBuffer (main_width_data, main_height_data);
	bitbuffer = new bit_buffer();
	bitbuffer->setSize (main_width_data, main_height_data);
	zbuffer = new float_buffer (main_width_data, main_height_data);
	*zbuffer = -10.0; // FIXME
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
	replaceCommand("set_size", setSize);
	replaceCommand("draw_surface", drawSurface);
	replaceCommand("save_color_image", saveColorImage);
	replaceCommand("clear_screen", clearScreen);
	replaceCommand("save_dithered_image", saveDitheredImage);
	replaceCommand("dither_surface", ditherSurface);
	replaceCommand("cut_with_surface", cutWithSurface);
	replaceCommand("resultant", computeResultant);
	replaceCommand("dither_curve", ditherCurve);
	replaceCommand("clear_pixmap", clearPixmap);
}

//
// --- Commands
//

void Script::setSize()
{
	checkVariables();

	BEGIN("Script::setSize");
	if (display) {
		display->setSize(main_width_data, main_height_data);
	}
	buffer->Realloc(main_width_data, main_height_data);
}


extern double Y_AXIS_LR_ROTATE;

void Script::drawSurface()
{
	checkVariables();
	setSize();
	BEGIN("Script::drawSurface");
	TRACE(main_width_data);
	TRACE(main_height_data);

	if (getDisplay()) {
		getDisplay()->showColorAreaWindow();
		getDisplay()->setSize (main_width_data, main_height_data);
	}

	Y_AXIS_LR_ROTATE = 0.0;
	SurfaceCalc sc;
	sc.setDisplay (getDisplay());
	sc.setPreview (getPreview());

	getBuffer()->clearTags();

	*getZBuffer() = -10.0;
	sc.surface_calculate(0, 0, main_width_data, main_height_data, *getBuffer());


	if( display_numeric.stereo_eye ) {
		// -----------------
		//  Draw a 3D image
		// -----------------
		RgbBuffer *intensity = 	getBuffer();
		*Script::getZBuffer3d() = -10.0;


		Y_AXIS_LR_ROTATE= 2*atan( display_numeric.stereo_eye/
					  (2*position_numeric.spectator_z) );

		intensity->StereoLeft( );

		int     back =(int)( 0.299*((float)(color_background_data[RED]))
				     +0.587*((float)(color_background_data[GREEN]))
				     +0.114*((float)(color_background_data[BLUE])));
		float   distf = display_numeric.stereo_z*display_numeric.stereo_eye/
			        position_numeric.spectator_z;
		int     dist = (int)(distf*((float)
					    (min(main_width_data,main_height_data)))/20.0);

		SurfaceCalc sc;
		sc.setDisplay (getDisplay());
		sc.setPreview (getPreview());
		sc.surface_calculate(0, 0, main_width_data, main_height_data,*getBuffer());

		intensity->StereoRight( display_numeric.stereo_red,display_numeric.stereo_green,
                                        display_numeric.stereo_blue,dist,back );
		if (GuiThread::haveGUI())
			getDisplay()->drawRgbBuffer (*intensity);
	}

}


void Script::saveColorImage ()
{
	if (Thread::shouldStop())
		return;

	checkVariables();

	BEGIN("Script::saveColorImage");
	Thread::setDoing("saving color image...");

	if (surface_filename_data == 0) {
		Misc::alert("no filename given.");
		return;
	}
	
	ImageFormats::saveColorImage(surface_filename_data,
				     *buffer, false);
}





void Script::clearScreen()
{
	if (Thread::shouldStop())
		return;

	checkVariables();

	// FIXME: clearing visible parts ???

	RgbBuffer *intensity = getBuffer();
	float_buffer *zbuffer = getZBuffer();

	*intensity = (byte)(-print_background_data);

  	if (getDisplay()) {
  		getDisplay()->showColorAreaWindow();
		getDisplay()->setSize(main_width_data, main_height_data);
  		getDisplay()->drawSquare(0,0, intensity->getWidth(),0,0,0);
  		getDisplay()->displayRectangle(0,0,intensity->getWidth(), intensity->getHeight());
		GuiThread::executeCommands();
  	}
//  	main_newcolor_init ( );
	intensity->NullInit_three( );
    
	*zbuffer = (float)clip_numeric.clip_back;
}

// should there really be any difference between the two ????

void Script::clearPixmap()
{
	if (Thread::shouldStop())
		return;

	checkVariables();

	// FIXME: clearing visible parts ???

	RgbBuffer *intensity = getBuffer();
	float_buffer *zbuffer = getZBuffer();

	*intensity = (byte)(-print_background_data);

    	if (getDisplay()) {
    		getDisplay()->showColorAreaWindow();
//   		getDisplay()->setSize(main_width_data, main_height_data);
    		getDisplay()->drawSquare(0, 0, intensity->getWidth(),0.0,0,0);
//    		getDisplay()->displayRectangle(0,0,intensity->getWidth(), intensity->getHeight());
//  		GuiThread::executeCommands();
    	}
//  	main_newcolor_init ( );
	intensity->NullInit_three( );
	intensity->clearTags();

	*zbuffer = (float)clip_numeric.clip_back;
	
}


void Script::saveDitheredImage()
{
	if (Thread::shouldStop())
		return;

	checkVariables();

	BEGIN("Script::saveDitheredImage");
	Thread::setDoing ("saving dithered image...");
	bit_buffer* pixel = getBitBuffer();

	if (surface_filename_data == 0) {
		Misc::alert("no filename given.");
		return;
	}

	ImageFormats::saveDitheredImage(surface_filename_data, *pixel,
					main_width_data, main_height_data,
					print_resolution_array_data[print_resolution_data],
					false);
}

void Script::ditherSurface()
{
	if (Thread::shouldStop())
		return;

	checkVariables();

	BEGIN("ditherSurface");
	
	Thread::setDoing ("dithering surface...");


	float_buffer fbuffer (main_width_data, main_height_data);    
	
	bit_buffer *pixel = getBitBuffer();
	pixel->setSize (main_width_data, main_height_data);

	// sk :copy gray_values from rgb_buffer to buffer(float_buffer) 
	copy_rgb_to_float(*getBuffer(), fbuffer, print_background_data);
	
	if( print_enhance_data == 0) {
		fbuffer.EnhanceEdges( print_alpha_data );     
	}

	if( print_tone_data == 0) {
		fbuffer.AdjustToneScale();
	}

	if( print_gamma_data != 1.0 && print_gamma_correction_data == 1) {
		fbuffer.CorrectGamma( 1.0/print_gamma_data );
	}


	dither_pixel_radius_adjust (fbuffer, (float)print_p_radius_data/100.0);

	if (print_dither_data == print_dither_floyd_steinberg_data) {
		dither_floyd_steinberg (fbuffer, *pixel, print_random_weights_data, print_weight_data, print_serpentine_raster_data);
	} else if (print_dither_data == print_dither_jarvis_judis_ninke_data) {
		dither_jarvis_judis_ninke (fbuffer, *pixel, print_random_weights_data, print_weight_data, print_serpentine_raster_data);
	} else if (print_dither_data == print_dither_stucki_data) {
		dither_stucki (fbuffer, *pixel, print_random_weights_data, print_weight_data, print_serpentine_raster_data);
	} else if (print_dither_data == print_dither_ordered_dither_data) {
  		dither_clustered (fbuffer, *pixel, print_pattern_size_data);
	} else if (print_dither_data == print_dither_dispersed_dither_data) {
  		dither_dispersed (fbuffer, *pixel, print_pattern_size_data);
	} else if (print_dither_data == print_dither_dot_diffusion_data) {
  		dither_dot_diffusion (fbuffer, *pixel, print_barons_data);
	} else if (print_dither_data == print_dither_smooth_dot_diffusion_data) {
  		dither_smooth_dot_diffusion (fbuffer, *pixel, print_barons_data);
	} else {
  		Misc::alert ("dithering_method out of range. no dithering done.");
	}

	if (getDisplay()) {
		getDisplay()->showDitherAreaWindow();
		getDisplay()->drawBitbuffer(*pixel);
	}
}


void Script::ditherCurve()
{
	int width  = getBuffer()->getWidth();
	int height = getBuffer()->getHeight();

	float_buffer buffer (width, height);           

	// copy gray_values from rgb_buffer to float_buffer
	copy_rgb_to_float_curve(*getBuffer(), buffer);
    
	dither_brute (buffer, *getBitBuffer());

	if (getDisplay()) {
		getDisplay()->showDitherAreaWindow();
		getDisplay()->setSize(width, height);
		getDisplay()->drawBitbuffer(*getBitBuffer());
	}
}

void Script::checkVariables()
{
	if (numeric_epsilon_data <= 0) {
		ostrstream cerr;
		cerr << "WARNING: epsilon = " << numeric_epsilon_data << " <= 0. Setting epsilon to 0.00001" << endl;
		numeric_epsilon_data = 0.00001;
		Misc::alert(cerr);
	}

	if (main_width_data <= 0) {
		ostrstream cerr;		
		cerr << "WARNING: width = " << main_width_data << " <= 0. Setting width to 200" << endl;
		main_width_data = 200;
		Misc::alert(cerr);		
	}

	if (main_height_data <= 0) {
		ostrstream cerr;				
		cerr << "WARNING: height = " << main_height_data << " <= 0. Setting height to 200" << endl;
		main_height_data = 200;
		Misc::alert(cerr);		
	}
}

#include "resultant.h"
void Script::computeResultant()
{
	SurfaceCalc sc;
	Polyxyz p1 (sc.sf_ds.getFormula (curve_surface_nr_data)->pxyz);

	CanvasDataStruct cds;
	cds.initWith_polyxyz (&sym_cutsurfaces[0]); 
	Polyxyz p2 (cds.pxyz);


	resultant(p1,p2);
	return;
}

void Script::cutWithSurface()
{
	if (getDisplay()) {
		getDisplay()->showColorAreaWindow();
		getDisplay()->setSize (main_width_data, main_height_data);
	}

	WindowGeometry wingeo = WindowGeometry(main_width_data, main_height_data);

        Y_AXIS_LR_ROTATE = 0.0;
	checkVariables();
  	Script::getBuffer()->clearCurveTags( );
 	Polyx::SetStatics( numeric_epsilon_data, numeric_iterations_data,
 			   numeric_root_finder_data, true );

	Script::getZBuffer()->Realloc(main_width_data, main_height_data);
	


	SurfaceCalc sc;
	sc.setDisplay(Script::getDisplay());
	Polyxyz p1 (sc.sf_ds.getFormula (curve_surface_nr_data)->pxyz);

	CanvasDataStruct cds;
	cds.initWith_polyxyz (&sym_cutsurfaces[0]); 
	Polyxyz p2 (cds.pxyz);

	DrawCurve dc;
	dc.setCurveWidthAndGamma (curve_width_data, curve_gamma_data);
	NewClip *clip = NewClip::createSimpleClip (position_perspective_data, clip_data);
	clip->init();
	dc.setClip (clip);

	dc.setGeometry (wingeo);
	dc.setPolys (p1,p2);

	for (unsigned int i = 1; i < sizeof(sym_cutsurfaces)/sizeof(sym_cutsurfaces[0]); i++) {
		if (sym_cutsurfaces[i].n > 0) {
			CanvasDataStruct cds;
			cds.initWith_polyxyz (&sym_cutsurfaces[i]); 
			Polyxyz p3 (cds.pxyz);
			dc.setAdditionalPoly (p3);
		}
	}

	dc.setBuffers (Script::getBuffer(), Script::getZBuffer());
	dc.drawCurve(0,0,main_width_data, main_height_data, sym_cut_distance);
	sc.CalculateCurveOnSurface(0,0,main_width_data,main_height_data,*Script::getBuffer(), *Script::getZBuffer() );

	delete clip;


	if( display_numeric.stereo_eye ) {
		// -----------------
		//  Draw a 3D image
		// -----------------
//	        Script::getBuffer()->clearTags( );
  		Script::getBuffer()->clearCurveTags( );
		Y_AXIS_LR_ROTATE= 2*atan( display_numeric.stereo_eye/
					  (2*position_numeric.spectator_z) );

		//int     back =(int)( 0.299*((float)(color_background_data[RED]))
		//		     +0.587*((float)(color_background_data[GREEN]))
		//		     +0.114*((float)(color_background_data[BLUE])));
		//float   distf = display_numeric.stereo_z*display_numeric.stereo_eye/
		//	        position_numeric.spectator_z;
		//int     dist = (int)(distf*((float)
		//			    (min(main_width_data,main_height_data)))/20.0);
        	SurfaceCalc sc;
	        sc.setDisplay(Script::getDisplay());
	        Polyxyz p1 (sc.sf_ds.getFormula (curve_surface_nr_data)->pxyz);

	        CanvasDataStruct cds;
	        cds.initWith_polyxyz (&sym_cutsurfaces[0]); 
	        Polyxyz p2 (cds.pxyz);

	        DrawCurve dc;
	        dc.setCurveWidthAndGamma (curve_width_data, curve_gamma_data);
	        NewClip *clip = NewClip::createSimpleClip (position_perspective_data, clip_data);
	        clip->init();
	        dc.setClip (clip);

	        dc.setGeometry (wingeo);
	        dc.setPolys (p1,p2);

	        for (unsigned int i = 1; i < sizeof(sym_cutsurfaces)/sizeof(sym_cutsurfaces[0]); i++) {
		        if (sym_cutsurfaces[i].n > 0) {
			        CanvasDataStruct cds;
			        cds.initWith_polyxyz (&sym_cutsurfaces[i]); 
			        Polyxyz p3 (cds.pxyz);
			        dc.setAdditionalPoly (p3);
		        }
	        }
		
		Script::getZBuffer3d()->Realloc(main_width_data, main_height_data);
		

		dc.setBuffers (Script::getBuffer(), Script::getZBuffer3d());
	        dc.drawCurve(0,0,main_width_data, main_height_data,sym_cut_distance );
	        sc.CalculateCurveOnSurface(0,0,main_width_data,main_height_data,*Script::getBuffer(), *Script::getZBuffer3d() );

	        delete clip;
	}
}
