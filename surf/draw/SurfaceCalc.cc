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
 * Johannes Gutenberg-Universitaet Mainz, Fachbereich 17 Mathematik,
 * hereby disclaims all copyright interest in the program `surf'
 * (which makes passes at compilers) written by Stephan Endrass,
 * Hans Huelf, Ruediger Oertel, Kai Schneider, Ralf Schmitt and
 * Johannes Beigel
 *
 */

#include <math.h>
#include <string.h>

#include "debug.h"

#include "SurfaceCalc.h"
#include "RgbBuffer.h"
#include "degree.h"

#include "color.h"
#include "roots.h"
#include "gui_enum.h"

#include "TSDrawingArea.h"
#include "compfn.h"

#include "gui_config.h"
#include "Thread.h"
#include "float_buffer.h"
#include "antialiasing_factor.h"

// FIXME

extern double Y_AXIS_LR_ROTATE;

#define SETNULL(x) memset(x, 0, sizeof(x))

static inline void set_null_root_n (int roots[MAIN_SURFACE_AMOUNT_NUM])
{
	memset (roots, 0, sizeof(roots));
}

static inline void set_null_root_n_with_curve (int roots[MAIN_SURFACE_AMOUNT_NUM+1])
{
	memset (roots, 0, sizeof(roots));
}

void SurfaceCalc::packLights()
{
        numLights = 0;

	for( int i=0; i<LIGHT_SOURCE_MAX_VALUE; i++ ) {
		if( licht[i].red>0 || licht[i].green>0 || licht[i].blue>0 ) {
			if (numLights != i) {
				LPOS [numLights]=LPOS [i];
				licht[numLights]=licht[i];
			}
			numLights++;
		}
	}
}

void  SurfaceCalc::surface_refine_pixel( int px_refine,int py_refine,
					 colorrgb& pixel_color_refine )
{
	// -------------------
	//  pixel coordinates 
	// -------------------
	double  px;
	double  py;
	double  px_start;
	double  py_start;
	double  p_inc;
	
	int     i,j,k,root_n;
	int     px_count;
	int     py_count;
	
	// ------------------
	//  user coordinates 
	// ------------------
	double  ux;
	double  uy;

	double  zmin;
	double  zmax;
	double  stepwidth;
	
	
	colorrgb pixel_tmp;      				
	pixel_tmp.setBlack();
	
	pixel_color_refine.setBlack();
	

	double  root[MAIN_SURFACE_AMOUNT_NUM][MAX_DEGREE];       	
	double  root_last_pixel[MAIN_SURFACE_AMOUNT_NUM][MAX_DEGREE];
	int     root_last_pixel_n[MAIN_SURFACE_AMOUNT_NUM];	
	
	float refine_radius=display_ref_radius_data;     
	
	// sk3:get refine px_start & py_start & p_inc & stepwidth with jump, refine_radius
	p_inc = ( refine_radius*jump )/display_ref_level_data;  
	px_start = px_refine - ((refine_radius/2.0)*jump) + 0.5*p_inc;  
	py_start = py_refine - ((refine_radius/2.0)*jump) + 0.5*p_inc;  
	stepwidth = fabs( pixel_to_user_x(px_start)-pixel_to_user_x (px_start + p_inc));
	
	// --------------------------------------------------------------------
	//  Cycle through all lines of the pixel 
	// --------------------------------------------------------------------
	for( py=py_start, py_count=0; py_count<display_ref_level_data && !Thread::shouldStop(); py_count++, py+=p_inc ) {

		uy = pixel_to_user_y( py );
		
		if (clipper->clip_user_y(uy)) {
			// -----------------
			//  Prepare formula
			// -----------------
			for(i=0;i<sf_ds.surf_n;i++) {  
				sf_ds.formulas[i].hpxyz->setRow (uy);  
				sf_ds.formulas[i].hpDxyz->setRow (uy);  
				sf_ds.formulas[i].hpxDyz->setRow (uy);  
			} 
			// ----------------------------------------------------------------
			// At first point of line assume that no root has been found before
			// ----------------------------------------------------------------
			set_null_root_n(root_last_pixel_n);
			
			// ----------------------------------------------------------------
			//  Cycle through all points of a line 
			// ----------------------------------------------------------------
			for( px = px_start, px_count = 0;
			     px_count < display_ref_level_data;
			     px_count++, px += p_inc ) {
				// -------------------------
				//  Get screen x-coordinate
				// -------------------------
				ux = pixel_to_user_x( px );
				
				if (clipper->clip_user_xy(ux,uy,zmin,zmax)) {
					// -----------------
					//  Prepare formula
					// -----------------

					for(i=0;i<sf_ds.surf_n;i++) {          
						sf_ds.formulas[i].hpxyz->setColumn (ux);
					}  
					// ----------------------------------------------------
					//  Look for roots on the z-axis in the interval
					//  [zmin,zmax] with estimates 
					//  root_last_pixel[0] 
					//  ...
					//  root_last_pixel[root_last_pixel_n-1]
					// ----------------------------------------------------
					sf_ds.root_n_all=0;				

					for(i=0;i<sf_ds.surf_n;i++) {  
						sf_ds.surfaces[i].root_n = 	
							zero_of_polyx( &sf_ds.formulas[i].hpxyz->pZ,
								       zmax,zmin,root[i],			
								       root_last_pixel[i],root_last_pixel_n[i],
                                                                       clipper );
						sf_ds.root_n_all += sf_ds.surfaces[i].root_n;
						root_last_pixel_n[i] = sf_ds.surfaces[i].root_n;     
					} 
					if (sf_ds.root_n_all) {
						
						// -----------------
						//  Prepare formula
						// -----------------

						for(i=0;i<sf_ds.surf_n;i++) {  
							sf_ds.formulas[i].hpDxyz->setColumn (ux);
							sf_ds.formulas[i].hpxDyz->setColumn (ux);
						} 

						
						illumination_of_surface(ux, uy, root, root_last_pixel,
									stepwidth,
									pixel_tmp );

						pixel_tmp *= antialiasing_factor(px_count, py_count, display_ref_level_data);	
						
						pixel_color_refine += pixel_tmp;
						
					} else {
						// ---------------------------------
						//  No surface point is background, 
						// ---------------------------------
						set_null_root_n(root_last_pixel_n);
						
						if( !color_do_background_data ) {
							pixel_tmp=backgr;
							pixel_tmp *= antialiasing_factor(px_count, py_count, display_ref_level_data);
							pixel_color_refine += pixel_tmp;
						}
					}
				} else {
					// --------------------------
					//  Invisible point is background, 
					// --------------------------
					set_null_root_n(root_last_pixel_n);
					
					if ( !color_do_background_data ) {
						pixel_tmp=backgr;
						pixel_tmp *= antialiasing_factor(px_count, py_count, display_ref_level_data);
						pixel_color_refine+=pixel_tmp;
					}
					
				}
				
			} // end , foreach Pixel
			
		} else {
			// -------------------------------------
			//  Invisible line is background, don't 
			// -------------------------------------
			if( !color_do_background_data ) {
				for( px=px_start, px_count=0;
				     px_count<display_ref_level_data;
				     px_count++, px += p_inc ) {
					pixel_tmp=backgr;
					pixel_tmp *= antialiasing_factor(px_count, py_count, display_ref_level_data);	  
					pixel_color_refine += pixel_tmp;
				}
			}
		}		
	} 
}


SurfaceCalc::SurfaceCalc()
{
	// memset (&sf_ds, 0, sizeof(sf_ds));
	vdisplay=0;

	main_factor_set();
 	cb = -10.0;
 	cf = 10.0;
	
	// clipper = new Clipper();
	clipper = NewClip::createSimpleClip (position_perspective_data, clip_data);
	clipper->init();

	sf_ds.init();
}

SurfaceCalc::~SurfaceCalc()
{
}

inline int SurfaceCalc::needsRefining (const colorrgb &i1, const colorrgb &i2, const colorrgb &i3,
				       const colorrgb &i4, const colorrgb &i5, const colorrgb &i6,
				       const colorrgb &i7, const colorrgb &i8, const colorrgb &i9)
{
	
	return     fabs( i5.red  - i1.red  ) > display_ref_treshold_data 
		|| fabs( i5.green- i1.green) > display_ref_treshold_data 
		|| fabs( i5.blue - i1.blue ) > display_ref_treshold_data 
		|| fabs( i5.red  - i2.red  ) > display_ref_treshold_data 
		|| fabs( i5.green- i2.green) > display_ref_treshold_data 
		|| fabs( i5.blue - i2.blue ) > display_ref_treshold_data 
		|| fabs( i5.red  - i3.red  ) > display_ref_treshold_data 
		|| fabs( i5.green- i3.green) > display_ref_treshold_data 
		|| fabs( i5.blue - i3.blue ) > display_ref_treshold_data 
		|| fabs( i5.red  - i4.red  ) > display_ref_treshold_data 
		|| fabs( i5.green- i4.green) > display_ref_treshold_data 
		|| fabs( i5.blue - i4.blue ) > display_ref_treshold_data 
		|| fabs( i5.red  - i6.red  ) > display_ref_treshold_data 
		|| fabs( i5.green- i6.green) > display_ref_treshold_data 
		|| fabs( i5.blue - i6.blue ) > display_ref_treshold_data 
		|| fabs( i5.red  - i7.red  ) > display_ref_treshold_data 
		|| fabs( i5.green- i7.green) > display_ref_treshold_data 
		|| fabs( i5.blue - i7.blue ) > display_ref_treshold_data 
		|| fabs( i5.red  - i8.red  ) > display_ref_treshold_data 
		|| fabs( i5.green- i8.green) > display_ref_treshold_data 
		|| fabs( i5.blue - i8.blue ) > display_ref_treshold_data 
		|| fabs( i5.red  - i9.red  ) > display_ref_treshold_data 
		|| fabs( i5.green- i9.green) > display_ref_treshold_data 
		|| fabs( i5.blue - i9.blue ) > display_ref_treshold_data;
}


void    SurfaceCalc::surface_calculate( int xmin, int ymin, int xmax, int ymax,
					RgbBuffer 	 &intensity )
{
	BEGIN("SurfaceCalc::surface_calculate");
	Thread::setDoing ("drawing surface...");

	int     i,j,k,root_n;
    
	// ------------------------
	//  for  calc_this  matrix                         
	// ------------------------

	int     jump_row_index = 0, jump_col_index = 0;
	bool    calc_this[3][3]={ {true,true,true},
				  {true,true,true},
				  {true,true,true}};
	// -------------------
	//  pixel coordinates
	// -------------------
	int     px;
	int     py;
	
	// ------------------
	//  user coordinates
	// ------------------
	double  ux;
	double  uy;

	// -------------------
	//  min/max on z-axis    
	// -------------------
	double  zmin;
	double  zmax;
	double  stepwidth;
	
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
	
	stepwidth = pixel_to_user_x( 1 ) - pixel_to_user_x( 2 );
	
	
	// -------------------------
	//  render big blocks first		  	
	// -------------------------
	int ju = MAIN_MEM_LAST_MOSAIC_INDEX - 1;
	int startjump = 27;	
	for( jump=startjump; jump>0; jump=jump/3,ju-- ) {
		if (main_mosaic_choice_data[ju]) {
			jump_memory = jump;
			halfjump = ( jump - 1 )/2;

			// ----------------------------------------
			//  cycle through all lines of the picture
			// ----------------------------------------
			
			for( py = ymin + halfjump; py < ymax && !Thread::shouldStop() ; py += jump ) {
				// ------------
				//  clear line
				// ------------
#if 0
				XSetForeground( sci->display,sci->gc,sci->bg );
				XFillRectangle( sci->display,pixmap,sci->gc,xmin,
						py - halfjump,xmax - xmin,jump );
#endif
				// ---------------------------------------------
				//  calculate row position in matrix  calc_this
				// ---------------------------------------------
				jump_row_index = ((py-halfjump)/jump)%3;
				
				uy = pixel_to_user_y( py );
				
				if (clipper->clip_user_y(uy)) {
					// ------------------
					//  prepare surfaces
					// ------------------
					for( i = 0; i < sf_ds.surf_n; i++ ) { 
						sf_ds.formulas[i].hpxyz->setRow (uy);
						sf_ds.formulas[i].hpDxyz->setRow (uy);
						sf_ds.formulas[i].hpxDyz->setRow (uy);
					}

					// --------------------------------------------
					//  At first point of line assume that no root
					//  has been found before
					// --------------------------------------------
					set_null_root_n(root_last_pixel_n);
					
					// ------------------------------------
					//  cycle through all points of a line
					// ------------------------------------
					for( px = xmin + halfjump; px < xmax; px += jump ) {
						// -----------------------------------------------
						//  calculate column position in matrix calc_this
						// -----------------------------------------------
						jump_col_index=((px-halfjump)/jump)%3;         
						
						ux = pixel_to_user_x( px );
		    
						if (clipper->clip_user_xy(ux,uy,zmin,zmax)) {
							if( calc_this[jump_row_index][jump_col_index] ) {
								// -----------------
								//  prepare formula
								// -----------------
								for( i = 0; i < sf_ds.surf_n; i++ ) {
									sf_ds.formulas[i].hpxyz->setColumn( ux ); 
								}
								// ----------------------------------------------
								//  look for roots on the z-axis in the interval
								//  [zmin,zmax] with estimates 
								//  root_last_pixel[0] 
								//  ...
								//  root_last_pixel[root_last_pixel[n-1]
								// -----------------------------------------------
								sf_ds.root_n_all=0;			
								for( i=0; i < sf_ds.surf_n; i++ ) {  
									sf_ds.surfaces[i].root_n = zero_of_polyx(
										&sf_ds.formulas[i].hpxyz->pZ,
										zmax,zmin,root[i],
										root_last_pixel[i],
										root_last_pixel_n[i],clipper );
								    sf_ds.root_n_all+=sf_ds.surfaces[i].root_n;
								    root_last_pixel_n[i] = sf_ds.surfaces[i].root_n;
								} 
								if( sf_ds.root_n_all ) {
									// -----------------
									//  prepare formula
									// -----------------
									for( i = 0;i < sf_ds.surf_n; i++ ) {
										sf_ds.formulas[i].hpDxyz->setColumn (ux);
										sf_ds.formulas[i].hpxDyz->setColumn (ux);
									}

									illumination_of_surface(ux,uy,root,
												root_last_pixel,stepwidth, // sf_d,
												pixel_color );

									surface_set_pixel (px, py, pixel_color);

									intensity.SetPixelColor (px, py, pixel_color);
									intensity.SetTag( px,py,SURFBIT | DATABIT);
								} else {
									// -----------------------------------------
									//  no surface point is background, 
									// -----------------------------------------
									set_null_root_n(root_last_pixel_n);
									
									if( !color_do_background_data ) {
										pixel_color=backgr;
										
										surface_set_pixel( px, py, pixel_color);
										intensity.SetPixelColor(px,py, pixel_color);
									}
								}
							} else { 
								// ------------------------------------------
								//  color calculated before: Get from memory
								// ------------------------------------------ 
								intensity.GetPixelColor (px, py, pixel_color);
								surface_set_pixel( px, py, pixel_color);
								
							}
						} else {
							// --------------------------
							//  Invisible point is background, 
							// --------------------------
							set_null_root_n(root_last_pixel_n);
							
							if( !color_do_background_data ) {
								pixel_color=backgr;
								
								surface_set_pixel( px, py, pixel_color );

								intensity.Set( px, py,
									       (int)(rint(pixel_color.red  *255.0)),
									       (int)(rint(pixel_color.green*255.0)),
									       (int)(rint(pixel_color.blue *255.0)) );
							}
						}
					}
				} else {
					// -----------------------------------------
					//  invisible line is background, 
					// -----------------------------------------
					
					if ( !color_do_background_data )  {
						for( px = xmin+halfjump; px<xmax; px=px+jump ) {
							pixel_color = backgr;
							
							surface_set_pixel( px, py, pixel_color );
							
							intensity.Set( px,py,
								       (int)(rint(pixel_color.red  *255.0)),
								       (int)(rint(pixel_color.green*255.0)),
								       (int)(rint(pixel_color.blue *255.0)) );
						}
					}
				}
				
				// ----------------------
				//  map line onto screen
				// ----------------------
				if (vdisplay) {
					vdisplay->displayRectangle(xmin,py-halfjump,xmax-xmin,jump);
				}
			}
			
			// -----------------------------------------------
			//  delete central entry of calc_this for gext go
			// -----------------------------------------------
			calc_this[1][1]=false;
		}
	}
	
	if( display_normalize_data == 1 ) {
		// ---------------------------
		//  suggest a normalize value 
		// ---------------------------
		//         display_normalize_set_value( 1.0/pixel_max_i );
	}
    
	jump = jump_memory;
	halfjump=(jump-1)/2;    
	
	// ----------------
	//  refine picture
	// ----------------
	int   refine_count = 0;
	

	if( display_ref_level_data>1 ) {
                int parity;
		colorrgb  i1,i2,i3,i4,i5,i6,i7,i8,i9;  		
		Thread::setDoing ("antialiasing...");
		for( py = ymin + halfjump; py < ymax  && !Thread::shouldStop() ; py += jump ) {
			// --------------------------------------
			//  read intensities of neighbour pixels
			// --------------------------------------
			
			intensity.GetPixelColor(-jump,    py-jump, i1);
			intensity.GetPixelColor(halfjump, py-jump, i2);
			intensity.GetPixelColor( jump,    py-jump, i3);

			intensity.GetPixelColor(-jump,    py,      i4);
			intensity.GetPixelColor(halfjump, py,      i5);
			intensity.GetPixelColor(jump,     py,      i6);

			intensity.GetPixelColor(-jump,    py+jump, i7);
			intensity.GetPixelColor(halfjump, py+jump, i8);
			intensity.GetPixelColor(jump,     py+jump, i9);
			
			parity = py%2;
			for( px= ( parity ? xmin+halfjump : xmax-halfjump-1 );
                                 ( parity ? px<xmax       : px>xmin-1 );
                             px+=( parity ? jump : -jump ) ) {
				if (needsRefining(i1,i2,i3,i4,i5,i6,i7,i8,i9)) {
					
					surface_refine_pixel   ( px,py,i5 );
					surface_set_pixel      ( px,py,i5 );              
					intensity.SetPixelColor( px,py,i5 );
					
					refine_count++;
				}
                                if( parity ){
				        i1=i2;
				        i2=i3;
				        intensity.GetPixelColor (px+jump, py-jump, i3);
				        i4=i5;
				        i5=i6;
				        intensity.GetPixelColor (px+jump, py,      i6);
				        i7=i8;
				        i8=i9;
				        intensity.GetPixelColor (px+jump, py+jump, i9);
				}
                                else {
				        i3=i2;
				        i2=i1;
				        intensity.GetPixelColor (px-jump, py-jump, i1);
				        i6=i5;
				        i5=i4;
				        intensity.GetPixelColor (px-jump, py,      i4);
				        i9=i8;
				        i8=i7;
				        intensity.GetPixelColor (px-jump, py+jump, i7);
				}
			}

			if (vdisplay)
				vdisplay->displayRectangle(0, py-halfjump, main_width_data, jump);
			Thread::setDone (((double)py)/((double)main_height_data));
		}
	}
}




void  SurfaceCalc::color_of_single_point_old( Vector &P, Vector &N, colorrgb &I, int obj_color )
{

	Vector  V;
	Vector  L;
	Vector  H;

	register  float   fade    = 1.0;
	register  float   fadecol = 1.0, fadecoln = 0.0;
	
	register  int     i;

	float  cm = ( cb + cf )/2.0; 	      // midpoint of clipping area
	float  cd = ( cf - cb );                  // diameter of clipping area

	// -----------------------------------------
	//  color-gradient-fading
	//  choose gradient outside, inside or both
	// -----------------------------------------
	
	if( ( obj_color == color_gradient_data - 1 &&
	      color_gradient_data ) || color_gradient_data==3 ) {
		fadecol = ( P.z - cm )/cd + 0.5;
		fadecol = max( fadecol,0.0 );
		fadecol = min( fadecol,1.0 );
	} else {
		fadecol = 1.0;
	}
	fadecoln = 1.0 - fadecol;

	// ------------------------------
	//  begin with color calculation
	// ------------------------------
    
	if( N.Length() > 1e-8 ) {
		V = POS-P;		// VECTOR_SUBTRACT( V,POS,P );
		V.Unify(); // vector_self_unit( &V );
 
		// -------------------
		//  set ambient light
		// -------------------

		if( flag_ambient ) {
			I.set( ambient[RED]  [obj_color]*fadecol + tocolor[RED]  *fadecoln,
			       ambient[GREEN][obj_color]*fadecol + tocolor[GREEN]*fadecoln,
			       ambient[BLUE] [obj_color]*fadecol + tocolor[BLUE] *fadecoln );
		} else {
			I.setBlack(); // I.Set ( 0.0,0.0,0.0 );
		}

		// ---------------------------
		//  cycle through all sources
		// --------------------------- 

		for( i = 0; i<numLights; i++) {
			
			L = LPOS[i]-P; // VECTOR_SUBTRACT( L, LPOS[i], P );
 
			L.Unify(); // vector_self_unit( &L );
			H = L+V; // VECTOR_ADD( H,L,V );   				
				
			H.Unify(); // vector_self_unit( &H );
			// VECTOR_NEGATE( Hd,H );
			
			double N_dot_L  = DotProduct (N,L); // VECTOR_DOT_PRODUCT( N,L );
			double H_dot_L  = DotProduct (H,N); // VECTOR_DOT_PRODUCT( H,N );  		
			// double Hd_dot_L = VECTOR_DOT_PRODUCT( Hd,N ); 		
 
			double H_dot_L_pow  = pow( H_dot_L,smoothness );
			// double Hd_dot_L_pow = pow( Hd_dot_L,smoothness );
 
			int gets_diffuse     = (   N_dot_L  > 0.0 );
			int gets_reflected   = ( ( H_dot_L  > 0.0 ) && gets_diffuse );
			// int gets_transmitted = ( ( Hd_dot_L > 0.0 ) && gets_diffuse );

			double factor=part_D*fadecoln;
			double my_reflected = flag_reflected && gets_reflected ? reflected*H_dot_L_pow : 0.0;
			if (flag_diffuse && gets_diffuse) {
				I.add( licht[i].red  *(( (diffuse[RED  ][obj_color]*fadecol+tocolor[RED  ]*factor)*N_dot_L) + my_reflected),
				       licht[i].green*(( (diffuse[GREEN][obj_color]*fadecol+tocolor[GREEN]*factor)*N_dot_L) + my_reflected),
				       licht[i].blue *(( (diffuse[BLUE ][obj_color]*fadecol+tocolor[BLUE ]*factor)*N_dot_L) + my_reflected));
			} else {
				I.add( licht[i].red  * my_reflected,
				       licht[i].green  * my_reflected,
				       licht[i].red  * my_reflected);
			}
		}
        
		// --------------
		//  depth cueing
		// --------------

		if( !display_depth_cue_data ) {
			fade = 1.3*( P.z - display_depth_value_data )/cd;
			fade = max( fade,0.0 );
			fade = min( fade,1.0 );
			
			I *= fade;
		}
	} else {
		// ------------------------
		//  Probably a singularity
		// ------------------------
		
		I.setBlack();
     }
}

// ---------------------------------------------
//  new variant of that very function
//  P = point on the surface
//  N = unit normal vector of the surface at P
//  obj_color = one of {SURFS,INSIDE}
//  
//  returns I = color value of the surface at P
// ---------------------------------------------

void  SurfaceCalc::color_of_single_point( Vector &P, Vector &N, colorrgb &I, int obj_color )
{
        // --------------
	//  depth cueing
	// --------------
        float  fade = 1.0;

	if( !display_depth_cue_data ) {
	        fade = 1.3*( P.z - display_depth_value_data )/( cf - cb );

                if( fade <= 0.0 ) {
 			I.setBlack( );
                        return;
		}
                else {
			fade = min( fade,1.0 );
		}
	}


	// ------------------------------
	//  begin with color calculation
	// ------------------------------
    	Vector  V,L,H;
        double N_dot_L, H_dot_N_pow;

        // -------------------
	//  set ambient light
	// -------------------
        if( flag_ambient ) {
		I.set( ambient[RED]  [obj_color],
                       ambient[GREEN][obj_color],
                       ambient[BLUE] [obj_color] );
        } else {
		I.setBlack( );
        }

        // ----------------------------------------------------------
	//  if N.length( )<=1e-18 we have probably hit a singularity
	// ----------------------------------------------------------
	if( N.Length( ) > 1e-18 ) {
		// -----------------------
                //  compute viewer vector
		// -----------------------
		V = POS - P;
		V.Unify( );

		// ---------------------------------
		//  cycle through all light sources
		// --------------------------------- 
		for( int i=0; i<numLights; i++ ) {
		        // ----------------------
                        //  compute light vector
		        // ----------------------
			L = LPOS[i] - P;
			L.Unify( );
 
          		// -------------------
		        //  set diffuse light
		        // -------------------
			N_dot_L = DotProduct( N,L );

			if( flag_diffuse && ( N_dot_L > 0.0 ) ) {
				I.add( licht[i].red  *diffuse[RED  ][obj_color]*N_dot_L,
				       licht[i].green*diffuse[GREEN][obj_color]*N_dot_L,
				       licht[i].blue *diffuse[BLUE ][obj_color]*N_dot_L );
			}

          		// -------------------
		        //  set reflected light
		        // -------------------
			H = L + V;
			H.Unify( );
			H_dot_N_pow = pow( DotProduct( H,N ),smoothness );

			if( flag_reflected && ( N_dot_L > 0.0  ) && ( H_dot_N_pow > 0.0 ) ) {
				I.add( licht[i].red  *reflected*H_dot_N_pow,
                                       licht[i].green*reflected*H_dot_N_pow,
                                       licht[i].blue *reflected*H_dot_N_pow );
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

void SurfaceCalc::illumination_of_surface( double wx,double wy,           
					   double (*root)[MAX_DEGREE],
					   double (*root_old)[MAX_DEGREE],    
					   double stepwidth,
					   colorrgb& color_intens ) 		
{
	// FIXME
	static    int     inside;         // true if normal points into monitor

	Vector  P;              // our point on the surface
	Vector  N;              // the surface normal in P


    
	register  float   norm_it = display_max_i_data;
	double    transfak,solidfak;

	transmission = 1.0;
    
	color_intens.setBlack();
	tmpcolor.setBlack();
   
	int k;
	int i         = 0;
	int root_count= 0;
	int best_i    =-1;
	int root_level[MAIN_SURFACE_AMOUNT_NUM];
	
	for( i = 0; i <= sf_ds.surf_n; i++ ) {
		root_level[i]=0;
	}

	P.x = wx;
	P.y = wy;

	// -------------------------------------
	//  go through all roots of all surface
	// -------------------------------------
	do {
		i = (best_i+1)%(sf_ds.surf_n);
		sf_ds.prev_root=-10000.0;
        
		for( k = 0; k < sf_ds.surf_n; k++ ) {        
			if ( root_level[i] < sf_ds.surfaces[i].root_n 	
			     && root[i][ root_level[i] ] >= sf_ds.prev_root ) {  
				sf_ds.prev_root=root[i][ root_level[i] ];
				best_i=i;
			}
			i =(i+1)%(sf_ds.surf_n);
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
		N.x = sf_ds.formulas[best_i].hpDxyz->pZ.horner  ( root[best_i][root_level[best_i]] );
		N.y = sf_ds.formulas[best_i].hpxDyz->pZ.horner  ( root[best_i][root_level[best_i]] );
		N.z = sf_ds.formulas[best_i].hpxyz->pZ.dx_horner( root[best_i][root_level[best_i]] );
		N.Unify( );

		root_old[best_i][root_level[best_i]] =
			root[best_i][root_level[best_i]] - 0.001;
        
		if( N.z != 0.0 ) {
			root_old[best_i][root_level[best_i]] += stepwidth*N.x/N.z;
		}

		root_level[best_i]++;
		root_count++;

		// -------------------------------
		//  make normal vector look to me
		// -------------------------------
		if( N.z < 0.0 ) {
			inside = 1;
			N.Negate( );
		} else {
     		        inside = 0;
		}

 
		// ------------------------------------
		//  set properties for current surface
		// ------------------------------------
                //  surface transparence
                transparence = ( flag_transmitted ? sf_ds.surfaces[best_i].transp : 0.0 );

		if( light_settings[best_i].thickness > 0 ) {
		        if( N.z > 0 ) {
				double tmp = light_settings[best_i].thickness/(30.0*N.z);
			     	transfak = pow( transparence,min( tmp,4.0 ) );
			} else {
				transfak = pow( transparence,4.0 );
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

	} while( root_count < sf_ds.root_n_all && flag_transmitted && transmission>0.0 );

	if( flag_transmitted && !color_do_background_data && transmission>0.0 ) {
		tmpcolor     =  backgr;
		tmpcolor     *= transmission;
		color_intens += tmpcolor;
	}

	// ---------------------
	//  normalize intensity
	// ---------------------
	if( display_normalize_data == 0 ) { 
		color_intens *= norm_it;
	}

	// --------------------------------    
	//  get max of all calculated pixels,
	//  to set it after all in the display-menu      
	// --------------------------------
	if( color_intens.red   > pixel_max_i ) 
		pixel_max_i = color_intens.red;
	if( color_intens.green > pixel_max_i ) 
		pixel_max_i = color_intens.green;
	if( color_intens.blue  > pixel_max_i ) 
		pixel_max_i = color_intens.blue;

	// ----------------------------
	//  cut all intensities at 1.0  
	// ----------------------------
	color_intens.cutAtMax( );
}

// ----------------------------------------------------------------------------
// sk : CalculateCurveOnSurface, ...
//      berechnet die Fläche an den Stellen, die im layer2 != 0 sind , ...
//      also dort wo die Kurve auf der Fläche liegt . 
// ----------------------------------------------------------------------------

void SurfaceCalc::CalculateCurveOnSurface( int xmin, int ymin, int xmax, int ymax,
					   RgbBuffer 	        &intensity,
					   float_buffer         &z_buffer)
{
	Thread::setDoing("drawing curve on surface ...");
	sf_ds.initCurveProperty();

	jump=1;
	int NULL_ROOT_N_WITH_CURVE[MAIN_SURFACE_AMOUNT_NUM+1]={0,0,0,0,0,0,0,0,0,0};	/* sk69 neu*/

	int     i,j,k,root_n;
    
	// -------------------
	//  Pixel coordinates
	int     px;
	int     py;
	// ------------------
	//  User coordinates
	double  ux;
	double  uy;
	// ------------------
	//  Min / Max Z-Axis    
	double  zmin;
	double  zmax;
	double  stepwidth;
	
	// -----------------------
	//  Save one Pixel-Color  
	colorrgb  pixel_color, pixel_old;     					
	
	pixel_max_i = 0.0;  
	
	// --------------------------------------------------------------
	// Root - Variablen 
	double  root[MAIN_SURFACE_AMOUNT_NUM+1][MAX_DEGREE];       	   
	double  root_last_pixel[MAIN_SURFACE_AMOUNT_NUM+1][MAX_DEGREE];
	int     root_last_pixel_n[MAIN_SURFACE_AMOUNT_NUM+1];

        double  tmp;
        float   distf = display_numeric.stereo_z*display_numeric.stereo_eye/
			position_numeric.spectator_z;
	int     dist  = (int)(distf*((float)(min(main_width_data,main_height_data)))/20.0);
        int     pxdist;

	initVars();
	
// 	XFlush( sci->display );

	i = MAIN_SURFACE_AMOUNT_NUM;
	// Does this make sense ??? FIXME
	// sk: get thickness of all surfaces
	thickness    =          light_settings[i].thickness;  
	thickness    = (thickness) ?  LIGHT_SETTINGS_SECOND_MAX_VALUE[LIGHT_THICKNESS]+1 - thickness  : 0; 
      
	//intensity.DelTag(0,0,ALREADYDITHERED);

	stepwidth = pixel_to_user_x( 1 ) - pixel_to_user_x( 2 );

	// ----------------------------------------
	//  Cycle through all lines of the picture
	// ----------------------------------------
	for( py = ymin; py < ymax && !Thread::shouldStop(); py=py+1 ) {

		// -------------------------
		//  Get screen y-coordinate
		// -------------------------
		uy = pixel_to_user_y( py );

		// ----------------------------------------------
		//  Visibility test if row contains curve points
		// ----------------------------------------------

		if (clipper->clip_user_y(uy) && intensity.GetTag(0,py,CURVELINEBIT)) {
			// -----------------
			//  Prepare formula
			// -----------------
			for( i=0; i<sf_ds.surf_n; i++ ) {  
				sf_ds.formulas[i].hpxyz->setRow (uy);
				sf_ds.formulas[i].hpDxyz->setRow (uy);
				sf_ds.formulas[i].hpxDyz->setRow (uy);
			} 
			// ------------------------------------
			//  At first point of line assume that
			//  no root has been found before
			// ------------------------------------
			set_null_root_n_with_curve(root_last_pixel_n);
			

			// ------------------------------------
			//  Cycle through all points of a line
			// ------------------------------------

			for( px = xmin; px < xmax; px = px + 1 ) { 
				// -------------------------
				//  Get screen x-coordinate
				// -------------------------
				ux = pixel_to_user_x( px );

				// --------------------------------------
				//  Visibility test if its a curve point
				// --------------------------------------

				if (clipper->clip_user_xy(ux,uy, zmin, zmax) 
				    && intensity.GetTag( px,py,CURVEBIT )) {
					// ------------------
					//  Prepare formulas
					// ------------------
					
					for( i = 0; i < sf_ds.surf_n; i++ ) {  
						sf_ds.formulas[i].hpxyz->setColumn (ux); 
					} 

					// --------------------------------------------------
					//  Look for roots on the z-axis in the interval
					//  [zmin,zmax] with estimates 
					//  root_last_pixel[0] 
					//  ...
					//  root_last_pixel[root_last_pixel_n-1]
					// --------------------------------------------------
					sf_ds.root_n_all=0;
					
					for( i = 0; i < sf_ds.surf_n; i++ ) {  
						sf_ds.surfaces[i].root_n = zero_of_polyx(
							&sf_ds.formulas[i].hpxyz->pZ,	
							zmax,zmin,root[i],			
							root_last_pixel[i],
							root_last_pixel_n[i],clipper );
						sf_ds.root_n_all += sf_ds.surfaces[i].root_n; 	
						root_last_pixel_n[i] = sf_ds.surfaces[i].root_n;
					}

					root_last_pixel_n[sf_ds.surf_n]=sf_ds.curve.root_n=1;
					root[sf_ds.surf_n][0]=z_buffer.Get(px,py);
					
					if( sf_ds.root_n_all) {
						// -----------------
						//  Prepare formula
						// -----------------
						for( i=0; i<sf_ds.surf_n; i++ ) {  
							sf_ds.formulas[i].hpDxyz->setColumn (ux);
							sf_ds.formulas[i].hpxDyz->setColumn (ux);
						}

					        // ---------------------------------
						//  Mix surface & curve intensities
						// ---------------------------------
						illumination_of_surface( ux,uy,root,root_last_pixel,stepwidth,
                                                                         pixel_color );
                        
                                                double lambda = (double)intensity.GetLayerTwo(px,py)/255.0;

                                                if( !display_numeric.stereo_eye )
						{
                                                        pixel_color *= lambda;
                                                        intensity.GetPixelColor( px,py,pixel_old );
                                                        pixel_old   *= 1.0 - (double)intensity.GetLayerTwo(px,py)/255.0;
                                                        pixel_color += pixel_old;

						        // ------------------
						        //  Draw onto screen
						        // ------------------
						        surface_set_pixel (px, py, pixel_color);

						        // ------------------
						        //  Draw into memory set pixel to memory 
						        // ------------------
						        intensity.SetPixelColor (px, py, pixel_color);
							intensity.SetTag        (px, py, DATABIT | SURFBIT );
						}
                                                else if( Y_AXIS_LR_ROTATE != 0.0 )
						{
                                                        pxdist = px+dist;

						        if( pxdist >= 0 && pxdist < xmax )
						        {
                                                                intensity.GetPixelColor( pxdist,py,pixel_old );

                                                                tmp               = pixel_color.getBW( )*lambda;
                                                                pixel_color.red   = intensity.GetPixelColorR( pxdist,py );
                                                                pixel_color.green = pixel_old.green*( 1.0 -lambda ) +
                                                                                    tmp*display_numeric.stereo_green;
                                                                pixel_color.blue  = pixel_old.blue*( 1.0 -lambda ) +
                                                                                    tmp*display_numeric.stereo_blue;

                                                                surface_set_pixel      (pxdist, py, pixel_color);
							        intensity.SetPixelColor(pxdist, py, pixel_color);
							        intensity.SetTag       (pxdist, py, DATABIT3D | SURFBIT3D );
						        }
						}
                                                else
						{
                                                        pxdist = px-dist;

						        if( pxdist >= 0 && pxdist < xmax )
						        {
                                                                intensity.GetPixelColor( pxdist,py,pixel_old );

                                                                pixel_color.red   = pixel_color.getBW( )*lambda
                                                                                    *display_numeric.stereo_red +
                                                                                    pixel_old.red*(1.0 -lambda );
                                                                pixel_color.green = intensity.GetPixelColorG( pxdist,py );
                                                                pixel_color.blue  = intensity.GetPixelColorB( pxdist,py );

                                                                surface_set_pixel      (pxdist, py, pixel_color);
							        intensity.SetPixelColor(pxdist, py, pixel_color);
							        intensity.SetTag       (pxdist, py, DATABIT | SURFBIT );
						        }
						}
						intensity.DelTag( 0,py,CURVELINEBIT); 
						intensity.DelTag(px,py,CURVEBIT    );

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

		// ----------------------
		//  Map line onto screen
		// ----------------------
		if (vdisplay) 
			vdisplay->displayRectangle (xmin, py, xmax-xmin,1);
		//XCopyArea( sci->display,pixmap,win,sci->gc,
		//           xmin,py,           
		//           xmax - xmin,1,           
		//           xmin,py);           
		
        
	}// end of 'Foreach line' 

	if (vdisplay)
		vdisplay->displayRectangle (xmin, ymin, xmax-xmin, ymax-ymin);
	else
		cout << "no vdisplay..." << endl;
#if 0
	XCopyArea( sci->display,pixmap,win,sci->gc,
		   xmin,ymin,           
		   xmax - xmin,ymax - ymin,           
		   xmin,ymin );           
#endif

	// ----------------
	//  Small notifier
	// ----------------
	//XFlush( sci->display );
#if 0
	if ( display_normalize_data==1) { 
		main_frame_set_left_footer( "normalizing ..." );
		display_normalize_set_value( 1.0/pixel_max_i );
	}
#endif
    
}

void SurfaceCalc::initVars( )
{
	// --------------------
	//  spectator position
	// --------------------
	POS.x = 0.0;
	POS.y = 0.0;
	POS.z = position_numeric.spectator_z;
	
	// ---------------
	//  light sources
	// ---------------
	int i;

	for( i=0; i<LIGHT_SOURCE_MAX_VALUE; i++ ) {
		//  position
		LPOS[i].x = light_data[i].position[LIGHT_POSITION_X];
		LPOS[i].y = light_data[i].position[LIGHT_POSITION_Y];
		LPOS[i].z = light_data[i].position[LIGHT_POSITION_Z];
		
		
		if( display_numeric.stereo_eye ) {
			//  change position for 3D image
			LPOS[i].x = cos(Y_AXIS_LR_ROTATE)*LPOS[i].x - sin(Y_AXIS_LR_ROTATE)*LPOS[i].z;
			LPOS[i].z = sin(Y_AXIS_LR_ROTATE)*LPOS[i].x + cos(Y_AXIS_LR_ROTATE)*LPOS[i].z;
		}

 		//  color
 		licht[i].red   = light_data[i].getColorValue( 0 );
 		licht[i].green = light_data[i].getColorValue( 1 );
 		licht[i].blue  = light_data[i].getColorValue( 2 );
	}
	
	packLights( ); 
	
	// ------------------------
	//  illumiation components
	// ------------------------
	flag_ambient      = light_illumination_data & light_illumination_ambient_data;
	flag_diffuse      = light_illumination_data & light_illumination_diffuse_data;
	flag_reflected    = light_illumination_data & light_illumination_reflected_data;
	flag_transmitted  = light_illumination_data & light_illumination_transmitted_data;
	
	numeric_multi_root = flag_transmitted;
	
	part_A=((float)light_settings[0].ambient)/100.0;
		
	// fade color
	tocolor[RED]   =(((double)color_gradient_end_data[RED]  )/255.0)*part_A;
	tocolor[GREEN] =(((double)color_gradient_end_data[GREEN])/255.0)*part_A;
	tocolor[BLUE]  =(((double)color_gradient_end_data[BLUE] )/255.0)*part_A;
	
	//  set background color     
	backgr.set( color_background_data[RED]  /255.0,
		    color_background_data[GREEN]/255.0,
		    color_background_data[BLUE] /255.0 );
}

