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





// ----------------------------------------------------------------------------
// file                oct_qaun.cc
// implementation by kai
// date                25.06.97
// sk :ganzer File neu
// ----------------------------------------------------------------------------

// OCT_QUAN.CPP - Octree Color Quantization Class

#ifndef OCT_QU_H
#define OCT_QU_H

#include <math.h>     
#include <stdio.h>
#include "def.h"
#include "compfn.h"      

#include "RgbBuffer.h"
#include "color.h"        
#include "float_buffer.h"
#include "oct_quan.h"


// Color palette  ---------------------------------------------------------

OctColor OctQuant::Palette[O_NumColor];

void   OctColor::Dither( int x,int y, double display_dither_value_data)
{
        float rr=((float)red  )/255.0;
        float gg=((float)green)/255.0;
        float bb=((float)blue )/255.0;

        rr *= display_dither_value_data;
        gg *= display_dither_value_data;
        bb *= display_dither_value_data;

        float  c = compare_function[1][2]( x,y );

        if( rr - (int)rr > c ) rr += 1.0;
        if( gg - (int)gg > c ) gg += 1.0;
        if( bb - (int)bb > c ) bb += 1.0;

        rr = ((float)((int)rr))/(display_dither_value_data+1)*255;
        gg = ((float)((int)gg))/(display_dither_value_data+1)*255;
        bb = ((float)((int)bb))/(display_dither_value_data+1)*255;

        red   = (byte)rr;
        green = (byte)gg;
        blue  = (byte)bb;
}

// ------------------------------------------------------------------------
// Build octree  
// ------------------------------------------------------------------------

bool OctQuant::BuildTree(RgbBuffer &optimizecolor, bool dither, double ditherval)   
{
    int      row;              		// Row counter
    int      col;              		// Column counter
    bool     status = true;    		// Return status
    OctColor rgbcolor;         		// Pixel color

    // Allocate octree root node

    if ((proot = MakeNode(0)) == NULL)
    {
        status = false;
    }

    // Build the octree

    if ( status )
    {
        for( row = 0; row < height; row++ )
        {
            for( col = 0; col < width; col++ )
            {
                if( !GetPixel( col,row,optimizecolor,rgbcolor ))
                {
                    status = false;
                    break;
                }
		if (dither)
			rgbcolor.Dither( col,row, ditherval );

                // Insert pixel color into octree
                if( !InsertNode( proot,rgbcolor ))
                { 
                    DeleteNode(proot);      	// Delete the octree
                    status =  false;
                    break;
                }

            // Reduce octree if too many colors
            if (num_leaf > O_NumColor)
            ReduceTree();
            }

        if (!status)
        break;
        }
    }
    return status;
}



// ------------------------------------------------------------------------
// Recursively delete octree nodes  
// ------------------------------------------------------------------------
void OctQuant::DeleteNode( OctNode *pn )
{
  int      i;   			// Loop index
  OctNode *pc;  			// Child node pointer

  if (pn == NULL)
    return;

  if (!pn->IsLeaf())
  {
    // Delete child nodes
    for (i = 0; i < 8; i++)
    {
      if ((pc = pn->GetChild(i)) != NULL)
      {
        DeleteNode(pc);
        pn->SetChild(i, NULL);
        pn->DecNumChild();
      }
    }
  }
  else
    num_leaf--;

  delete pn;
}



// ------------------------------------------------------------------------
// Set color palette entries   
// ------------------------------------------------------------------------
void OctQuant::FillPalette( OctNode *pn, int *pindex)
{
  int i;    				// Loop index

  // Perform recursive depth-first traversal of octree
  if (pn != NULL)
  {
    if ((pn->IsLeaf()) || pn->GetLevel() ==
        leaf_level)
    {
      // Set color palette entry
      Palette[*pindex] = pn->GetColor();

      // Set node color palette index
      pn->SetIndex(*pindex);

      // Advance to next color palette entry
      *pindex = *pindex + 1;
    }
    else
    {
      // Visit child nodes
      for (i = 0; i < 8; i++)
        FillPalette(pn->GetChild(i), pindex);
    }
  }
}



