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



#include <SurfaceCalc.h>
#include <RgbBuffer.h>
#include <degree.h>
#include <Script.h>
#include <color.h>
#include <roots.h>
#include <compfn.h>
#include <ScriptVar.h>
#include <float_buffer.h>
#include <antialiasing_factor.h>
#include <debug.h>
#include <Misc.h>

#include <math.h>

extern double Y_AXIS_LR_ROTATE;

namespace {
inline void set_null_root_n(int roots[MAIN_SURFACE_AMOUNT_NUM])
{
	memset(roots, 0, sizeof(roots));
}

inline void set_null_root_n_with_curve(int roots[MAIN_SURFACE_AMOUNT_NUM + 1])
{
	memset(roots, 0, sizeof(roots));
}
}

void SurfaceCalc::packLights()
{
        numLights = 0;

	for(int i = 0; i < LIGHT_SOURCE_MAX_VALUE; i++) {
		if(licht[i].red > 0 || licht[i].green > 0 || licht[i].blue > 0) {
			if(numLights != i) {
				LPOS[numLights] = LPOS[i];
				licht[numLights] = licht[i];
			}
			numLights++;
		}
	}
}

void SurfaceCalc::surface_refine_pixel(int px_refine, int py_refine,
				       colorrgb& pixel_color_refine)
{
	pixel_color_refine.setBlack();

	double root[MAIN_SURFACE_AMOUNT_NUM][MAX_DEGREE];       	
	double root_last_pixel[MAIN_SURFACE_AMOUNT_NUM][MAX_DEGREE];
	int root_last_pixel_n[MAIN_SURFACE_AMOUNT_NUM];	
	
	float refine_radius = ScriptVar::display_ref_radius_data;     
	
	double p_inc = refine_radius/ScriptVar::display_ref_level_data;  
	double px_start = px_refine - (refine_radius/2.0) + 0.5*p_inc;  
	double py_start = py_refine - (refine_radius/2.0) + 0.5*p_inc;  
	double stepwidth = fabs(pixel_to_user_x(px_start) - pixel_to_user_x(px_start + p_inc));
	
	// --------------------------------------------------------------------
	//  Cycle through all lines of the pixel 
	// --------------------------------------------------------------------
	double py;
	int py_count;
	for(py = py_start, py_count = 0;
	    py_count < ScriptVar::display_ref_level_data;
	    py_count++, py += p_inc) {

		double uy = pixel_to_user_y(py);
		
		if(clipper->clip_user_y(uy)) {
			
			//  Prepare formula:
			for(int i = 0; i < sf_ds.surf_n; i++) {  
				sf_ds.formulas[i].hpxyz->setRow(uy);  
				sf_ds.formulas[i].hpDxyz->setRow(uy);  
				sf_ds.formulas[i].hpxDyz->setRow(uy);  
			}
			
			// At first point of line assume that no root has been found before:
			set_null_root_n(root_last_pixel_n);
			
			//  Cycle through all points of a line:
			double px;
			int px_count;
			for(px = px_start, px_count = 0;
			    px_count < ScriptVar::display_ref_level_data;
			    px_count++, px += p_inc) {

				//  Get screen x-coordinate:
				double ux = pixel_to_user_x( px );
				
				double zmin;
				double zmax;

				if(clipper->clip_user_xy(ux, uy, zmin, zmax)) {

					//  Prepare formula:
					for(int i = 0; i < sf_ds.surf_n; i++) {          
						sf_ds.formulas[i].hpxyz->setColumn(ux);
					}
					
					// ----------------------------------------------------
					//  Look for roots on the z-axis in the interval
					//  [zmin,zmax] with estimates 
					//  root_last_pixel[0] 
					//  ...
					//  root_last_pixel[root_last_pixel_n-1]
					// ----------------------------------------------------
					sf_ds.root_n_all = 0;				

					for(int i = 0; i < sf_ds.surf_n; i++) {  
						sf_ds.surfaces[i].root_n = zero_of_polyx(&sf_ds.formulas[i].hpxyz->pZ,
											 zmax, zmin, root[i],			
											 root_last_pixel[i], root_last_pixel_n[i],
											 clipper);
						sf_ds.root_n_all += sf_ds.surfaces[i].root_n;
						root_last_pixel_n[i] = sf_ds.surfaces[i].root_n;
					} 
					if(sf_ds.root_n_all) {
						
						//  Prepare formula:
						for(int i = 0; i < sf_ds.surf_n; i++) {  
							sf_ds.formulas[i].hpDxyz->setColumn(ux);
							sf_ds.formulas[i].hpxDyz->setColumn(ux);
						}
						
						colorrgb pixel_tmp;
						illumination_of_surface(ux, uy, root, root_last_pixel,
									stepwidth, pixel_tmp);

						pixel_tmp *= antialiasing_factor(px_count, py_count, ScriptVar::display_ref_level_data);
						pixel_color_refine += pixel_tmp;
						
					} else {

						//  No surface point is background:
						set_null_root_n(root_last_pixel_n);
						
						if(!ScriptVar::color_do_background_data) {
							colorrgb pixel_tmp = backgr;
							pixel_tmp *= antialiasing_factor(px_count, py_count, ScriptVar::display_ref_level_data);
							pixel_color_refine += pixel_tmp;
						}
					}
				} else {

					//  Invisible point is background:
					set_null_root_n(root_last_pixel_n);
					
					if(!ScriptVar::color_do_background_data) {
						colorrgb pixel_tmp = backgr;
						pixel_tmp *= antialiasing_factor(px_count, py_count, ScriptVar::display_ref_level_data);
						pixel_color_refine += pixel_tmp;
					}
					
				}
				
			} // end , foreach Pixel
			
		} else {

			//  Invisible line is background, don't
			if(!ScriptVar::color_do_background_data) {
				double px;
				int px_count;
				for(px = px_start, px_count = 0;
				     px_count < ScriptVar::display_ref_level_data;
				     px_count++, px += p_inc) {
					colorrgb pixel_tmp = backgr;
					pixel_tmp *= antialiasing_factor(px_count, py_count, ScriptVar::display_ref_level_data);
					pixel_color_refine += pixel_tmp;
				}
			}
		}		
	} 
}


