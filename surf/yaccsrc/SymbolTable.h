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


#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <stdlib.h>

#include <iostream.h>
#include <stdio.h>

class SymbolTable
{
public:
	SymbolTable();
	~SymbolTable();

	const char *lookup (const char *key);
	int lookupInt (const char *key)
		{
			const char *str = lookup (key);
			if (str == 0) {
				cerr << __FILE__ << " " << __LINE__ << " WARNING: lookup of " << key << " failed." << endl;
				return 0;
			}
			else return atoi (str);
		}
	double lookupDouble (const char *key)
		{
			const char *str = lookup (key);
			if (str == 0) {
				cerr << "lookup of " << key << " failed." << endl;
				return 0.0;
			}
			else return atof (str);
			
		}

	void insert (const char *key, const char *value);
	
	void insertInt (const char *key, int val)
		{
			char str[16];
			sprintf (str, "%d", val);
			insert(key, str);
		}

	void insertDouble (const char *key, double val)
		{
			char str[32];
			sprintf (str, "%e", val);
			insert(key, str);
		}

	friend ostream & operator << (ostream &os, const SymbolTable &);

private:
	class Node 
	{
	public:
		Node *left;
		Node *right;
		char *key;
		char *value;

		Node(const char *key, const char *val) ;
		~Node();
		void setValue (const char *val);
		
		void print (ostream &os)
			{
				os << key << "=" << value << ";" << endl;
				if (left)
					left->print(os);
				if (right)
					right->print(os);
			}
	};


	Node *root;
	Node *lookup (const char *key, bool insert);
};

//  extern SymbolTable defaultValues;

#endif
