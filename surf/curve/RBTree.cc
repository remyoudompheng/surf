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



#include <iostream.h>
#include <assert.h>

#include "RBTree.h"


#define RED   RBNode::RED
#define BLACK RBNode::BLACK

RBNode RBNode::sentinel(1); //; = {NIL,NIL,0,RBNode::BLACK};


inline int isLeftChild (RBNode *node)
{
	return node->parent ? node->parent->left == node : 0;
}

inline int isRightChild (RBNode *node)
{
	return node->parent ? node->parent->right == node : 0;
}




void iteratorNext (RBNode *&iterator)
{
	assert(iterator);
	assert(iteratorIsValid(iterator));

	if (iterator->left != NIL) {
		// There are nodes < *iterator...search for the greates of these nodes
		iterator=iterator->left;
		while (iterator->right != NIL)
			iterator=iterator->right;
		return;
	}



	if (isRightChild(iterator)) {
		iterator=iterator->parent;
		return;
	}
	
	if (isLeftChild(iterator)) {
		while (isLeftChild(iterator)) {
			iterator = iterator->parent;
		}
		iterator=iterator->parent;
		if (iterator==0)
			iterator=NIL;
		return;
	}
	iterator=NIL;
}



RBNode * cloneTree (RBNode *root, RBNode *parent, newNodeFunc newNode, copyNodeFunc copyNode)
{
	if (root == NIL)
		return NIL;
	
	RBNode *newRoot = newNode();
	copyNode (newRoot, root);
	newRoot->parent = parent;
	newRoot->color = root->color;
	newRoot->left = cloneTree (root->left, newRoot, newNode, copyNode);
	newRoot->right = cloneTree (root->right, newRoot, newNode, copyNode);
	
	return newRoot;
}


void rotateLeft (RBNode *x, RBNode* &root)
{
	
	/**************************
	 *  rotate node x to left *
	 **************************/
	
	RBNode *y = x->right;
	
	/* establish x->right link */
	x->right = y->left;
	if (y->left != NIL) 
		y->left->parent = x;
	
	/* establish y->parent link */
	if (y != NIL) 
		y->parent = x->parent;
	if (x->parent) {
		if (x == x->parent->left)
			x->parent->left = y;
		else
			x->parent->right = y;
	} else {
		root = y;
	}

	/* link x and y */
	y->left = x;
	if (x != NIL) 
		x->parent = y;
}

void rotateRight(RBNode *x, RBNode* &root) 
{

	/****************************
	 *  rotate node x to right  *
	 ****************************/
	
	RBNode *y = x->left;
	
	/* establish x->left link */
	x->left = y->right;
	if (y->right != NIL) 
		y->right->parent = x;
	
	/* establish y->parent link */
	if (y != NIL) 
		y->parent = x->parent;
	if (x->parent) {
		if (x == x->parent->right)
			x->parent->right = y;
		else
			x->parent->left = y;
	} else {
		root = y;
	}
	
	/* link x and y */
	y->right = x;
	if (x != NIL) 
		x->parent = y;
}


void insertFixup(RBNode *x, RBNode *&root) 
{

	/*************************************
	 *  maintain Red-Black tree balance  *
	 *  after inserting node x           *
	 *************************************/
	
	/* check Red-Black properties */
	while (x != root && x->parent->color == RED) {
		/* we have a violation */
		if (x->parent == x->parent->parent->left) {
			RBNode *y = x->parent->parent->right;
			if (y->color == RED) {
				
				/* uncle is RED */
				x->parent->color = BLACK;
				y->color = BLACK;
				x->parent->parent->color = RED;
				x = x->parent->parent;
			} else {

				/* uncle is BLACK */
				if (x == x->parent->right) {
					/* make x a left child */
					x = x->parent;
					rotateLeft(x, root);
				}

				/* recolor and rotate */
				x->parent->color = BLACK;
				x->parent->parent->color = RED;
				rotateRight(x->parent->parent, root);
			}
		} else {
			
			/* mirror image of above code */
			RBNode *y = x->parent->parent->left;
			if (y->color == RED) {

				/* uncle is RED */
				x->parent->color = BLACK;
				y->color = BLACK;
				x->parent->parent->color = RED;
				x = x->parent->parent;
			} else {

				/* uncle is BLACK */
				if (x == x->parent->left) {
					x = x->parent;
					rotateRight(x, root);
				}
				x->parent->color = BLACK;
				x->parent->parent->color = RED;
				rotateLeft(x->parent->parent, root);
			}
		}
	}
	root->color = BLACK;
}


void deleteFixup(RBNode *x, RBNode *&root) 
{

	/*************************************
	 *  maintain Red-Black tree balance  *
	 *  after deleting node x            *
	 *************************************/

	while (x != root && x->color == BLACK) {
		if (x == x->parent->left) {
			RBNode *w = x->parent->right;
			if (w->color == RED) {
				w->color = BLACK;
				x->parent->color = RED;
				rotateLeft (x->parent, root);
				w = x->parent->right;
			}
			if (w->left->color == BLACK && w->right->color == BLACK) {
				w->color = RED;
				x = x->parent;
			} else {
				if (w->right->color == BLACK) {
					w->left->color = BLACK;
					w->color = RED;
					rotateRight (w, root);
					w = x->parent->right;
				}
				w->color = x->parent->color;
				x->parent->color = BLACK;
				w->right->color = BLACK;
				rotateLeft (x->parent, root);
				x = root;
			}
		} else {
			RBNode *w = x->parent->left;
			if (w->color == RED) {
				w->color = BLACK;
				x->parent->color = RED;
				rotateRight (x->parent, root);
				w = x->parent->left;
			}
			if (w->right->color == BLACK && w->left->color == BLACK) {
				w->color = RED;
				x = x->parent;
			} else {
				if (w->left->color == BLACK) {
					w->right->color = BLACK;
					w->color = RED;
					rotateLeft (w, root);
					w = x->parent->left;
				}
				w->color = x->parent->color;
				x->parent->color = BLACK;
				w->left->color = BLACK;
				rotateRight (x->parent, root);
				x = root;
			}
		}
	}
	x->color = BLACK;
}

void deleteNode(RBNode *z, RBNode *&root, copyNodeFunc copyNode, freeNodeFunc freeNode) 
{
	RBNode *x, *y;
	
	/*****************************
	 *  delete node z from tree  *
	 *****************************/
	
	if (!z || z == NIL) 
		return;
	

	if (z->left == NIL || z->right == NIL) {
		/* y has a NIL node as a child */
		y = z;
	} else {
		/* find tree successor with a NIL node as a child */
		y = z->right;
		while (y->left != NIL) 
			y = y->left;
	}
	
	/* x is y's only child */
	if (y->left != NIL)
		x = y->left;
	else
		x = y->right;
	
	/* remove y from the parent chain */
	x->parent = y->parent;
	if (y->parent) {
		if (y == y->parent->left)
			y->parent->left = x;
		else
			y->parent->right = x;
	} else
		root = x;
	
	if (y != z)
		copyNode (z, y);
		//z->data = y->data;
	

	if (y->color == BLACK)
		deleteFixup (x, root);
	
	freeNode(y);
	// free (y);
}