SurfaceCalc::SurfaceCalc()
	: cb(-10.0), cf(10.0)
{
	main_factor_set();
	clipper = NewClip::createSimpleClip(ScriptVar::position_perspective_data, ScriptVar::clip_data);
	clipper->init();
	sf_ds.init();
}

inline int SurfaceCalc::needsRefining(const colorrgb& i1, const colorrgb& i2, const colorrgb& i3,
				      const colorrgb& i4, const colorrgb& i5, const colorrgb& i6,
				      const colorrgb& i7, const colorrgb& i8, const colorrgb& i9)
{
	return     fabs( i5.red  - i1.red  ) > ScriptVar::display_ref_treshold_data 
		|| fabs( i5.green- i1.green) > ScriptVar::display_ref_treshold_data 
		|| fabs( i5.blue - i1.blue ) > ScriptVar::display_ref_treshold_data 
		|| fabs( i5.red  - i2.red  ) > ScriptVar::display_ref_treshold_data 
		|| fabs( i5.green- i2.green) > ScriptVar::display_ref_treshold_data 
		|| fabs( i5.blue - i2.blue ) > ScriptVar::display_ref_treshold_data 
		|| fabs( i5.red  - i3.red  ) > ScriptVar::display_ref_treshold_data 
		|| fabs( i5.green- i3.green) > ScriptVar::display_ref_treshold_data 
		|| fabs( i5.blue - i3.blue ) > ScriptVar::display_ref_treshold_data 
		|| fabs( i5.red  - i4.red  ) > ScriptVar::display_ref_treshold_data 
		|| fabs( i5.green- i4.green) > ScriptVar::display_ref_treshold_data 
		|| fabs( i5.blue - i4.blue ) > ScriptVar::display_ref_treshold_data 
		|| fabs( i5.red  - i6.red  ) > ScriptVar::display_ref_treshold_data 
		|| fabs( i5.green- i6.green) > ScriptVar::display_ref_treshold_data 
		|| fabs( i5.blue - i6.blue ) > ScriptVar::display_ref_treshold_data 
		|| fabs( i5.red  - i7.red  ) > ScriptVar::display_ref_treshold_data 
		|| fabs( i5.green- i7.green) > ScriptVar::display_ref_treshold_data 
		|| fabs( i5.blue - i7.blue ) > ScriptVar::display_ref_treshold_data 
		|| fabs( i5.red  - i8.red  ) > ScriptVar::display_ref_treshold_data 
		|| fabs( i5.green- i8.green) > ScriptVar::display_ref_treshold_data 
		|| fabs( i5.blue - i8.blue ) > ScriptVar::display_ref_treshold_data 
		|| fabs( i5.red  - i9.red  ) > ScriptVar::display_ref_treshold_data 
		|| fabs( i5.green- i9.green) > ScriptVar::display_ref_treshold_data 
		|| fabs( i5.blue - i9.blue ) > ScriptVar::display_ref_treshold_data;
}


