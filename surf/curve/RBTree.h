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



#ifndef RBTREE_H
#define RBTREE_H

#include <assert.h>
#include <iostream.h>

#include "debug.h"

class RBNode;
typedef void (*copyNodeFunc)(RBNode *dst, const RBNode *src);
typedef void (*freeNodeFunc)(RBNode *node);
typedef RBNode* (*newNodeFunc) (void);

class RBNode;

#define NIL &RBNode::sentinel

// extern void check(RBNode *root);

class RBNode
{
public:
	RBNode () {left=right=NIL; parent=0; color=RED;};

	RBNode *left;
	RBNode *right;
	RBNode *parent;
	enum {BLACK, RED} color;

	static RBNode sentinel;
private:

	RBNode(int) {left=right=NIL; parent=0; color = BLACK;};


};

inline void iteratorInit (RBNode *&iterator, RBNode *root)
{
	assert(root);
	iterator = root;
	while (iterator->right != NIL) 
		iterator=iterator->right;
}

inline int iteratorIsValid (RBNode *iterator)
{
	return iterator != NIL;
}

extern void iteratorNext (RBNode *&iterator);



extern void deleteNode	(RBNode *z, RBNode *&root,  copyNodeFunc copyNode, freeNodeFunc freeNode);
void insertFixup(RBNode *x, RBNode *&root) ; 

RBNode * cloneTree (RBNode *root, RBNode *parent, newNodeFunc newNode, copyNodeFunc copyNode);

extern int countNodes (RBNode *root);


#endif
