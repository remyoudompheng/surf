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



#include <iostream.h>
#include <iostream.h>
#include <assert.h>
#include <stdlib.h>

#include <string.h>
#include "SymbolTable.h"
#include "addDefaultSymbols.h"


inline char *copy (const char *str)
{
	char *retval = new char [strlen(str)+1];
	strcpy (retval, str);
	return retval;
}

SymbolTable::Node::Node(const char *k, const char *val)
{
	left = 0;
	right = 0;
	key = copy(k);
	value = copy(val);
}

void SymbolTable::Node::setValue (const char *val)
{
	delete[] value;
	value = copy (val);
}

SymbolTable::Node::~Node ()
{
	delete [] key;
	delete [] value;
	delete left;
	delete right;
}



SymbolTable::SymbolTable ()
{
	root = 0;
}

SymbolTable::~SymbolTable ()
{
	delete root;
}

const char *SymbolTable::lookup (const char *key)
{
	Node *node = lookup (key, false);
	if (node) {
		return node->value;
	} else
		return 0;
}


void SymbolTable::insert (const char *key, const char *value)
{
	Node *n = lookup (key, true);
	n->setValue(value);
}

SymbolTable::Node * SymbolTable::lookup (const char *key, bool insert)
{
	assert(key);
	if (root == 0) {
		if (insert) {
			root = new Node (key, "");
			return root;
		} else
			return 0;
	}
	
	Node *node = root;	
	while (true) {
		int cmp = strcmp(node->key, key);
		if (cmp == 0)
			return node;
		
		if (cmp < 0) {
			if (node->left == 0) {
				if (insert) {
					return node->left = new Node (key, "");
				} else
					return 0;
			} else
				node = node->left;
		} else {
			if (node->right == 0) {
				if (insert) {
					return node->right = new Node (key, "");
				} else
					return 0;
			} else
				node = node->right;
		}
	}
}


ostream & operator << (ostream &os, const SymbolTable &st)
{
	if (st.root)
		st.root->print(os);
	return os;
}