void SurfaceCalc::surface_calculate(RgbBuffer& intensity)
{
	BEGIN("SurfaceCalc::surface_calculate");

	int width = ScriptVar::main_width_data;
	int height = ScriptVar::main_height_data;

	// -------------------
	//  min/max on z-axis    
	// -------------------
	double  zmin;
	double  zmax;
	
	// ----------------------
	//  save one Pixel-Color
	// ----------------------
	colorrgb  pixel_color;     					  
	
	pixel_max_i = 0.0;  
	
	// ----------------
	//  root variables 
	// ----------------
	double  root[MAIN_SURFACE_AMOUNT_NUM][MAX_DEGREE];       	  
	double  root_last_pixel[MAIN_SURFACE_AMOUNT_NUM][MAX_DEGREE]; 
	int     root_last_pixel_n[MAIN_SURFACE_AMOUNT_NUM];		  
	
	initVars();
	
	double stepwidth = pixel_to_user_x(1) - pixel_to_user_x(2);
	
	Misc::progress("Rendering surface");
	int image_length = 3*width*height;
	int bytes_written = 0;

	// ----------------------------------------
	//  cycle through all lines of the picture
	// ----------------------------------------
			
	for(int py = 0; py < height; py++) {
		double uy = pixel_to_user_y( py );

		if(Script::isStopped()) {
			Misc::progress(Misc::aborted);
			return;
		}
				
		if(clipper->clip_user_y(uy)) {
			// ------------------
			//  prepare surfaces
			// ------------------
			for(int i = 0; i < sf_ds.surf_n; i++) { 
				sf_ds.formulas[i].hpxyz->setRow(uy);
				sf_ds.formulas[i].hpDxyz->setRow(uy);
				sf_ds.formulas[i].hpxDyz->setRow(uy);
			}

			// --------------------------------------------
			//  At first point of line assume that no root
			//  has been found before
			// --------------------------------------------
			set_null_root_n(root_last_pixel_n);
			
			// ------------------------------------
			//  cycle through all points of a line
			// ------------------------------------
			for(int px = 0; px < width; px++) {
				double ux = pixel_to_user_x( px );
				
				if(clipper->clip_user_xy(ux, uy, zmin, zmax)) {
					// -----------------
					//  prepare formula
					// -----------------
					for(int i = 0; i < sf_ds.surf_n; i++) {
						sf_ds.formulas[i].hpxyz->setColumn(ux); 
					}
					// ----------------------------------------------
					//  look for roots on the z-axis in the interval
					//  [zmin,zmax] with estimates 
					//  root_last_pixel[0] 
					//  ...
					//  root_last_pixel[root_last_pixel[n-1]
					// -----------------------------------------------
					sf_ds.root_n_all=0;			
					for(int i = 0; i < sf_ds.surf_n; i++) {  
						sf_ds.surfaces[i].root_n = zero_of_polyx(
							&sf_ds.formulas[i].hpxyz->pZ,
							zmax, zmin, root[i],
							root_last_pixel[i],
							root_last_pixel_n[i], clipper);
						sf_ds.root_n_all += sf_ds.surfaces[i].root_n;
						root_last_pixel_n[i] = sf_ds.surfaces[i].root_n;
					} 
					if(sf_ds.root_n_all) {
						// -----------------
						//  prepare formula
						// -----------------
						for(int i = 0; i < sf_ds.surf_n; i++) {
							sf_ds.formulas[i].hpDxyz->setColumn(ux);
							sf_ds.formulas[i].hpxDyz->setColumn(ux);
						}
						
						illumination_of_surface(ux, uy, root,
									root_last_pixel, stepwidth,
									pixel_color );
						
						intensity.SetPixelColor(px, py, pixel_color);
						intensity.SetTag(px, py, SURFBIT | DATABIT);
					} else {
						// -----------------------------------------
						//  no surface point is background, 
						// -----------------------------------------
						set_null_root_n(root_last_pixel_n);
						
						if(!ScriptVar::color_do_background_data) {
							pixel_color = backgr;
							
							intensity.SetPixelColor(px, py, pixel_color);
						}
					}
				} else {
					// --------------------------
					//  Invisible point is background, 
					// --------------------------
					set_null_root_n(root_last_pixel_n);
					
					if(!ScriptVar::color_do_background_data) {
						pixel_color = backgr;
						
						intensity.Set(px, py,
							      int(rint(pixel_color.red*255.0)),
							      int(rint(pixel_color.green*255.0)),
							      int(rint(pixel_color.blue*255.0)));
					}
				}
			}
		} else {
			// -----------------------------------------
			//  invisible line is background, 
			// -----------------------------------------
			
			if(!ScriptVar::color_do_background_data)  {
				for(int px = 0; px < width; px++) {
					pixel_color = backgr;
					
					intensity.Set(px, py,
						      int(rint(pixel_color.red  *255.0)),
						      int(rint(pixel_color.green*255.0)),
						      int(rint(pixel_color.blue *255.0)) );
				}
			}

		}

		bytes_written += 3*width;
		Misc::progress(bytes_written*100/image_length);
	}

	Misc::progress(Misc::done);

	// ----------------
	//  refine picture
	// ----------------
	
	if(ScriptVar::display_ref_level_data > 1) {
		std::string action = "Antialiasing";
		Misc::progress(action);
		bytes_written = 0;

		colorrgb i1, i2, i3, i4, i5, i6, i7, i8, i9;  		
		for(int py = 0; py < height; py++) {

			if(Script::isStopped()) {
				Misc::progress(Misc::aborted);
				return;
			}
			
			// --------------------------------------
			//  read intensities of neighbour pixels
			// --------------------------------------
			
			intensity.GetPixelColor(-1, py - 1, i1);
			intensity.GetPixelColor( 0, py - 1, i2);
			intensity.GetPixelColor( 1, py - 1, i3);

			intensity.GetPixelColor(-1, py, i4);
			intensity.GetPixelColor( 0, py, i5);
			intensity.GetPixelColor( 1, py, i6);

			intensity.GetPixelColor(-1, py + 1, i7);
			intensity.GetPixelColor( 0, py + 1, i8);
			intensity.GetPixelColor( 1, py + 1, i9);
			
			int parity = py % 2;

			for(int px = (parity ? 0 : width - 1);
			    (parity ? px < width : px > -1);
			    px += (parity ? 1 : -1) ) {
				if(needsRefining(i1, i2, i3, i4, i5, i6, i7, i8, i9)) {
					surface_refine_pixel(px, py, i5);
					intensity.SetPixelColor(px, py, i5);
				}

                                if(parity) {
				        i1 = i2;
				        i2 = i3;
				        intensity.GetPixelColor(px + 1, py - 1, i3);
				        i4 = i5;
				        i5 = i6;
				        intensity.GetPixelColor(px + 1, py, i6);
				        i7 = i8;
				        i8 = i9;
				        intensity.GetPixelColor(px + 1, py + 1, i9);
				} else {
				        i3=i2;
				        i2=i1;
				        intensity.GetPixelColor (px - 1, py - 1, i1);
				        i6=i5;
				        i5=i4;
				        intensity.GetPixelColor (px - 1, py, i4);
				        i9=i8;
				        i8=i7;
				        intensity.GetPixelColor (px - 1, py + 1, i7);
				}
			}

			bytes_written += 3*width;
			Misc::progress(bytes_written*100/image_length);
		}

		Misc::progress(Misc::done);
	}
}