// ------------------------------------------------------------------------
// Get next reducible node pointer  
// ------------------------------------------------------------------------
OctNode *OctQuant::GetReducible()
{
  int      new_level;   		// New reducible node level
  
  OctNode *prn;         		// Reducible node pointer
  OctNode *plcn = NULL; 		// Largest pixel count node
  OctNode *pnext;       		// Next node
  OctNode *pprev;       		// Previous node

  new_level = leaf_level - 1;

  // Find lowest reducible node level 
  while (prnl[new_level] == NULL)
    new_level--;


  // Find node with largest pixel count
  prn = prnl[new_level];

  while (prn != NULL)
  {
    if (plcn == NULL)
      plcn = prn;

    else if (prn->GetCount() < plcn->GetCount())
      plcn = prn;

    prn = prn->GetNext();
  }

  
  // Remove node from reducible list
  pnext = plcn->GetNext();
  pprev = plcn->GetPrev();

  if (pprev == NULL)
  {
    prnl[new_level] = pnext;
    
    if (pnext != NULL)
      pnext->SetPrev(NULL);
      
  }
  else
  {
    pprev->SetNext(pnext);

    if (pnext != NULL)
      pnext->SetPrev(pprev);
      
  }

  plcn->SetNext(NULL);
  plcn->SetPrev(NULL);
  plcn->SetMark(false);

  return plcn;
}


// ------------------------------------------------------------------------
// Reduce octree   
// ------------------------------------------------------------------------
void OctQuant::ReduceTree()     
{
  int      i;        			// Loop index

  OctNode *pn;       			// Node pointer
  OctNode *pc;       			// Child node pointer

  pn = GetReducible();  		// Get next reducible node

  // Delete children
  for (i = 0; i < 8; i++)
  {
    if ((pc = pn->GetChild(i)) != NULL)
    {
      DeleteNode(pc);
      pn->SetChild(i, NULL);
      pn->DecNumChild();
    }
  }

  pn->SetLeaf(true);    		// Mark node as leaf
  num_leaf++;           		// Increment leaf count


  // Update reduction and leaf levels
  if (pn->GetLevel() < (leaf_level - 1))
    leaf_level = pn->GetLevel() + 1;
}


// ------------------------------------------------------------------------
// Insert node into octree 
// ------------------------------------------------------------------------
bool OctQuant::InsertNode( OctNode *pn, OctColor &c)
{
  int      c_index;         		// Child index
  int      level;           		// Node level
  bool     status = true;   		// Return status
  OctNode *pc;              		// Child node pointer

  level = pn->GetLevel();   		// Get node level
  pn->AddColor(c);          		// Add RGB color to node

  if (!pn->IsLeaf() && level < leaf_level)
  {
    // Find child node
    c_index = pn->FindChild(c);

    if ((pc = pn->GetChild(c_index)) != NULL)
    {
      if ((pn->GetNumChild() > 1) && (!pn->IsMark()))
      {
        // Mark node as candidate for reduction
        MakeReducible(pn);
      }
    }
    else
    {
      // Allocate child node
      if ((pc = MakeNode(level + 1)) == NULL)
        return false;

      // Set child node pointer
      pn->SetChild(c_index, pc);

      pn->IncNumChild();      		// Increment child count
    }

    // Insert child node into octree
    status = InsertNode(pc, c);

  }

  return status;
}

