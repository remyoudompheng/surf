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


// OCT_QUAN.H - Octree Color Quantization Class

#ifndef OCT_QUAN_H
#define OCT_QUAN_H

#include <mytypes.h> // byte typedef
#include <RgbBuffer.h>

#define O_MaxDepth 7
#define O_NumColor 215

typedef unsigned long DWORD;

// 24-bit RGB color model:

class OctColor {
public:
	OctColor() {}
	OctColor(byte r, byte g, byte b)
		: red(r), green(g), blue(b) {}

	byte GetBlue() const {
		return blue;
	}
	byte GetGreen() const {
		return green;
	}
	byte GetRed() const {
		return red;
	}
	void SetBlue(byte b) {
		blue = b;
	}
	void SetGreen(byte g) {
		green = g;
	}
	void SetRed(byte r) {
		red = r;
	}

	void Dither(int, int, double);

private:  
	byte red;
	byte green;
	byte blue;
};

// Octree node:

class OctNode {                       
public:  
	OctNode(int node_level, bool leaf) 
		: level(node_level), leaf_flag(leaf), mark_flag(false),
		  count(0), index(0), children(0),
		  pnext(0), pprev(0) {
		sum.red = sum.green = sum.blue = 0;
		for (int i = 0; i < 8; i++) {
			pchild[i] = 0;
		}
	}

	bool IsLeaf() const  {
		return leaf_flag;
	}
	bool IsMark() const {
		return mark_flag;
	}
	DWORD GetCount() const {
		return count;
	}

	OctColor GetColor() const {
		return OctColor(sum.red/count, sum.green/count, sum.blue/count);
	}
	int GetIndex() const {
		return index;
	}
	int GetLevel() const {
		return level;
	}


	// Determine child node according to color ------------------
	int FindChild(const OctColor& c) const {
		int index;    // Child node pointer index

		// Determine child node pointer index
		index = TestBit(c.GetRed(), O_MaxDepth - level) << 2 |
			TestBit(c.GetGreen(), O_MaxDepth - level) << 1 |
			TestBit(c.GetBlue(), O_MaxDepth - level);

		return index;
	}

	OctNode* GetChild(int i) {
		return pchild[i];
	}
	OctNode* GetNext() {
		return pnext;
	}
	OctNode* GetPrev() {
		return pprev;
	}
	int GetNumChild() const {
		return children;
	}


	// Add RGB color to node ----------------------------------
	void AddColor(const OctColor& c) {
		sum.red   += c.GetRed();
		sum.green += c.GetGreen();
		sum.blue  += c.GetBlue();
		count++;
	}
	// --------------------------------------------------------
    
	void DecNumChild() {
		children--;
	}
	void IncNumChild() {
		children++;
	}
    
	void SetChild(int i, OctNode* pc) {
		pchild[i] = pc;
	}
	void SetIndex(int i) {
		index = i;
	}
	void SetLeaf(bool flag) {
		leaf_flag = flag;
	}
	void SetMark(bool flag) {
		mark_flag = flag;
	}
	void SetNext(OctNode* pn) {
		pnext = pn;
	}
	void SetPrev(OctNode* pn) {
		pprev = pn;
	}

private:
	int level;
	bool leaf_flag;
	bool mark_flag;
	DWORD count; // Pixel count
    
	struct {
		DWORD red;
		DWORD green;
		DWORD blue;
	} sum;               // Summed color value
	int index;               // Color palette index
	int children;            // Number of child nodes
	OctNode* pchild[8];      // Children node pointers
	OctNode* pnext;          // Next reducible node pointer
	OctNode* pprev;          // Previous reducible node pointer

	int TestBit(byte val, int index) const {
       		return ((val & (1 << index)) > index) ? 1 : 0;
	}
};



// Octree color quantization:

class OctQuant {
public:
	OctNode* proot;       	// Octree root node pointer  

	static OctColor Palette[O_NumColor]; // Color palette 
    
	void FillPalette(OctNode*, int*); 

	// Quantize color
	int QuantizeColor(OctNode* pn, OctColor& c) {
		if(pn->IsLeaf() || pn->GetLevel() == leaf_level) {
			return pn->GetIndex();
		} else {
			int c_index = pn->FindChild(c);
			return QuantizeColor(pn->GetChild(c_index), c);
		}
	}
	
	// ----------------------------------------------------------
	OctQuant()
		: width(0), height(0),
		  num_leaf(0), leaf_level(O_MaxDepth + 1) {

		// Clear the reducible node list pointers
		for(int i = 0; i < leaf_level; i++) {
			prnl[i] = 0;
		}

		// Clear the color palette
		for(int i = 0; i < O_NumColor; i++) {
			Palette[i].SetRed(0);
			Palette[i].SetGreen(0);
			Palette[i].SetBlue(0);
		}

		proot = 0;
	}   
	// -------------------------------------------------------
     
	bool BuildTree(RgbBuffer&, bool dither, double ditherval);

	int width;                  	// Bitmap width  
	int height;                 	// Bitmap height 

protected:

	bool InsertNode(OctNode*, OctColor&);

	OctNode* MakeNode(int level) {
		bool leaf = (level >= leaf_level);

		if(leaf) {
			num_leaf++;
		}

		return new OctNode(level, leaf);
	}

	OctNode* GetReducible();

	// -----------------------------------------------------------
    
	bool GetPixel(int, int, RgbBuffer&, OctColor&);   

	void DeleteNode(OctNode*);
    
	void DeleteTree() {
		DeleteNode(proot);
	}
    
    
	// Add node to reducible list  -------------------------------

	void MakeReducible(OctNode* pn) {
		int level = pn->GetLevel();
		OctNode* phead = prnl[level];
      
		pn->SetNext(phead);

		if(phead != 0) {
			phead->SetPrev(pn);
		}
        
		prnl[level] = pn;
		pn->SetMark(true);
	}

	void ReduceTree();
    
    
private:
	int num_leaf;               	// Number of leaf nodes
	int leaf_level;             	// Leaf level
    
	OctNode* prnl[O_MaxDepth + 1]; 	// Reducible node list pointers
};

#endif // OCT_QUAN_H