// ---------------------------------------------
//  P = point on the surface
//  N = unit normal vector of the surface at P
//  obj_color = one of {SURFS,INSIDE}
//  
//  returns I = color value of the surface at P
// ---------------------------------------------

void SurfaceCalc::color_of_single_point(Vector& P, Vector& N, colorrgb& I, int obj_color)
{
        // --------------
	//  depth cueing
	// --------------
        float fade = 1.0;

	if(!ScriptVar::display_depth_cue_data) {
	        fade = 1.3*(P.z - ScriptVar::display_depth_value_data)/(cf - cb);

                if(fade <= 0.0) {
 			I.setBlack();
                        return;
		}
                else {
			fade = MIN(fade, 1.0);
		}
	}


	// ------------------------------
	//  begin with color calculation
	// ------------------------------
    	Vector V;
	Vector L;
	Vector H;
        double N_dot_L;
	double H_dot_N_pow;

        // -------------------
	//  set ambient light
	// -------------------
        if(flag_ambient) {
		I.set(ambient[RED][obj_color],
                      ambient[GREEN][obj_color],
                      ambient[BLUE][obj_color]);
        } else {
		I.setBlack();
        }

        // ----------------------------------------------------------
	//  if N.length( )<=1e-18 we have probably hit a singularity
	// ----------------------------------------------------------
	if(N.Length() > 1e-18) {
		// -----------------------
                //  compute viewer vector
		// -----------------------
		V = POS - P;
		V.Unify();

		// ---------------------------------
		//  cycle through all light sources
		// --------------------------------- 
		for(int i = 0; i < numLights; i++) {
		        // ----------------------
                        //  compute light vector
		        // ----------------------
			L = LPOS[i] - P;
			L.Unify();
 
          		// -------------------
		        //  set diffuse light
		        // -------------------
			N_dot_L = DotProduct(N, L);

			if(flag_diffuse && (N_dot_L > 0.0)) {
				I.add(licht[i].red*diffuse[RED][obj_color]*N_dot_L,
				      licht[i].green*diffuse[GREEN][obj_color]*N_dot_L,
				      licht[i].blue*diffuse[BLUE][obj_color]*N_dot_L);
			}

          		// -------------------
		        //  set reflected light
		        // -------------------
			H = L + V;
			H.Unify();
			H_dot_N_pow = pow(DotProduct(H, N), smoothness);

			if(flag_reflected && (N_dot_L > 0.0) && (H_dot_N_pow > 0.0)) {
				I.add(licht[i].red*reflected*H_dot_N_pow,
                                      licht[i].green*reflected*H_dot_N_pow,
                                      licht[i].blue*reflected*H_dot_N_pow);
			}
		}
	}

        I *= fade;
}
 
