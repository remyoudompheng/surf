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

// OCT_QUAN.H - Octree Color Quantization Class

#ifndef OCT_QUAN_H
#define OCT_QUAN_H


typedef unsigned long DWORD;

#define O_MaxDepth 7
#define O_NumColor 215

// ----------------------------------------------------------------
// 24-bit RGB color model
// ----------------------------------------------------------------
class OctColor          
{
  private:  
    byte   red;
    byte   green;
    byte   blue;

  public:
    byte   GetBlue() { return blue; }
    byte   GetGreen() { return green; }
    byte   GetRed() { return red; }
    void   SetBlue ( byte b ) { blue = b; }
    void   SetGreen( byte g ) { green = g; }
    void   SetRed  ( byte r ) { red = r; }
    void   Dither( int,int,double );
};

// ----------------------------------------------------------------
// Octree node 
// ----------------------------------------------------------------
class OctNode           
{                       
  private:
    int      level;          // Node level
    bool     leaf_flag;      // Leaf flag
    bool     mark_flag;      // Marked flag
    DWORD    count;          // Pixel count
    
    struct
    {
      DWORD  red;
      DWORD  green;
      DWORD  blue;
    }

    sum;                     // Summed color value
    int      index;          // Color palette index
    int      children;       // Number of child nodes
    OctNode *pchild[8];      // Children node pointers
    OctNode *pnext;          // Next reducible node pointer
    OctNode *pprev;          // Previous reducible node pointer

    int      TestBit(byte val, int index)
       		{ return ((val & (1 << index)) > index) ? 1 : 0; }

  public:  
    OctNode( int node_level, bool leaf )
    {
      int    i;    	     // Loop index

      level     = node_level;
      leaf_flag = leaf;
      mark_flag = false;
      count     = 0L;
      index     = 0;
      children  = 0;
      
      sum.red = sum.green = sum.blue = 0L;

      for (i = 0; i < 8; i++)
        pchild[i] = NULL;

      pnext = pprev = NULL;
    };


    bool   IsLeaf()   { return leaf_flag; }
    bool   IsMark()   { return mark_flag; }
    DWORD  GetCount() { return count; }

    OctColor GetColor()
    {
      OctColor temp;        // Temporary color

      temp.SetRed  ((byte) (sum.red  / count));
      temp.SetGreen((byte) (sum.green/ count));
      temp.SetBlue ((byte) (sum.blue / count));

      return temp;
    }

    int GetIndex() { return index; }
    int GetLevel() { return level; }


  // Determine child node according to color ------------------
    int FindChild( OctColor &c )
    {
      int index;    			// Child node pointer index

      // Determine child node pointer index
      index = TestBit(c.GetRed(), O_MaxDepth - level) << 2 |
              TestBit(c.GetGreen(), O_MaxDepth - level) << 1 |
              TestBit(c.GetBlue(), O_MaxDepth - level);

      return index;
    }

    OctNode *GetChild( int i ) { return pchild[i]; }
    OctNode *GetNext() { return pnext; }
    OctNode *GetPrev() { return pprev; }
    int GetNumChild() { return children; }


  // Add RGB color to node ----------------------------------
    void AddColor( OctColor &c )

    {
      sum.red   += (DWORD) c.GetRed();
      sum.green += (DWORD) c.GetGreen();
      sum.blue  += (DWORD) c.GetBlue();

      count++;

    }
  // --------------------------------------------------------
    
    void DecNumChild() { children--; }
    void IncNumChild() { children++; }
    
    void SetChild( int i, OctNode *pc ) { pchild[i] = pc; }
    void SetIndex( int i )       { index = i; }
    void SetLeaf ( bool flag   ) { leaf_flag = flag; }
    void SetMark ( bool flag   ) { mark_flag = flag; }
    void SetNext ( OctNode *pn ) { pnext = pn; }
    void SetPrev ( OctNode *pn ) { pprev = pn; }
};


// -----------------------------------------------------------
// Octree color quantization  
// -----------------------------------------------------------
class OctQuant     
{
  private:
    int leaf_level;             	// Leaf level
    int num_leaf;               	// Number of leaf nodes
    
    OctNode *prnl[O_MaxDepth +1]; 	// Reducible node list pointers

  protected:

    bool InsertNode( OctNode *, OctColor & );

    OctNode *MakeNode( int level ) 
    {
      bool leaf;    			// Leaf node flag

      leaf = (level >= leaf_level);

      if (leaf)
        num_leaf++;

      return new OctNode(level, leaf );
    }


    OctNode *GetReducible();     

  // -----------------------------------------------------------
    
    bool GetPixel( int, int,RgbBuffer&, OctColor & );   

    void DeleteNode( OctNode * );
    
    void DeleteTree() { DeleteNode(proot); }
    
    
  // Add node to reducible list  -------------------------------

    void MakeReducible( OctNode *pn )
    {
      int level;       			// Node level
      OctNode *phead;   		// List head node
      
      level = pn->GetLevel();
      phead = prnl[level];
      
      pn->SetNext(phead);

      if (phead != NULL)
        phead->SetPrev(pn);
        
      prnl[level] = pn;
      pn->SetMark(true);
    }

    void ReduceTree();
    
    
  public:
    OctNode *proot;             	// Octree root node pointer  

    static OctColor Palette[O_NumColor];// Color palette 
    
    void FillPalette( OctNode *, int * ); 


  // Quantize color
    int QuantizeColor( OctNode *pn, OctColor &c )  
    {
      int c_index;      		// Child node pointer index

      if ((pn->IsLeaf()) || pn->GetLevel() == leaf_level)
        return pn->GetIndex();
        
      else
      {
        c_index = pn->FindChild(c);

        return QuantizeColor(pn->GetChild(c_index), c);
      }
    }
  // ----------------------------------------------------------
    OctQuant()       
    {
      int i;    			// Loop index
      width = height = 0;
      num_leaf = 0;
      leaf_level = O_MaxDepth + 1;


      // Clear the reducible node list pointers
      for (i = 0; i < leaf_level; i++)
        prnl[i] = NULL;


      // Clear the color palette
      for (i = 0; i < O_NumColor; i++)
      {
        Palette[i].SetRed((byte) 0);
        Palette[i].SetGreen((byte) 0);
        Palette[i].SetBlue((byte) 0);
      }

      proot = NULL;
    }   
 // -------------------------------------------------------
     
    bool BuildTree(RgbBuffer&, bool dither, double ditherval);

    int height;                 	// Bitmap height 
    int width;                  	// Bitmap width  

#if 0
    void MapColors(Display* ,GC ,Pixmap ,Window , unsigned long* ,
       		   int,int,int,int,RgbBuffer&,int,int);   
#endif
};


#endif // OCT_QUAN_H