// ------------------------------------------------------------------------
// GetPixel(x,y,rgb,getcolor) 
// ------------------------------------------------------------------------ 
bool OctQuant::GetPixel(int x,int y,RgbBuffer& picture, OctColor &getcolor )
{
   colorrgb color_opt;
   
   color_opt.set((double)picture.Get_one(x,y,red  ),
                 (double)picture.Get_one(x,y,green),
                 (double)picture.Get_one(x,y,blue ) );	

   getcolor.SetRed  ( (byte)floor(color_opt.red  ) );
   getcolor.SetGreen( (byte)floor(color_opt.green) );
   getcolor.SetBlue ( (byte)floor(color_opt.blue ) );

   return true;
}

#if 0
// ------------------------------------------------------------------------
// MapColors() 
// ------------------------------------------------------------------------
void OctQuant::MapColors(Display *dpy,GC gc,Pixmap pixmap,
                	 Window win, unsigned long *pixel,
                	int y_min,int y_max,int x_min,int x_max,
                        RgbBuffer& color_to_map,int jump_mapc,
                        int show)
{
   int index;
   int m=MAIN_MEM_LAST_MOSAIC_INDEX;    
   int anfang_ende=0,step_mos=0;
   int x_m,y_m;
   int a,b,c,count=0;
   OctColor rgb;
   
   // wenn Fläche in grober Auflösung berechnet wurde und danach cut_surface
   // aufgerufen wird ( dabei wird automat. mit 1x1 Auflösung gerechnet ) 
   // dann werden nur noch die einzelen Punkte gezeichnet
   // Darum wird versucht genau diese Mosaicgröße wieder auszugeben
   // search for smallest Mosaic square
   while (anfang_ende != 2 ) 
   {
     m--;
     if( (main_mosaic_choice_data[m] != 0 && anfang_ende==0 ) || 
         (main_mosaic_choice_data[m-1] == 0 && anfang_ende==1 ) ) anfang_ende++;
     if( m==1 && anfang_ende!=2 ) {anfang_ende=2;m--;}
   }
   step_mos= (int)pow(3,m);
   
   // go through all lines with distance of smallest mosaic
   for(int y_i=y_min+(jump_mapc-1)/2 ; y_i < y_max ; y_i+=jump_mapc,count++)  
   {
      // do each pixel of lines with distance of smallest mosaic
      for(int x_i=x_min+(jump_mapc-1)/2 ; x_i < x_max ; x_i+=jump_mapc)   
      {  
          a=color_to_map.Get_one(x_i, y_i ,red);
          b=color_to_map.Get_one(x_i, y_i ,green);
          c=color_to_map.Get_one(x_i, y_i ,blue);
          
          if ( m!=0 && a==0 && b==0 && c==0)
          {
          x_m=(step_mos-1)/2 + ((int)(x_i/step_mos))*step_mos;
          y_m=(step_mos-1)/2 + ((int)(y_i/step_mos))*step_mos;
          a=color_to_map.Get_one(x_m, y_m ,red);
          b=color_to_map.Get_one(x_m, y_m ,green);
          c=color_to_map.Get_one(x_m, y_m ,blue);          
          }
          
          rgb.SetRed   (a);
          rgb.SetGreen (b);
          rgb.SetBlue  (c);
       
          rgb.Dither( x_i,y_i );

          index=QuantizeColor(proot,rgb);
          
          XSetForeground(dpy,gc,pixel[index]);
          XFillRectangle( dpy,pixmap,gc,x_i-(jump_mapc-1)/2,
                	  y_i-(jump_mapc-1)/2,jump_mapc,jump_mapc ); 
      }
      //XCopyArea(dpy,pixmap,win,gc,
      //    	   x_min,y_i-(jump_mapc-1)/2,
      //             x_max-x_min,jump_mapc,
      //             x_min,y_i-(jump_mapc-1)/2 );
      if( count%5 == 4 )
      {
          notify_dispatch( );
          XFlush( dpy );
      }
   }
   if( show )
   {
       XCopyArea(dpy,pixmap,win,gc,
          	   x_min,y_min,
                   x_max-x_min,y_max-y_min,
                   x_min,y_min );
       XFlush( dpy );
   }
} 
#endif


#endif //OCT_QU_H