// ===========================================================================
// (sk :)Compute the intensity at wx,wy     
//  sk1: neuer struct für surface-datas
//  sk2: unterscheide ob 'N' zum oder weg vom Betrachter ...
//       zeigt, mit Flächenfarbe
//  sk3: multisurfaces     
// =========================================================================== 

void SurfaceCalc::illumination_of_surface(double wx, double wy,           
					  double (*root)[MAX_DEGREE],
					  double (*root_old)[MAX_DEGREE],    
					  double stepwidth,
					  colorrgb& color_intens)
{
	// FIXME
	static int inside; // true if normal points into monitor

	Vector P; // our point on the surface
	Vector N; // the surface normal in P

	float norm_it = ScriptVar::display_max_i_data;
	double transfak;
	double solidfak;

	transmission = 1.0;
    
	color_intens.setBlack();
	tmpcolor.setBlack();
   
	int root_count = 0;
	int best_i = -1;
	int root_level[MAIN_SURFACE_AMOUNT_NUM];
	
	for(int i = 0; i <= sf_ds.surf_n; i++) {
		root_level[i] = 0;
	}

	P.x = wx;
	P.y = wy;

	// -------------------------------------
	//  go through all roots of all surface
	// -------------------------------------
	do {
		int i = (best_i + 1)%(sf_ds.surf_n);
		sf_ds.prev_root = -10000.0;
        
		for(int k = 0; k < sf_ds.surf_n; k++) {        
			if(root_level[i] < sf_ds.surfaces[i].root_n 	
			   && root[i][root_level[i]] >= sf_ds.prev_root) {  
				sf_ds.prev_root=root[i][root_level[i]];
				best_i = i;
			}
			i =(i + 1)%(sf_ds.surf_n);
		}          
		
		//  best_i = number of surface
		//  root_level[best_i] = number of root of surface best_i

		// --------------------------
		//  compute point of surface
		// --------------------------
		P.z = root[best_i][root_level[best_i]];

		// ---------------------------
		//  compute normal of surface
		// ---------------------------
		N.x = sf_ds.formulas[best_i].hpDxyz->pZ.horner(root[best_i][root_level[best_i]]);
		N.y = sf_ds.formulas[best_i].hpxDyz->pZ.horner(root[best_i][root_level[best_i]]);
		N.z = sf_ds.formulas[best_i].hpxyz->pZ.dx_horner(root[best_i][root_level[best_i]]);
		N.Unify();

		root_old[best_i][root_level[best_i]] =
			root[best_i][root_level[best_i]] - 0.001;
        
		if(N.z != 0.0) {
			root_old[best_i][root_level[best_i]] += stepwidth*N.x/N.z;
		}

		root_level[best_i]++;
		root_count++;

		// -------------------------------
		//  make normal vector look to me
		// -------------------------------
		if(N.z < 0.0) {
			inside = 1;
			N.Negate();
		} else {
     		        inside = 0;
		}

 
		// ------------------------------------
		//  set properties for current surface
		// ------------------------------------
                //  surface transparence
                transparence = (flag_transmitted ? sf_ds.surfaces[best_i].transp : 0.0);

		if(ScriptVar::light_settings[best_i].thickness > 0) {
		        if(N.z > 0) {
				double tmp = ScriptVar::light_settings[best_i].thickness/(30.0*N.z);
			     	transfak = pow(transparence, MIN(tmp, 4.0));
			} else {
				transfak = pow(transparence, 4.0);
			}
	        } else {
			transfak = transparence;
		}

		solidfak = 1.0 - transfak;

                //  reflected, transmitted, smoothness
		reflected    = ( solidfak==0.0 ? 0.0 : sf_ds.surfaces[best_i].reflect );
		transmitted  = ( flag_transmitted ? sf_ds.surfaces[best_i].transmit : 0.0 );
		smoothness   = sf_ds.surfaces[best_i].smooth; 
	        part_D = sf_ds.surfaces[best_i].diffuse;

		// --------------------------------------------
		//  if we are on a curve take curve properties
		// --------------------------------------------
		//  better would be: take the nearest point on the surface ...
		if( sf_ds.curve.root_n && ( fabs( root[sf_ds.surf_n][0] - P.z ) < 0.01 ) ) {
		        //  the curve os not opaque
                        transfak      = 0.0;
                        solidfak      = 1.0;

		        ambient[RED]  [SURFS] = ambient[RED]  [INSIDE] = part_A*sf_ds.curve.red;
		        ambient[GREEN][SURFS] = ambient[GREEN][INSIDE] = part_A*sf_ds.curve.green;
		        ambient[BLUE] [SURFS] = ambient[BLUE] [INSIDE] = part_A*sf_ds.curve.blue;

		        diffuse[RED]  [SURFS] = diffuse[RED]  [INSIDE] = part_D*sf_ds.curve.red;
		        diffuse[GREEN][SURFS] = diffuse[GREEN][INSIDE] = part_D*sf_ds.curve.green;
		        diffuse[BLUE] [SURFS] = diffuse[BLUE] [INSIDE] = part_D*sf_ds.curve.blue;
		} else {
			ambient[RED]  [SURFS]  = sf_ds.surfaces[best_i].red      *part_A*solidfak;
			ambient[GREEN][SURFS]  = sf_ds.surfaces[best_i].green    *part_A*solidfak;
			ambient[BLUE] [SURFS]  = sf_ds.surfaces[best_i].blue     *part_A*solidfak;
			ambient[RED]  [INSIDE] = sf_ds.surfaces[best_i].ins_red  *part_A*solidfak;
			ambient[GREEN][INSIDE] = sf_ds.surfaces[best_i].ins_green*part_A*solidfak;
			ambient[BLUE] [INSIDE] = sf_ds.surfaces[best_i].ins_blue *part_A*solidfak;
			
			diffuse[RED]  [SURFS]  = sf_ds.surfaces[best_i].red      *part_D*solidfak;
			diffuse[GREEN][SURFS]  = sf_ds.surfaces[best_i].green    *part_D*solidfak;
			diffuse[BLUE] [SURFS]  = sf_ds.surfaces[best_i].blue     *part_D*solidfak;
			diffuse[RED]  [INSIDE] = sf_ds.surfaces[best_i].ins_red  *part_D*solidfak;
			diffuse[GREEN][INSIDE] = sf_ds.surfaces[best_i].ins_green*part_D*solidfak;
			diffuse[BLUE] [INSIDE] = sf_ds.surfaces[best_i].ins_blue *part_D*solidfak;
		}
         
		// ---------------
		//  Get intensity
		// ---------------
		color_of_single_point( P,N,tmpcolor,SURFS+inside ); 
        
		tmpcolor     *= transmission;
		color_intens += tmpcolor;
		transmission *= transfak;

	} while( root_count < sf_ds.root_n_all && flag_transmitted && transmission > 0.0 );

	if(flag_transmitted && !ScriptVar::color_do_background_data && transmission > 0.0) {
		tmpcolor =  backgr;
		tmpcolor *= transmission;
		color_intens += tmpcolor;
	}

	// ---------------------
	//  normalize intensity
	// ---------------------
	if(ScriptVar::display_normalize_data == 0) { 
		color_intens *= norm_it;
	}

	// --------------------------------    
	//  get max of all calculated pixels,
	//  to set it after all in the display-menu      
	// --------------------------------
	if(color_intens.red > pixel_max_i) {
		pixel_max_i = color_intens.red;
	}
	if(color_intens.green > pixel_max_i) {
		pixel_max_i = color_intens.green;
	}
	if(color_intens.blue  > pixel_max_i) {
		pixel_max_i = color_intens.blue;
	}

	// ----------------------------
	//  cut all intensities at 1.0  
	// ----------------------------
	color_intens.cutAtMax();
}

