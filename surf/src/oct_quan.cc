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


// Octree Color Quantization Class

#include <oct_quan.h>
#include <def.h>
#include <compfn.h>
#include <RgbBuffer.h>
#include <color.h>
#include <float_buffer.h>

#include <cmath>

// Color palette  ---------------------------------------------------------

OctColor OctQuant::Palette[O_NumColor];

void OctColor::Dither(int x, int y, double display_dither_value_data)
{
        float rr = float(red)/255.0;
        float gg = float(green)/255.0;
        float bb = float(blue)/255.0;

        rr *= display_dither_value_data;
        gg *= display_dither_value_data;
        bb *= display_dither_value_data;

        float c = compare_function[1][2](x, y);

        if(rr - int(rr) > c) {
		rr += 1.0;
	}
        if(gg - int(gg) > c) {
		gg += 1.0;
	}
        if(bb - int(bb) > c) {
		bb += 1.0;
	}

	rr = float(int(rr))/(display_dither_value_data + 1)*255;
	gg = float(int(gg))/(display_dither_value_data + 1)*255;
	bb = float(int(bb))/(display_dither_value_data + 1)*255;

        red = byte(rr);
        green = byte(gg);
        blue = byte(bb);
}

// Build octree  

bool OctQuant::BuildTree(RgbBuffer& optimizecolor, bool dither, double ditherval)
{
	bool status = true;    		// Return status
	OctColor rgbcolor;         		// Pixel color

	// Allocate octree root node

	if((proot = MakeNode(0)) == 0) {
		status = false;
	}

	// Build the octree

	for(int row = 0; row < height && status; row++) {
		for(int col = 0; col < width; col++) {
			if(!GetPixel(col, row, optimizecolor, rgbcolor)) {
				status = false;
				break;
			}
			if(dither) {
				rgbcolor.Dither(col, row, ditherval);
			}

			// Insert pixel color into octree
			if(!InsertNode(proot, rgbcolor)) {
				DeleteNode(proot); // Delete the octree
				status = false;
				break;
			}

			// Reduce octree if too many colors
			if(num_leaf > O_NumColor) {
				ReduceTree();
			}
		}
	}
	return status;
}




// Recursively delete octree nodes:

void OctQuant::DeleteNode(OctNode* pn)
{
	if(pn == 0) {
		return;
	}

	if(!pn->IsLeaf()) {
		// Delete child nodes
		for(int i = 0; i < 8; i++) {
			OctNode* pc = pn->GetChild(i);
			if(pc != 0) {
				DeleteNode(pc);
				pn->SetChild(i, 0);
				pn->DecNumChild();
			}
		}
	} else {
		num_leaf--;
	}

	delete pn;
}



// Set color palette entries:

void OctQuant::FillPalette(OctNode* pn, int* pindex)
{
	if(pn == 0) {
		return;
	}

	// Perform recursive depth-first traversal of octree
	if(pn->IsLeaf() || pn->GetLevel() == leaf_level) {
		// Set color palette entry
		Palette[*pindex] = pn->GetColor();

		// Set node color palette index
		pn->SetIndex(*pindex);

		// Advance to next color palette entry
		*pindex = *pindex + 1;
	} else {
		// Visit child nodes
		for(int i = 0; i < 8; i++) {
			FillPalette(pn->GetChild(i), pindex);
		}
	}
}


// Get next reducible node pointer:

OctNode* OctQuant::GetReducible()
{
	// Find lowest reducible node level 
	int new_level = leaf_level - 1;
	while(prnl[new_level] == 0) {
		new_level--;
	}

	// Find node with largest pixel count
	OctNode* prn = prnl[new_level];

	OctNode *plcn = 0; // Largest pixel count node
	while(prn != 0) {
		if(plcn == 0) {
			plcn = prn;
		} else if(prn->GetCount() < plcn->GetCount()) {
			plcn = prn;
		}
		prn = prn->GetNext();
	}

  
	// Remove node from reducible list
	OctNode* pnext = plcn->GetNext();
	OctNode* pprev = plcn->GetPrev();

	if(pprev == 0) {
		prnl[new_level] = pnext;
    
		if(pnext != 0) {
			pnext->SetPrev(0);
		}
	} else {
		pprev->SetNext(pnext);

		if(pnext != 0) {
			pnext->SetPrev(pprev);
		}
      
	}

	plcn->SetNext(0);
	plcn->SetPrev(0);
	plcn->SetMark(false);

	return plcn;
}


// Reduce octree:

void OctQuant::ReduceTree()     
{
	OctNode* pn = GetReducible();  		// Get next reducible node

	// Delete children
	for(int i = 0; i < 8; i++) {
		OctNode* pc = pn->GetChild(i);
		if(pc != 0) {
			DeleteNode(pc);			
			pn->SetChild(i, 0);
			pn->DecNumChild();
		}
	}

	pn->SetLeaf(true);    		// Mark node as leaf
	num_leaf++;           		// Increment leaf count


	// Update reduction and leaf levels
	if(pn->GetLevel() < (leaf_level - 1)) {
		leaf_level = pn->GetLevel() + 1;
	}
}



// Insert node into octree:

bool OctQuant::InsertNode(OctNode* pn, OctColor& c)
{
	bool status = true;   		// Return status
	int level = pn->GetLevel();   		// Get node level
	pn->AddColor(c);          		// Add RGB color to node

	if(!pn->IsLeaf() && level < leaf_level) {
		// Find child node
		int c_index = pn->FindChild(c);

		OctNode* pc = pn->GetChild(c_index);
		if(pc != 0) {
			if((pn->GetNumChild() > 1) && (!pn->IsMark())) {
				// Mark node as candidate for reduction
				MakeReducible(pn);
			}
		} else {
			// Allocate child node
			pc = MakeNode(level + 1);
			if(pc == 0) {
				return false;
			}

			// Set child node pointer
			pn->SetChild(c_index, pc);
			
			pn->IncNumChild(); // Increment child count
		}
		
		// Insert child node into octree
		status = InsertNode(pc, c);
	}

	return status;
}


bool OctQuant::GetPixel(int x, int y, RgbBuffer& picture, OctColor& getcolor)
{
	colorrgb color_opt;
   
	color_opt.set(double(picture.Get_one(x, y, red)),
		      double(picture.Get_one(x,y,green)),
		      double(picture.Get_one(x,y,blue)));	

	getcolor.SetRed(byte(std::floor(color_opt.red)));
	getcolor.SetGreen(byte(std::floor(color_opt.green)));
	getcolor.SetBlue(byte(std::floor(color_opt.blue)));

	return true;
}