// ----------------------------------------------------------------------------
// sk : CalculateCurveOnSurface, ...
//      berechnet die Fläche an den Stellen, die im layer2 != 0 sind , ...
//      also dort wo die Kurve auf der Fläche liegt . 
// ----------------------------------------------------------------------------

void SurfaceCalc::CalculateCurveOnSurface(int xmin, int ymin, int xmax, int ymax,
					  RgbBuffer& intensity,
					  float_buffer& z_buffer)
{
	sf_ds.initCurveProperty();

	//  Min / Max Z-Axis    
	double zmin;
	double zmax;
	
	//  Save one Pixel-Color  
	colorrgb pixel_color, pixel_old;     					
	
	pixel_max_i = 0.0;  
	
	// Root variables 
	double root[MAIN_SURFACE_AMOUNT_NUM+1][MAX_DEGREE];       	   
	double root_last_pixel[MAIN_SURFACE_AMOUNT_NUM+1][MAX_DEGREE];
	int root_last_pixel_n[MAIN_SURFACE_AMOUNT_NUM+1];

        float distf = ScriptVar::display_numeric.stereo_z*ScriptVar::display_numeric.stereo_eye/
		      ScriptVar::position_numeric.spectator_z;
	int dist = int(distf*float(MIN(ScriptVar::main_width_data, ScriptVar::main_height_data))/20.0);

	initVars();
	
	// Does this make sense ??? FIXME
	// sk: get thickness of all surfaces
	thickness =  ScriptVar::light_settings[0].thickness;
	if(thickness) {
		thickness = ScriptVar::LIGHT_SETTINGS_SECOND_MAX_VALUE[LIGHT_THICKNESS] + 1 - thickness;
	}
      
	double stepwidth = pixel_to_user_x(1) - pixel_to_user_x(2);

	//  Cycle through all lines of the picture:
	for(int py = ymin; py < ymax; py++) {

		//  Get screen y-coordinate:
		double uy = pixel_to_user_y(py);

		//  Visibility test if row contains curve points
		if(clipper->clip_user_y(uy) && intensity.GetTag(0, py, CURVELINEBIT)) {

			//  Prepare formula:
			for(int i = 0; i < sf_ds.surf_n; i++) {  
				sf_ds.formulas[i].hpxyz->setRow(uy);
				sf_ds.formulas[i].hpDxyz->setRow(uy);
				sf_ds.formulas[i].hpxDyz->setRow(uy);
			}

			//  At first point of line assume that
			//  no root has been found before:
			set_null_root_n_with_curve(root_last_pixel_n);
			
			//  Cycle through all points of a line:
			for(int px = xmin; px < xmax; px++) {
				
				//  Get screen x-coordinate:
				double ux = pixel_to_user_x(px);

				//  Visibility test if its a curve point:
				if(clipper->clip_user_xy(ux, uy, zmin, zmax) 
				   && intensity.GetTag(px, py, CURVEBIT)) {

					//  Prepare formulas:
					for(int i = 0; i < sf_ds.surf_n; i++) {  
						sf_ds.formulas[i].hpxyz->setColumn(ux);
					} 

					// --------------------------------------------------
					//  Look for roots on the z-axis in the interval
					//  [zmin,zmax] with estimates 
					//  root_last_pixel[0] 
					//  ...
					//  root_last_pixel[root_last_pixel_n-1]
					// --------------------------------------------------
					sf_ds.root_n_all = 0;
					
					for(int i = 0; i < sf_ds.surf_n; i++) {  
						sf_ds.surfaces[i].root_n = zero_of_polyx(
							&sf_ds.formulas[i].hpxyz->pZ,	
							zmax, zmin, root[i],			
							root_last_pixel[i],
							root_last_pixel_n[i], clipper);
						sf_ds.root_n_all += sf_ds.surfaces[i].root_n;
						root_last_pixel_n[i] = sf_ds.surfaces[i].root_n;
					}

					root_last_pixel_n[sf_ds.surf_n] = sf_ds.curve.root_n = 1;
					root[sf_ds.surf_n][0] = z_buffer.Get(px, py);
					
					if(sf_ds.root_n_all) {

						//  Prepare formula:
						for(int i=0; i<sf_ds.surf_n; i++ ) {
							sf_ds.formulas[i].hpDxyz->setColumn(ux);
							sf_ds.formulas[i].hpxDyz->setColumn(ux);
						}

						//  Mix surface & curve intensities:
						illumination_of_surface(ux, uy, root,root_last_pixel, stepwidth,
                                                                        pixel_color);
                        
                                                double lambda = double(intensity.GetLayerTwo(px, py))/255.0;

                                                if(!ScriptVar::display_numeric.stereo_eye) {
                                                        pixel_color *= lambda;
                                                        intensity.GetPixelColor(px, py, pixel_old);
                                                        pixel_old *= 1.0 - double(intensity.GetLayerTwo(px, py))/255.0;
                                                        pixel_color += pixel_old;

						        //  Draw into memory set pixel to memory:
						        intensity.SetPixelColor(px, py, pixel_color);
							intensity.SetTag(px, py, DATABIT | SURFBIT);
							
						} else if(Y_AXIS_LR_ROTATE != 0.0) {
                                                        int pxdist = px + dist;

						        if(pxdist >= 0 && pxdist < xmax) {
                                                                intensity.GetPixelColor(pxdist, py, pixel_old);

                                                                double tmp = pixel_color.getBW()*lambda;
                                                                pixel_color.red = intensity.GetPixelColorR(pxdist, py);
                                                                pixel_color.green = pixel_old.green*( 1.0 - lambda) +
                                                                                    tmp*ScriptVar::display_numeric.stereo_green;
                                                                pixel_color.blue = pixel_old.blue*(1.0 - lambda ) +
                                                                                    tmp*ScriptVar::display_numeric.stereo_blue;

							        intensity.SetPixelColor(pxdist, py, pixel_color);
							        intensity.SetTag(pxdist, py, DATABIT3D | SURFBIT3D);
						        }
						} else {
                                                        int pxdist = px - dist;

						        if(pxdist >= 0 && pxdist < xmax) {
                                                                intensity.GetPixelColor(pxdist, py, pixel_old);

                                                                pixel_color.red = pixel_color.getBW()*lambda
									          *ScriptVar::display_numeric.stereo_red +
                                                                                  pixel_old.red*(1.0 - lambda );
                                                                pixel_color.green = intensity.GetPixelColorG(pxdist, py);
                                                                pixel_color.blue = intensity.GetPixelColorB(pxdist, py);

							        intensity.SetPixelColor(pxdist, py, pixel_color);
							        intensity.SetTag(pxdist, py, DATABIT | SURFBIT);
						        }
						}
						intensity.DelTag(0, py, CURVELINEBIT); 
						intensity.DelTag(px, py, CURVEBIT);

					} else { // end of 'Find root'

						// ---------------------------
						//  No surface point is black   OR ... 
						// ---------------------------
						
						set_null_root_n_with_curve(root_last_pixel_n);
					}
				} else { //end of 'Inside xy-Clip'
					// --------------------------
					//  Invisible point is black   OR  ...
					// --------------------------
					set_null_root_n_with_curve(root_last_pixel_n);
				}

			}// end of 'For each point in line'
			
		}// end of 'Inside y-clip'


	}// end of 'Foreach line' 
}

void SurfaceCalc::initVars()
{
	// --------------------
	//  spectator position
	// --------------------
	POS.x = 0.0;
	POS.y = 0.0;
	POS.z = ScriptVar::position_numeric.spectator_z;
	
	// ---------------
	//  light sources
	// ---------------
	for(size_t i = 0; i < LIGHT_SOURCE_MAX_VALUE; i++) {
		//  position
		LPOS[i].x = ScriptVar::light_data[i].position[LIGHT_POSITION_X];
		LPOS[i].y = ScriptVar::light_data[i].position[LIGHT_POSITION_Y];
		LPOS[i].z = ScriptVar::light_data[i].position[LIGHT_POSITION_Z];
		
		if(ScriptVar::display_numeric.stereo_eye) {
			// change position for 3D image
			LPOS[i].x = cos(Y_AXIS_LR_ROTATE)*LPOS[i].x - sin(Y_AXIS_LR_ROTATE)*LPOS[i].z;
			LPOS[i].z = sin(Y_AXIS_LR_ROTATE)*LPOS[i].x + cos(Y_AXIS_LR_ROTATE)*LPOS[i].z;
		}

 		// color
 		licht[i].red   = ScriptVar::light_data[i].getColorValue(0);
 		licht[i].green = ScriptVar::light_data[i].getColorValue(1);
 		licht[i].blue  = ScriptVar::light_data[i].getColorValue(2);
	}
	packLights(); 
	
	// ------------------------
	//  illumiation components
	// ------------------------
	flag_ambient     = ScriptVar::light_illumination_data & ScriptVar::light_illumination_ambient_data;
	flag_diffuse     = ScriptVar::light_illumination_data & ScriptVar::light_illumination_diffuse_data;
	flag_reflected   = ScriptVar::light_illumination_data & ScriptVar::light_illumination_reflected_data;
	flag_transmitted = ScriptVar::light_illumination_data & ScriptVar::light_illumination_transmitted_data;
	
	numeric_multi_root = flag_transmitted;
	
	part_A = float(ScriptVar::light_settings[0].ambient)/100.0;
		
	// fade color
	tocolor[RED]   = (double(ScriptVar::color_gradient_end_data[RED])/255.0)*part_A;
	tocolor[GREEN] = (double(ScriptVar::color_gradient_end_data[GREEN])/255.0)*part_A;
	tocolor[BLUE]  = (double(ScriptVar::color_gradient_end_data[BLUE])/255.0)*part_A;
	
	// set background color     
	backgr.set(ScriptVar::color_background_data[RED]  /255.0,
		   ScriptVar::color_background_data[GREEN]/255.0,
		   ScriptVar::color_background_data[BLUE] /255.0);
}
